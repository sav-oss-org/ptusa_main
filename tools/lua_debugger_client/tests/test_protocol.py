import json
import struct

from ptusa_lua_debugger.protocol import Command, DebuggerProtocol, ProtocolError

import pytest


def response(packet_id: int, document: dict) -> bytes:
    payload = json.dumps(document).encode() + b"\0"
    return struct.pack(">cBBH", b"s", 12, packet_id, len(payload)) + payload


class FakeSocket:
    def __init__(self, incoming: bytes) -> None:
        self.incoming = bytearray(incoming)
        self.sent = bytearray()
        self.closed = False

    def settimeout(self, timeout: float) -> None:
        pass

    def setsockopt(self, level: int, option: int, value: int) -> None:
        pass

    def recv(self, size: int) -> bytes:
        result = bytes(self.incoming[:size])
        del self.incoming[:size]
        return result

    def sendall(self, data: bytes) -> None:
        self.sent.extend(data)

    def shutdown(self, how: int) -> None:
        pass

    def close(self) -> None:
        self.closed = True


def test_execute_sends_multiline_chunk_once() -> None:
    fake = FakeSocket(response(1, {"ok": True, "type": "number", "value": 2}))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "abc"
    code = "x=2\nreturn x"
    assert client.execute(code)["value"] == 2
    assert fake.sent[6:] == bytes([14]) + b"abc\nx=2\nreturn x"
    for invalid in ("", " " * 10, "x" * 16385, "я" * 8193):
        before = bytes(fake.sent)
        with pytest.raises(ValueError):
            client.execute(invalid)
        assert bytes(fake.sent) == before


def test_connect_creates_session(monkeypatch) -> None:
    fake = FakeSocket(
        b"PAC accept"
        + response(
            1,
            {
                "ok": True,
                "session_id": "0123456789abcdef",
                "timeout_ms": 10_000,
                "controller_time_unix_ms": 1_789_123_456_789,
                "controller_time_millisec": 123_456,
            },
        )
    )
    monkeypatch.setattr("socket.create_connection", lambda *args, **kwargs: fake)
    readings = iter(
        [1_700_000_000_000_000_000, 1_700_000_000_010_000_000]
    )
    monkeypatch.setattr(
        "ptusa_lua_debugger.protocol.time.time_ns", lambda: next(readings)
    )

    client = DebuggerProtocol()
    assert client.connect("127.0.0.1") == "0123456789abcdef"
    assert client.connected
    assert client.controller_time_unix_ms == 1_789_123_456_789
    assert client.controller_time_millisec == 123_456
    assert client.client_time_unix_ms == (
        1_700_000_000_000 + 1_700_000_000_010
    ) // 2
    assert client.client_time_millisec == 123_456

    net_id, service, frame_type, packet_id, length = struct.unpack(
        ">cBBBH", fake.sent[:6]
    )
    assert (net_id, service, frame_type, packet_id) == (b"s", 2, 1, 1)
    assert fake.sent[6 : 6 + length] == bytes((Command.CREATE_SESSION,))


def test_request_contains_session_id() -> None:
    fake = FakeSocket(response(1, {"ok": True, "count": 2}))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"

    client.set_expressions(["TE1:get_value()", "M1:get_state()"])

    _, service, frame_type, packet_id, length = struct.unpack(">cBBBH", fake.sent[:6])
    assert (service, frame_type, packet_id) == (2, 1, 1)
    assert fake.sent[6 : 6 + length] == (
        bytes((Command.SET_CHART_EXPRESSIONS,))
        + b"session1\nTE1:get_value()\nM1:get_state()"
    )


def test_chart_data_contains_session_time_anchor() -> None:
    fake = FakeSocket(response(1, {"ok": True, "server_time_ms": 12, "series": []}))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"
    client.controller_time_unix_ms = 1_789_123_456_789
    client.controller_time_millisec = 123_456
    client.client_time_unix_ms = 1_700_000_000_000
    client.client_time_millisec = 123_450

    data = client.get_chart_data()

    assert data["controller_time_unix_ms"] == 1_789_123_456_789
    assert data["controller_time_millisec"] == 123_456
    assert data["client_time_unix_ms"] == 1_700_000_000_000
    assert data["client_time_millisec"] == 123_450


