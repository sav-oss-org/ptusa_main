from __future__ import annotations

from PySide6.QtCore import QObject, QTimer, Signal, Slot

from .protocol import DebuggerProtocol, ProtocolError


class DebuggerWorker(QObject):
    connected = Signal(str)
    disconnected = Signal(str)
    chart_data = Signal(dict)
    messages = Signal(dict)
    evaluated = Signal(str, dict)
    executed = Signal(str, dict)
    command_executed = Signal(int, dict)
    reload_objects_loaded = Signal(list)
    reload_objects_failed = Signal(str)
    browse_loaded = Signal(str, int, int, dict)
    browse_failed = Signal(str, int, int, str)
    assignment_done = Signal(str, int, dict)
    assignment_failed = Signal(str, int, str)
    error = Signal(str)

    def __init__(self) -> None:
        super().__init__()
        self._client = DebuggerProtocol()
        self._timer = QTimer(self)
        self._timer.timeout.connect(self.poll)
        self._interval_ms = 500

    @Slot(str, int)
    def connect_to(self, host: str, port: int) -> None:
        try:
            session_id = self._client.connect(host, port)
            self._timer.start(self._interval_ms)
            self.connected.emit(session_id)
            self.poll()
        except (OSError, ProtocolError) as exc:
            self._client.disconnect(send_close=False)
            self.disconnected.emit(str(exc))

    @Slot()
    def disconnect(self) -> None:
        self._timer.stop()
        self._client.disconnect()
        self.disconnected.emit("")

    @Slot(int)
    def set_interval(self, interval_ms: int) -> None:
        self._interval_ms = max(100, min(interval_ms, 9_000))
        if self._timer.isActive():
            self._timer.start(self._interval_ms)

    @Slot(list)
    def set_expressions(self, expressions: list[str]) -> None:
        if not self._client.connected:
            return
        try:
            self._client.set_expressions(expressions)
            self.poll()
        except (OSError, ProtocolError) as exc:
            self.error.emit(str(exc))

    @Slot(str)
    def evaluate(self, expression: str) -> None:
        if not self._client.connected:
            self.evaluated.emit(
                expression, {"ok": False, "error": "Нет подключения к контроллеру"}
            )
            return
        try:
            self.evaluated.emit(expression, self._client.evaluate(expression))
        except (OSError, ProtocolError) as exc:
            self.evaluated.emit(expression, {"ok": False, "error": str(exc)})

    @Slot(str)
    def execute(self, code: str) -> None:
        if not self._client.connected:
            self.executed.emit(
                code, {"ok": False, "error": "Нет подключения к контроллеру"}
            )
            return
        try:
            result = self._client.execute(code)
            if "Unknown command" in str(result.get("error", "")):
                result = {"ok": False, "error":
                          "Для выполнения Lua-кода обновите ядро (команда 14)"}
            self.executed.emit(code, result)
            if result.get("ok"):
                self.poll()
        except ValueError as exc:
            self.executed.emit(code, {"ok": False, "error": str(exc)})
        except (OSError, ProtocolError) as exc:
            self._timer.stop()
            self._client.disconnect(send_close=False)
            message = f"{exc}. Результат выполнения Lua неизвестен; команда не повторяется"
            self.executed.emit(code, {"ok": False, "error": message})
            self.disconnected.emit(message)

    @Slot(str, int, int)
    def browse_variables(
        self, expression: str, offset: int, request_id: int
    ) -> None:
        if not self._client.connected:
            self.browse_failed.emit(
                expression, offset, request_id,
                "Нет подключения к контроллеру",
            )
            return
        try:
            result = self._client.browse_variables(expression, offset)
            self.browse_loaded.emit(expression, offset, request_id, result)
        except OSError as exc:
            self.error.emit(str(exc))
            self.browse_failed.emit(expression, offset, request_id, str(exc))
        except (ProtocolError, ValueError) as exc:
            self.browse_failed.emit(
                expression, offset, request_id,
                self._kernel_hint(str(exc)),
            )

    @Slot(str, str, str, int)
    def set_variable(
        self, expression: str, value_type: str, value: str, request_id: int
    ) -> None:
        if not self._client.connected:
            self.assignment_failed.emit(
                expression, request_id, "Нет подключения к контроллеру"
            )
            return
        try:
            result = self._client.set_variable(expression, value_type, value)
            self.assignment_done.emit(expression, request_id, result)
        except OSError as exc:
            self.error.emit(str(exc))
            self.assignment_failed.emit(expression, request_id, str(exc))
        except (ProtocolError, ValueError) as exc:
            self.assignment_failed.emit(
                expression, request_id, self._kernel_hint(str(exc))
            )

    @staticmethod
    def _kernel_hint(message: str) -> str:
        if "Unknown command" in message:
            return ("Ядро контроллера не поддерживает просмотр и запись "
                    "переменных (команды 12/13) — обновите ядро")
        return message

    @Slot(int)
    def execute_controller_command(self, command_id: int) -> None:
        if not self._client.connected:
            self.error.emit("Нет подключения к контроллеру")
            return
        try:
            response = self._client.execute_controller_command(command_id)
            self.command_executed.emit(command_id, response)
        except (OSError, ProtocolError) as exc:
            self.error.emit(str(exc))

    @Slot()
    def refresh_reload_objects(self) -> None:
        if not self._client.connected:
            return
        try:
            self.reload_objects_loaded.emit(self._client.get_reload_objects())
        except (OSError, ProtocolError) as exc:
            self.reload_objects_failed.emit(str(exc))

    @Slot()
    def clear_chart_data(self) -> None:
        if not self._client.connected:
            return
        try:
            self._client.clear_chart_data()
            self.poll()
        except (OSError, ProtocolError) as exc:
            self.error.emit(str(exc))

    @Slot()
    def poll(self) -> None:
        if not self._client.connected:
            return
        try:
            data = self._client.poll()
            self.chart_data.emit(data)
            self.messages.emit(data["events"])
        except (OSError, ProtocolError) as exc:
            self._timer.stop()
            self._client.disconnect(send_close=False)
            self.disconnected.emit(str(exc))

    @Slot()
    def shutdown(self) -> None:
        self._timer.stop()
        self._client.disconnect()
