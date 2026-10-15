from __future__ import annotations

import json
import math
import re
import socket
import struct
import time
from enum import IntEnum
from typing import Any


class Command(IntEnum):
    CREATE_SESSION = 1
    EVALUATE = 2
    SET_CHART_EXPRESSIONS = 3
    GET_CHART_DATA = 4
    CLEAR_CHART_DATA = 5
    CLOSE_SESSION = 6
    KEEP_ALIVE = 7
    GET_MESSAGES = 8
    POLL = 9
    EXEC_CONTROLLER_COMMAND = 10
    GET_RELOAD_OBJECTS = 11
    BROWSE_VARIABLES = 12
    SET_VARIABLE = 13
    EXECUTE = 14


VARIABLE_TYPES = ("number", "boolean", "string", "nil")
MAX_REQUEST_PATH_BYTES = 1024
# Same ASCII decimal/scientific shape the kernel accepts: no leading
# '+', whitespace, underscores or non-ASCII digits.
_NUMBER_PATTERN = re.compile(
    r"-?(?:[0-9]+\.?[0-9]*|\.[0-9]+)(?:[eE][+-]?[0-9]+)?"
)


def _check_path(expression: str) -> None:
    if (
        not isinstance(expression, str)
        or not expression
        or len(expression.encode("utf-8")) > MAX_REQUEST_PATH_BYTES
    ):
        raise ValueError("Некорректное выражение для запроса")


class ProtocolError(RuntimeError):
    pass