def test_messages_contain_session_time_anchor() -> None:
    fake = FakeSocket(
        response(
            1,
            {
                "ok": True,
                "dropped": 0,
                "messages": [
                    {
                        "id": 1,
                        "time_ms": 123_500,
                        "source": "log",
                        "priority": 6,
                        "text": "ready",
                    }
                ],
            },
        )
    )
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"
    client.controller_time_unix_ms = 1_789_123_456_789
    client.controller_time_millisec = 123_456
    client.client_time_unix_ms = 1_700_000_000_000
    client.client_time_millisec = 123_450

    data = client.get_messages()

    assert data["controller_time_unix_ms"] == 1_789_123_456_789
    assert data["controller_time_millisec"] == 123_456
    assert data["client_time_unix_ms"] == 1_700_000_000_000
    assert data["client_time_millisec"] == 123_450
    assert data["messages"][0]["text"] == "ready"
    assert fake.sent[6] == Command.GET_MESSAGES


def test_poll_uses_one_request_for_chart_and_messages() -> None:
    fake = FakeSocket(response(1, {
        "ok": True, "series": [],
        "events": {"ok": True, "messages": [], "dropped": 0},
    }))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"
    client.controller_time_unix_ms = 1000
    client.controller_time_millisec = 50
    client.client_time_unix_ms = 2000
    client.client_time_millisec = 40
    data = client.poll()
    assert fake.sent[6] == Command.POLL
    assert len(fake.sent) == 6 + 1 + len("session1\n")
    assert data["events"]["controller_time_unix_ms"] == 1000
    assert data["controller_time_millisec"] == 50
    assert data["client_time_unix_ms"] == 2000
    assert data["client_time_millisec"] == 40
    assert data["events"]["client_time_unix_ms"] == 2000
    assert data["events"]["client_time_millisec"] == 40


def test_controller_command_uses_debugger_session() -> None:
    fake = FakeSocket(response(1, {
        "ok": True, "command": 102, "result": 0, "queued": True,
    }))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"

    assert client.execute_controller_command(102)["queued"] is True
    _, service, frame_type, packet_id, length = struct.unpack(">cBBBH", fake.sent[:6])
    assert (service, frame_type, packet_id) == (2, 1, 1)
    assert fake.sent[6 : 6 + length] == (
        bytes((Command.EXEC_CONTROLLER_COMMAND,)) + b"session1\n102"
    )


def test_controller_command_reports_controller_failure() -> None:
    fake = FakeSocket(response(1, {
        "ok": False, "command": 100, "result": 1,
        "error": "Controller command failed",
    }))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"

    with pytest.raises(ProtocolError, match="Controller command failed"):
        client.execute_controller_command(100)


def test_reload_objects_use_debugger_session() -> None:
    objects = [{"id": 42, "lua_name": "OBJECT42", "name": "Tank", "idle": True}]
    fake = FakeSocket(response(1, {"ok": True, "objects": objects}))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"
    assert client.get_reload_objects() == objects
    assert fake.sent[6:] == bytes((Command.GET_RELOAD_OBJECTS,)) + b"session1\n"


def test_reload_reports_detailed_controller_failure() -> None:
    fake = FakeSocket(response(1, {
        "ok": False, "command": 1030042, "result": -3,
        "error": "Changed object.par_float; cold restart required",
    }))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"
    with pytest.raises(ProtocolError, match="Changed object.par_float"):
        client.execute_controller_command(1030042)


def test_browse_variables_framing() -> None:
    fake = FakeSocket(response(1, {
        "ok": True, "expression": "_G", "type": "table", "entries": [],
        "offset": 0, "next_offset": None, "truncated": False,
    }))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"

    result = client.browse_variables("_G", 128)

    assert result["ok"] is True
    assert result["entries"] == []
    _, service, frame_type, packet_id, length = struct.unpack(
        ">cBBBH", fake.sent[:6]
    )
    assert (service, frame_type, packet_id) == (2, 1, 1)
    assert fake.sent[6 : 6 + length] == (
        bytes((Command.BROWSE_VARIABLES,)) + b"session1\n_G\n128"
    )


def test_browse_variables_default_arguments() -> None:
    fake = FakeSocket(response(1, {"ok": True, "type": "table", "entries": []}))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"

    client.browse_variables()

    assert fake.sent[6:] == (
        bytes((Command.BROWSE_VARIABLES,)) + b"session1\n_G\n0"
    )


def test_browse_variables_reports_application_error() -> None:
    fake = FakeSocket(response(1, {
        "ok": False, "error": "attempt to index a nil value",
    }))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"
    with pytest.raises(ProtocolError, match="attempt to index a nil value"):
        client.browse_variables("NIL", 0)


def test_set_variable_framing_multiline_string() -> None:
    fake = FakeSocket(response(1, {
        "ok": True, "expression": "name", "type": "string",
        "value": "a\nb\n\"quoted\"",
    }))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"

    result = client.set_variable("name", "string", "a\nb\n\"quoted\"")

    assert result["value"] == "a\nb\n\"quoted\""
    assert fake.sent[6:] == (
        bytes((Command.SET_VARIABLE,))
        + b"session1\nname\nstring\na\nb\n\"quoted\""
    )


def test_set_variable_number_and_nil_framing() -> None:
    fake = FakeSocket(response(1, {"ok": True, "type": "nil", "value": "nil"}))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"

    client.set_variable("t.x", "nil", "")
    assert fake.sent[6:] == (
        bytes((Command.SET_VARIABLE,)) + b"session1\nt.x\nnil\n"
    )


def test_set_variable_reports_application_error() -> None:
    fake = FakeSocket(response(1, {
        "ok": False, "error": "target is read-only",
    }))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"
    with pytest.raises(ProtocolError, match="target is read-only"):
        client.set_variable("obj.ro", "number", "1")


def test_browse_and_set_require_session() -> None:
    client = DebuggerProtocol()
    with pytest.raises(ProtocolError):
        client.browse_variables()
    with pytest.raises(ProtocolError):
        client.set_variable("x", "number", "1")


def _session_client() -> tuple[DebuggerProtocol, FakeSocket]:
    fake = FakeSocket(response(1, {"ok": True}))
    client = DebuggerProtocol()
    client._socket = fake
    client.session_id = "session1"
    return client, fake


def test_browse_variables_rejects_invalid_arguments() -> None:
    client, fake = _session_client()
    invalid = [
        ("", 0),
        ("x" * 1025, 0),
        ("я" * 600, 0),          # 1200 UTF-8 bytes over the limit
        ("_G", -1),
        ("_G", True),
        ("_G", 1.5),
        ("_G", "0"),
    ]
    for expression, offset in invalid:
        with pytest.raises(ValueError):
            client.browse_variables(expression, offset)
    assert len(fake.sent) == 0


def test_browse_variables_accepts_multibyte_limit() -> None:
    client, fake = _session_client()
    client.browse_variables("я" * 500, 0)   # 1000 bytes, allowed
    assert len(fake.sent) > 0


def test_set_variable_rejects_invalid_arguments() -> None:
    client, fake = _session_client()
    cases = [
        ("a\nb", "number", "1"),
        ("a\rb", "number", "1"),
        ("", "number", "1"),
        ("x" * 1025, "number", "1"),
        ("x", "bogus", "1"),
        ("x", "nil", "leftover"),
        ("x", "boolean", "True"),
        ("x", "boolean", "1"),
        ("x", "number", ""),
        ("x", "number", "abc"),
        ("x", "number", "nan"),
        ("x", "number", "inf"),
        ("x", "number", "1 2"),
        ("x", "number", " 1"),
        ("x", "number", "1 "),
        ("x", "number", "+1"),
        ("x", "number", "1_0"),
        ("x", "number", "١٢"),
        ("x", "number", "0x10"),
        ("x", "number", 3),
    ]
    for expression, kind, value in cases:
        with pytest.raises(ValueError):
            client.set_variable(expression, kind, value)
    assert len(fake.sent) == 0


def test_set_variable_accepts_binary_safe_string() -> None:
    client, fake = _session_client()
    client.set_variable("x", "string", "a\x00b\n")
    assert fake.sent[6:] == (
        bytes((Command.SET_VARIABLE,))
        + b"session1\nx\nstring\na\x00b\n"
    )