class DebuggerProtocol:
    SERVICE_ID = 2
    FRAME_SINGLE = 1
    ACK_ERROR = 7
    ACK_OK = 12
    ACCEPT_MESSAGE = b"PAC accept"

    def __init__(self, timeout: float = 3.0) -> None:
        self.timeout = timeout
        self.session_id: str | None = None
        self.session_timeout_ms = 10_000
        self.controller_time_unix_ms: int | None = None
        self.controller_time_millisec: int | None = None
        self.client_time_unix_ms: int | None = None
        self.client_time_millisec: int | None = None
        self._socket: socket.socket | None = None
        self._packet_id = 0

    @property
    def connected(self) -> bool:
        return self._socket is not None and self.session_id is not None

    def connect(self, host: str, port: int = 10_000) -> str:
        self.disconnect(send_close=False)
        connection = socket.create_connection((host, port), timeout=self.timeout)
        connection.settimeout(self.timeout)
        try:
            connection.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
            greeting = self._recv_exact(connection, len(self.ACCEPT_MESSAGE))
            if greeting != self.ACCEPT_MESSAGE:
                raise ProtocolError("Контроллер не прислал приветствие PAC accept")
            self._socket = connection
            request_started_ms = time.time_ns() // 1_000_000
            response = self._request(Command.CREATE_SESSION)
            request_finished_ms = time.time_ns() // 1_000_000
            self._ensure_ok(response)
            try:
                self.session_id = str(response["session_id"])
                self.session_timeout_ms = int(response.get("timeout_ms", 10_000))
                self.controller_time_unix_ms = int(
                    response["controller_time_unix_ms"]
                )
                self.controller_time_millisec = int(
                    response["controller_time_millisec"]
                )
                self.client_time_unix_ms = (
                    request_started_ms + request_finished_ms
                ) // 2
                self.client_time_millisec = self.controller_time_millisec
            except (KeyError, TypeError, ValueError) as error:
                raise ProtocolError(
                    "Контроллер не прислал временной якорь сессии"
                ) from error
            return self.session_id
        except Exception:
            connection.close()
            self._socket = None
            raise

    def disconnect(self, *, send_close: bool = True) -> None:
        connection = self._socket
        if connection is None:
            self.session_id = None
            self.controller_time_unix_ms = None
            self.controller_time_millisec = None
            self.client_time_unix_ms = None
            self.client_time_millisec = None
            return
        if send_close and self.session_id:
            try:
                self._request(Command.CLOSE_SESSION)
            except (OSError, ProtocolError):
                pass
        try:
            connection.shutdown(socket.SHUT_RDWR)
        except OSError:
            pass
        connection.close()
        self._socket = None
        self.session_id = None
        self.controller_time_unix_ms = None
        self.controller_time_millisec = None
        self.client_time_unix_ms = None
        self.client_time_millisec = None

    def evaluate(self, expression: str) -> dict[str, Any]:
        return self._request(Command.EVALUATE, expression)

    def execute(self, code: str) -> dict[str, Any]:
        if not code.strip() or len(code.encode("utf-8")) > 16384:
            raise ValueError("Lua-код должен содержать от 1 до 16384 байт")
        return self._request(Command.EXECUTE, code)

    def execute_controller_command(self, command_id: int) -> dict[str, Any]:
        response = self._request(Command.EXEC_CONTROLLER_COMMAND, str(command_id))
        self._ensure_ok(response)
        return response

    def get_reload_objects(self) -> list[dict[str, Any]]:
        response = self._request(Command.GET_RELOAD_OBJECTS)
        self._ensure_ok(response)
        return response["objects"]

    def browse_variables(
        self, expression: str = "_G", offset: int = 0
    ) -> dict[str, Any]:
        _check_path(expression)
        if isinstance(offset, bool) or not isinstance(offset, int) or offset < 0:
            raise ValueError("Некорректное смещение обзора")
        response = self._request(
            Command.BROWSE_VARIABLES, f"{expression}\n{offset}"
        )
        self._ensure_ok(response)
        return response

    def set_variable(
        self, expression: str, value_type: str, value: str
    ) -> dict[str, Any]:
        _check_path(expression)
        # The target must not inject extra lines into the framed payload.
        if "\n" in expression or "\r" in expression:
            raise ValueError("Некорректная цель записи")
        if value_type not in VARIABLE_TYPES:
            raise ValueError(f"Неизвестный тип значения: {value_type}")
        if not isinstance(value, str):
            raise ValueError("Значение должно быть строкой")
        if value_type == "nil" and value:
            raise ValueError("Значение nil не имеет данных")
        if value_type == "boolean" and value not in ("true", "false"):
            raise ValueError("Логическое значение должно быть true или false")
        if value_type == "number":
            if not _NUMBER_PATTERN.fullmatch(value):
                raise ValueError("Некорректное число")
            if not math.isfinite(float(value)):
                raise ValueError("Некорректное число")
        response = self._request(
            Command.SET_VARIABLE, f"{expression}\n{value_type}\n{value}"
        )
        self._ensure_ok(response)
        return response

    def set_expressions(self, expressions: list[str]) -> dict[str, Any]:
        response = self._request(
            Command.SET_CHART_EXPRESSIONS, "\n".join(expressions)
        )
        self._ensure_ok(response)
        return response

    def get_chart_data(self) -> dict[str, Any]:
        response = self._request(Command.GET_CHART_DATA)
        self._ensure_ok(response)
        self._attach_time_anchor(response)
        return response

    def clear_chart_data(self) -> None:
        self._ensure_ok(self._request(Command.CLEAR_CHART_DATA))

    def get_messages(self) -> dict[str, Any]:
        response = self._request(Command.GET_MESSAGES)
        self._ensure_ok(response)
        self._attach_time_anchor(response)
        return response

    def poll(self) -> dict[str, Any]:
        response = self._request(Command.POLL)
        self._ensure_ok(response)
        for document in (response, response["events"]):
            self._attach_time_anchor(document)
        return response

    def _attach_time_anchor(self, document: dict[str, Any]) -> None:
        document["controller_time_unix_ms"] = self.controller_time_unix_ms
        document["controller_time_millisec"] = self.controller_time_millisec
        document["client_time_unix_ms"] = self.client_time_unix_ms
        document["client_time_millisec"] = self.client_time_millisec

    def keep_alive(self) -> None:
        self._ensure_ok(self._request(Command.KEEP_ALIVE))

    def _request(self, command: Command, text: str = "") -> dict[str, Any]:
        connection = self._socket
        if connection is None:
            raise ProtocolError("Нет подключения к контроллеру")

        if command is not Command.CREATE_SESSION:
            if not self.session_id:
                raise ProtocolError("Сессия отладчика не создана")
            text = f"{self.session_id}\n{text}"

        payload = bytes((int(command),)) + text.encode("utf-8")
        if len(payload) > 0xFFFF:
            raise ProtocolError("Запрос слишком большой")

        self._packet_id = self._packet_id % 255 + 1
        frame = struct.pack(
            ">cBBBH",
            b"s",
            self.SERVICE_ID,
            self.FRAME_SINGLE,
            self._packet_id,
            len(payload),
        ) + payload
        connection.sendall(frame)

        header = self._recv_exact(connection, 5)
        net_id, acknowledgement, packet_id, length = struct.unpack(">cBBH", header)
        if net_id != b"s" or packet_id != self._packet_id:
            raise ProtocolError("Получен ответ от другого пакета")
        response_payload = self._recv_exact(connection, length)
        if acknowledgement == self.ACK_ERROR:
            code = response_payload[0] if response_payload else -1
            raise ProtocolError(f"Ошибка протокола PAC: {code}")
        if acknowledgement != self.ACK_OK:
            raise ProtocolError(f"Неизвестный тип ответа PAC: {acknowledgement}")

        try:
            return json.loads(response_payload.rstrip(b"\0").decode("utf-8"))
        except (UnicodeDecodeError, json.JSONDecodeError) as error:
            raise ProtocolError("Некорректный JSON в ответе контроллера") from error

    @staticmethod
    def _recv_exact(connection: socket.socket, size: int) -> bytes:
        chunks = bytearray()
        while len(chunks) < size:
            chunk = connection.recv(size - len(chunks))
            if not chunk:
                raise ProtocolError("Контроллер закрыл соединение")
            chunks.extend(chunk)
        return bytes(chunks)

    @staticmethod
    def _ensure_ok(response: dict[str, Any]) -> None:
        if not response.get("ok"):
            raise ProtocolError(str(response.get("error", "Неизвестная ошибка")))
