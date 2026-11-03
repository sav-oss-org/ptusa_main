from __future__ import annotations

import os
import re
import pytest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtWidgets import QApplication, QMessageBox

from ptusa_lua_debugger.window import DebuggerSessionWidget, MainWindow
from ptusa_lua_debugger.pulse_counter import PulseDefinition


def _watch_property(session, item, label):
    row = next(item.child(index) for index in range(item.childCount())
               if item.child(index).text(0) == label)
    return session.variables.itemWidget(row, 2)


def test_expression_description_display_and_session_roundtrip(tmp_path, monkeypatch):
    from copy import deepcopy
    from ptusa_lua_debugger.session_store import load_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    restored = DebuggerSessionWidget()
    try:
        expression = "OBJECT1.par_float[15]"
        session._create_expression(expression, history_enabled=True,
                                   setter={"code": "set_value(<newvalue>)", "limits": ""})
        root = session.variables.topLevelItem(0)
        assert root.text(0) == expression
        assert _watch_property(session, root, "Lua-выражение").text() == expression
        description = _watch_property(session, root, "Описание")
        session._on_chart_data({"server_time_ms": 2000, "series": [
            {"expression": expression, "samples": [
                {"time_ms": 1000, "value": 10, "type": "number", "ok": True}
            ]}
        ]})
        original = deepcopy(session._last_chart_data)
        description.setText("Температура продукта")
        assert root.text(0) == "Температура продукта"
        assert root.toolTip(0) == expression
        assert session._expressions() == [expression]
        assert session._history_expressions() == [expression]
        assert session._last_chart_data == original
        session._refresh_values()
        assert root.text(2) == "10"
        assert root.child(1).text(2) == "10"
        assert session.plot.listDataItems()[0].name() == expression
        session.expression_edit.setText(expression)
        session._add_expression()
        assert session.variables.topLevelItemCount() == 1
        session._connected = True
        session.execute_requested.disconnect(session._worker.execute)
        sent = []
        session.execute_requested.connect(sent.append)
        monkeypatch.setattr(QMessageBox, "question", lambda *args: QMessageBox.Yes)
        _watch_property(session, root, "Новое значение").setText("12")
        session._set_watch_value(root)
        assert len(sent) == 1 and expression in sent[0]
        assert "Температура продукта" not in sent[0]
        path = tmp_path / "description.ptlua.json"
        session._save_session_to(path)
        restored.load_document(load_session(path))
        restored_root = restored.variables.topLevelItem(0)
        assert restored_root.text(0) == "Температура продукта"
        assert _watch_property(restored, restored_root, "Описание").text() == description.text()
        assert _watch_property(restored, restored_root, "Lua-выражение").text() == expression
        assert restored._expressions() == [expression]
        description.setText("   ")
        assert root.text(0) == expression
        description.setText("Температура продукта")
        session._on_browser_watch(expression, False)
        assert session._expressions() == []
        assert session.plot.listDataItems() == []
    finally:
        session.shutdown()
        restored.shutdown()
        session.deleteLater()
        restored.deleteLater()
        application.processEvents()


def test_edit_expression_preserves_settings_and_resets_only_affected_history(tmp_path):
    from ptusa_lua_debugger.session_store import load_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    restored = DebuggerSessionWidget()
    try:
        session._create_expression("x", history_enabled=True,
                                   style={"description": "Датчик", "name": "График", "offset": 2},
                                   setter={"code": "set_value(<newvalue>)", "limits": "(0;1)"})
        session._create_expression("other", history_enabled=True)
        session._on_chart_data({"server_time_ms": 2000, "series": [
            {"expression": expression, "samples": [
                {"time_ms": 1000, "value": value, "type": "number", "ok": True}
            ]} for expression, value in [("x", 10), ("other", 20)]
        ]})
        root = session.variables.topLevelItem(0)
        root.setExpanded(True)
        style = session._series_styles()["x"]
        session.expressions_requested.disconnect(session._worker.set_expressions)
        sent = []
        session.expressions_requested.connect(sent.append)
        session._connected = True
        editor = _watch_property(session, root, "Lua-выражение")
        editor.setText("  y  ")
        assert session._expressions() == ["x", "other"]
        editor.editingFinished.emit()
        assert editor.text() == "y"
        assert root.text(0) == "Датчик" and root.toolTip(0) == "y"
        assert root.isExpanded()
        assert root.text(2) == "—"
        assert root.child(0).text(2) == "—"
        assert root.child(1).text(2) == "—"
        assert session._expressions() == ["y", "other"]
        assert session._history_expressions() == ["y", "other"]
        assert session._series_styles()["y"] == style
        assert session._setter_settings()["y"] == {"code": "set_value(<newvalue>)", "limits": "(0;1)"}
        assert _watch_property(session, root, "Имя на графике").placeholderText() == "y"
        assert sent == [["y", "other"]]
        assert set(session._statistics) == {"other"}
        assert [series["expression"] for series in session._last_chart_data["series"]] == ["other"]
        assert session.variables.topLevelItem(1).text(2) == "20"
        editor.editingFinished.emit()
        assert len(sent) == 1
        session._on_chart_data({"server_time_ms": 3000, "series": [
            {"expression": "y", "samples": [
                {"time_ms": 3000, "value": 1, "type": "number", "ok": True}
            ]}
        ]})
        assert root.text(2) == "1"
        path = tmp_path / "edited.ptlua.json"
        session._save_session_to(path)
        restored.load_document(load_session(path))
        assert restored._expressions() == ["y", "other"]
        assert restored.variables.topLevelItem(0).text(0) == "Датчик"
        assert restored._series_styles()["y"] == style
        assert restored._setter_settings() == session._setter_settings()
        _watch_property(session, root, "Описание").clear()
        assert root.text(0) == "y"
    finally:
        session.shutdown()
        restored.shutdown()
        session.deleteLater()
        restored.deleteLater()
        application.processEvents()


@pytest.mark.parametrize("commit", ["enter", "focus_loss"])
def test_expression_editor_commits_with_keyboard(commit):
    from PySide6.QtCore import Qt
    from PySide6.QtTest import QTest

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session._create_expression("x")
        root = session.variables.topLevelItem(0)
        root.setExpanded(True)
        session.resize(1180, 900)
        session.show()
        editor = _watch_property(session, root, "Lua-выражение")
        editor.setFocus()
        application.processEvents()
        assert editor.hasFocus()
        editor.setText("y")
        if commit == "enter":
            QTest.keyClick(editor, Qt.Key_Return)
        else:
            _watch_property(session, root, "Описание").setFocus()
            application.processEvents()
        assert session._expressions() == ["y"]
        assert root.text(0) == "y"
    finally:
        session.close()
        session.deleteLater()
        application.processEvents()


@pytest.mark.parametrize("expression", ["", "   ", "other", "я" * 513, "x\ny", "x\ry", "x\0y"])
def test_invalid_expression_edit_leaves_watch_unchanged(monkeypatch, expression):
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session._create_expression("x", style={"description": "Датчик"})
        session._create_expression("other")
        errors, sent = [], []
        monkeypatch.setattr(session, "_show_error", errors.append)
        session.expressions_requested.disconnect(session._worker.set_expressions)
        session.expressions_requested.connect(sent.append)
        session._connected = True
        root = session.variables.topLevelItem(0)
        editor = _watch_property(session, root, "Lua-выражение")
        editor.setText(expression)
        editor.editingFinished.emit()
        assert errors and not sent
        assert editor.text() == "x"
        assert root.text(0) == "Датчик"
        assert session._expressions() == ["x", "other"]
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_edit_described_pulse_source_updates_dependencies_and_removal(tmp_path):
    from ptusa_lua_debugger.session_store import load_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        definition = PulseDefinition("Партия", "left", "right", 1, 1)
        session._pulse_counters.add(definition)
        session._create_expression("left", history_enabled=True,
                                   style={"description": "Левый датчик"})
        session._create_expression("right", history_enabled=True)
        session._create_expression(definition.expression, history_enabled=True,
                                   style={"description": "Импульсы за партию"})
        counter = session.variables.topLevelItem(2)
        assert _watch_property(session, counter, "Lua-выражение").isReadOnly()
        session._pulse_counters.states[definition.expression].count = 12
        root = session.variables.topLevelItem(0)
        editor = _watch_property(session, root, "Lua-выражение")
        editor.setText("new_left")
        editor.editingFinished.emit()
        assert session._pulse_counters.definitions[definition.expression].source == "new_left"
        assert session._pulse_counters.states[definition.expression].count == 0
        assert session._expressions() == ["new_left", "right"]
        path = tmp_path / "pulse.ptlua.json"
        session._save_session_to(path)
        assert load_session(path)["pulse_definitions"][0]["source"] == "new_left"
        root.child(0).setSelected(True)
        session._remove_expressions()
        assert session._expressions() == ["right"]
        assert session._pulse_counters.definitions == {}
        assert session.variables.topLevelItemCount() == 1
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


@pytest.mark.parametrize("chart_type, expected_x, expected_y", [
    ("step_post", [1, 3, 3, 6, 6, 8, 8, 8], [0, 0, 1, 1, 0, 0, 0, 0]),
    ("step_pre", [1, 1, 1, 3, 3, 6, 6, 8], [0, 0, 1, 1, 0, 0, 0, 0]),
    ("step_mid", [1, 2, 2, 4.5, 4.5, 8], [0, 0, 1, 1, 0, 0]),
    ("line", [1, 3, 6, 8], [0, 1, 0, 0]),
    ("scatter", [1, 3, 6], [0, 1, 0]),
])
@pytest.mark.parametrize("single_sample", [False, True])
def test_chart_type_geometry_and_roundtrip(tmp_path, chart_type, expected_x,
                                          expected_y, single_sample) -> None:
    from copy import deepcopy
    from ptusa_lua_debugger.session_store import load_session, save_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    restored = DebuggerSessionWidget()
    try:
        session._create_expression("x", history_enabled=True)
        session._create_expression("other", history_enabled=True)
        samples = [
            {"time_ms": t, "value": v, "type": "number", "ok": True}
            for t, v in [(1000, 0), (3000, 1), (6000, 0)]
        ]
        data = {"server_time_ms": 8000, "series": [
            {"expression": "x", "samples": samples[:1] if single_sample else samples}
        ]}
        session._on_chart_data(data)
        original = deepcopy(session._last_chart_data)
        root = session.variables.topLevelItem(0)
        selector = _watch_property(session, root, "Тип графика")
        selector.setCurrentIndex(selector.findData(chart_type))
        curve = session.plot.listDataItems()[0]
        # Inspect the rendered geometry, not just the selected option.
        path = curve.curve.getPath()
        if chart_type != "scatter" and not single_sample:
            assert [path.elementAt(i).x for i in range(path.elementCount())] == expected_x
            assert [path.elementAt(i).y for i in range(path.elementCount())] == expected_y
        if chart_type == "scatter":
            assert list(curve.xData) == ([1] if single_sample else expected_x)
            assert curve.opts["pen"] is None
            assert curve.opts["symbol"] == "o"
        assert session._series_styles()["other"]["chart_type"] == "step_post"
        assert session._last_chart_data == original
        path = tmp_path / "styles.json"
        save_session(path, host="localhost", port=10000, poll_interval_ms=500,
                     history_limit=5000, expressions=session._expressions(),
                     history_expressions=session._history_expressions(),
                     chart_data=session._last_chart_data,
                     series_styles=session._series_styles())
        restored.load_document(load_session(path))
        assert restored._series_styles() == session._series_styles()
        assert restored.plot.listDataItems()[0].opts["stepMode"] == curve.opts["stepMode"]
    finally:
        session.shutdown()
        restored.shutdown()
        session.deleteLater()
        restored.deleteLater()
        application.processEvents()


def test_setter_confirmation_limits_and_session_roundtrip(monkeypatch, tmp_path):
    from ptusa_lua_debugger.session_store import load_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    restored = DebuggerSessionWidget()
    try:
        session.execute_requested.disconnect(session._worker.execute)
        sent, errors, confirmations = [], [], []
        session.execute_requested.connect(sent.append)
        monkeypatch.setattr(session, "_show_error", errors.append)
        session._connected = True
        getter = "LINE1V1:get_value()"
        settings = {"code": "LINE1V1:set_value(<newvalue>)", "limits": "(0;1;2)"}
        session._create_expression(getter, setter=settings)
        root = session.variables.topLevelItem(0)
        value = _watch_property(session, root, "Новое значение")

        def confirm(*args):
            confirmations.append(args)
            return QMessageBox.Yes

        monkeypatch.setattr(QMessageBox, "question", confirm)
        value.setText("3")
        session._set_watch_value(root)
        assert errors and not sent and not confirmations
        value.setText("2")
        session._set_watch_value(root)
        assert len(sent) == 1 and "LINE1V1:set_value(2)" in sent[0]
        assert confirmations[0][-1] == QMessageBox.No
        session._set_watch_value(root)
        assert len(sent) == 1  # Do not queue repeated clicks.
        session._on_executed(sent[0], {"ok": True, "type": "number", "value": 1})
        assert "number: 1" in session.evaluate_result.toPlainText()
        monkeypatch.setattr(QMessageBox, "question", lambda *args: QMessageBox.No)
        session._set_watch_value(root)
        assert len(sent) == 1

        path = tmp_path / "setter.ptlua.json"
        session._save_session_to(path)
        document = load_session(path)
        restored.load_document(document)
        assert restored._setter_settings() == {getter: settings}
        assert _watch_property(restored, restored.variables.topLevelItem(0), "Новое значение").text() == ""
        assert len(sent) == 1  # Loading never runs a setter.
        document.pop("setter_settings")
        restored.load_document(document)
        assert restored._setter_settings()[getter] == {"code": "", "limits": ""}
    finally:
        session.shutdown()
        restored.shutdown()
        session.deleteLater()
        restored.deleteLater()
        application.processEvents()


@pytest.mark.parametrize("failure", [OSError("lost reply"), TimeoutError("timeout")])
def test_execute_worker_does_not_retry_and_polls_after_success(monkeypatch, failure):
    from ptusa_lua_debugger.worker import DebuggerWorker
    from ptusa_lua_debugger.protocol import ProtocolError

    application = QApplication.instance() or QApplication([])
    worker = DebuggerWorker()
    class Client:
        connected = True
        error = None
        calls = 0
        closes = []

        def execute(self, code):
            self.calls += 1
            if self.error:
                raise self.error
            return {"ok": True, "value": 1}

        def disconnect(self, *, send_close=True):
            self.closes.append(send_close)
            self.connected = False

    client = Client()
    worker._client = client
    results, polls, disconnections = [], [], []
    worker.executed.connect(lambda code, result: results.append(result))
    worker.disconnected.connect(disconnections.append)
    monkeypatch.setattr(worker, "poll", lambda: polls.append(True))
    try:
        worker._timer.start(9000)
        worker.execute("x=1")
        assert results[-1]["ok"] and len(polls) == 1
        client.error = failure
        worker.execute("x=1")
        assert not results[-1]["ok"] and client.calls == 2 and len(polls) == 1
        assert client.closes == [False] and not client.connected
        assert not worker._timer.isActive() and len(disconnections) == 1
        assert "неизвестен" in results[-1]["error"]
        worker.execute("x=1")
        assert not results[-1]["ok"] and client.calls == 2
        client.connected = True
        client.error = ProtocolError("bad packet")
        worker.execute("x=1")
        assert client.closes == [False, False] and client.calls == 3
    finally:
        worker._timer.stop()
        worker.deleteLater()
        application.processEvents()


@pytest.mark.parametrize("failure", [
    ValueError("invalid local code"),
    {"ok": False, "type": "error", "error": "Lua runtime error"},
    {"ok": False, "error": "Unknown command"},
])
def test_execute_worker_keeps_connection_for_known_errors(monkeypatch, failure):
    from ptusa_lua_debugger.worker import DebuggerWorker

    application = QApplication.instance() or QApplication([])
    worker = DebuggerWorker()

    class Client:
        connected = True

        def execute(self, code):
            if isinstance(failure, ValueError):
                raise failure
            return failure

    worker._client = Client()
    results, polls, disconnections = [], [], []
    worker.executed.connect(lambda code, result: results.append(result))
    worker.disconnected.connect(disconnections.append)
    monkeypatch.setattr(worker, "poll", lambda: polls.append(True))
    try:
        worker._timer.start(9000)
        worker.execute("x=1")
        assert len(results) == 1 and not results[0]["ok"]
        assert worker._timer.isActive() and worker._client.connected
        assert not polls and not disconnections
        if isinstance(failure, dict) and failure["error"] == "Unknown command":
            assert "обновите ядро" in results[0]["error"]
    finally:
        worker._timer.stop()
        worker.deleteLater()
        application.processEvents()


@pytest.mark.parametrize("reconnect", [False, True])
def test_lua_confirmation_cancelled_when_connection_changes(monkeypatch, reconnect):
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session.execute_requested.disconnect(session._worker.execute)
        session.reload_objects_requested.disconnect(session._worker.refresh_reload_objects)
        session.expressions_requested.disconnect(session._worker.set_expressions)
        sent, errors = [], []
        session.execute_requested.connect(sent.append)
        monkeypatch.setattr(session, "_show_error", errors.append)
        session._connected = True

        def confirm(*args):
            session._on_disconnected("lost connection")
            if reconnect:
                session._on_connected("new-session")
            return QMessageBox.Yes

        monkeypatch.setattr(QMessageBox, "question", confirm)
        session._submit_lua("x=1", "x=1")
        assert not sent and errors and not session._lua_pending
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_lua_disconnect_clears_pending_request():
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session._lua_pending = True
        session._on_disconnected("lost connection")
        assert not session._lua_pending
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_lua_console_delivers_worker_result_on_gui_thread(monkeypatch):
    from PySide6.QtCore import QEventLoop, QThread, QTimer
    from PySide6.QtWidgets import QDialog, QPlainTextEdit, QPushButton

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    executed_threads, output_threads = [], []
    append = QPlainTextEdit.appendPlainText

    class Client:
        connected = True

        def execute(self, code):
            executed_threads.append(QThread.currentThread())
            return {"ok": True, "type": "number", "value": 1}

        def disconnect(self, **kwargs):
            self.connected = False

    def record_output(widget, text):
        output_threads.append(QThread.currentThread())
        append(widget, text)

    def run_dialog(dialog):
        editor, output = dialog.findChildren(QPlainTextEdit)
        editor.setPlainText("return 1")
        loop = QEventLoop()
        deadline = QTimer()
        deadline.setSingleShot(True)
        deadline.timeout.connect(loop.quit)
        output.textChanged.connect(loop.quit)
        dialog.findChild(QPushButton).click()
        deadline.start(2000)
        loop.exec()
        deadline.stop()
        assert "return 1" in output.toPlainText()
        return QDialog.Accepted

    session._worker._client = Client()
    session._connected = True
    monkeypatch.setattr(session._worker, "poll", lambda: None)
    monkeypatch.setattr(QMessageBox, "question", lambda *args: QMessageBox.Yes)
    monkeypatch.setattr(QPlainTextEdit, "appendPlainText", record_output)
    monkeypatch.setattr(QDialog, "exec", run_dialog)
    try:
        session._open_lua_console()
        assert executed_threads == [session._thread]
        assert output_threads and all(thread == application.thread() for thread in output_threads)
        assert not session._lua_pending
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


@pytest.mark.parametrize("source, expected", [
    ("return LIN", "return LINE1V1"),
    ("LINE1V1:ge", "LINE1V1:get_value"),
    ("LINE1V1.se", "LINE1V1.set_value"),
    ('_G["LINE1V1"]:ge', '_G["LINE1V1"]:get_value'),
    ('_G["LINE1V1"].se', '_G["LINE1V1"].set_value'),
    ("OBJECTS[1].le", "OBJECTS[1].level"),
    ('return _G[', 'return _G["LINE1V1"]'),
    ("local s='LIN';\nreturn LIN", "local s='LIN';\nreturn LINE1V1"),
    ('local s="\U0001d11e"; return LIN',
     'local s="\U0001d11e"; return LINE1V1'),
])
def test_lua_console_tab_uses_browser_snapshot(monkeypatch, source, expected):
    from PySide6.QtCore import Qt
    from PySide6.QtGui import QTextCursor
    from PySide6.QtTest import QTest
    from PySide6.QtWidgets import QDialog, QPlainTextEdit

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    browser = session._variable_browser
    browser._root_expression = "_G"
    browser._add_entry(None, {
        "name": "LINE1V1", "expression": '_G["LINE1V1"]',
        "type": "userdata",
    })
    browser._add_entry(None, {
        "name": "get_value",
        "expression": '_G["LINE1V1"]["get_value"]', "type": "function",
    })
    browser._add_entry(None, {
        "name": "set_value",
        "expression": '_G["LINE1V1"]["set_value"]', "type": "function",
    })
    browser._add_entry(None, {
        "name": "level", "expression": 'OBJECTS[1]["level"]',
        "type": "number",
    })
    requests, executions = [], []
    browser.browse_requested.connect(lambda *args: requests.append(args))
    session.execute_requested.connect(executions.append)

    def run_dialog(dialog):
        editor = dialog.findChildren(QPlainTextEdit)[0]
        editor.setPlainText(source)
        editor.moveCursor(QTextCursor.End)
        QTest.keyClick(editor, Qt.Key_Tab)
        assert editor.toPlainText() == expected
        editor.undo()
        assert editor.toPlainText() == source
        return QDialog.Accepted

    monkeypatch.setattr(QDialog, "exec", run_dialog)
    try:
        session._open_lua_console()
        assert not requests and not executions
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


@pytest.mark.parametrize("source", [
    "", "\n    ", "unknown", "line1", 'print("LIN', "-- LIN",
    "-- LIN\n", "LINE1V1\n", "--[=[\nLIN", "local s=[=[LIN",
])
def test_lua_completion_keeps_tab_without_code_matches(source):
    from PySide6.QtCore import Qt
    from PySide6.QtGui import QTextCursor
    from PySide6.QtTest import QTest
    from ptusa_lua_debugger.window import LuaConsoleEdit

    application = QApplication.instance() or QApplication([])
    editor = LuaConsoleEdit(lambda: ["LINE1V1"])
    try:
        editor.setPlainText(source)
        editor.moveCursor(QTextCursor.End)
        QTest.keyClick(editor, Qt.Key_Tab)
        assert editor.toPlainText() == source + "\t"
    finally:
        editor.deleteLater()
        application.processEvents()


@pytest.mark.parametrize("accept_key", ["tab", "enter"])
def test_lua_completion_popup_selection_midword_and_live_updates(accept_key):
    from PySide6.QtCore import Qt
    from PySide6.QtGui import QTextCursor
    from PySide6.QtTest import QTest
    from ptusa_lua_debugger.window import LuaConsoleEdit

    application = QApplication.instance() or QApplication([])
    expressions = ["LINE1V1:get_value", "LINE1V1:set_value"]
    editor = LuaConsoleEdit(lambda: list(expressions))
    try:
        editor.show()
        editor.setFocus()
        application.processEvents()
        editor.setPlainText("LINE1V1:")
        editor.moveCursor(QTextCursor.End)
        QTest.keyClick(editor, Qt.Key_Tab)
        popup = editor._completer.popup()
        assert popup.isVisible()
        assert editor.toPlainText() == "LINE1V1:"
        QTest.keyClick(popup, Qt.Key_Down)
        QTest.keyClick(editor, Qt.Key_Tab if accept_key == "tab"
                       else Qt.Key_Return)
        assert editor.toPlainText() == "LINE1V1:set_value"
        editor.setPlainText("return LINE1V1:get_value()")
        cursor = editor.textCursor()
        cursor.setPosition(len("return LINE1V1:get_va"))
        editor.setTextCursor(cursor)
        QTest.keyClick(editor, Qt.Key_Tab)
        assert editor.toPlainText() == "return LINE1V1:get_value()"
        editor.setPlainText("LINE1V1:new")
        editor.moveCursor(QTextCursor.End)
        expressions.append("LINE1V1:new_method")
        QTest.keyClick(editor, Qt.Key_Tab)
        assert editor.toPlainText() == "LINE1V1:new_method"
        editor.setPlainText("LINE1V1:")
        editor.moveCursor(QTextCursor.End)
        QTest.keyClick(editor, Qt.Key_Tab)
        QTest.keyClick(editor._completer.popup(), Qt.Key_Escape)
        assert editor.toPlainText() == "LINE1V1:"
        assert not editor._completer.popup().isVisible()
    finally:
        editor.close()
        editor.deleteLater()
        application.processEvents()


def test_tabs_own_independent_workers_and_threads() -> None:
    application = QApplication.instance() or QApplication([])
    window = MainWindow()
    try:
        first = window.sessions.widget(0)
        second = window._add_session()

        assert isinstance(first, DebuggerSessionWidget)
        assert first is not second
        assert first._worker is not second._worker
        assert first._thread is not second._thread
        assert window.sessions.count() == 2

        window._close_session(window.sessions.indexOf(second))
        application.processEvents()
        assert window.sessions.count() == 1
        assert first._thread.isRunning()
    finally:
        window.close()
        application.processEvents()


def test_session_displays_debugger_messages() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session._on_messages(
            {
                "controller_time_unix_ms": 1_789_123_456_789,
                "controller_time_millisec": 123_456,
                "dropped": 0,
                "messages": [
                    {
                        "id": 1,
                        "time_ms": 123_500,
                        "source": "set_err_msg",
                        "priority": 3,
                        "text": "Тестовая авария",
                    }
                ],
            }
        )

        assert session.messages_table.rowCount() == 1
        assert session.messages_table.item(0, 1).text() == "set_err_msg"
        assert session.messages_table.item(0, 2).text() == "ERROR"
        assert session.messages_table.item(0, 3).text() == "Тестовая авария"
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_tree_chart_styles_and_session_roundtrip(tmp_path) -> None:
    from copy import deepcopy
    from ptusa_lua_debugger.session_store import save_session, load_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    restored = DebuggerSessionWidget()
    try:
        style = {"name": "Ступень", "color": "#32aaff", "offset": 2.5, "points": True}
        session._create_expression("x", history_enabled=True, style=style)
        data = {"server_time_ms": 3000, "series": [{"expression": "x", "samples": [
            {"time_ms": 1000, "value": 0, "type": "number", "ok": True},
            {"time_ms": 2000, "value": 1, "type": "number", "ok": True},
        ]}]}
        original = deepcopy(data)
        session._on_chart_data(data)
        root = session.variables.topLevelItem(0)
        assert root.childCount() == 14
        assert "Медиана" not in [
            root.child(index).text(0) for index in range(root.childCount())
        ]
        assert not root.isExpanded()
        assert root.text(2) == "1"
        assert root.child(0).text(2) == "0"
        assert root.child(1).text(2) == "0"
        assert root.child(2).text(2) == "1"
        assert root.child(3).text(2) == "0.5"
        line, points = session.plot.listDataItems()
        assert line.opts["stepMode"] == "right"
        assert line.name() == "Ступень"
        assert line.opts["pen"].color().name() == "#32aaff"
        assert list(line.yData) == [2.5, 3.5, 3.5]
        assert list(points.yData) == [2.5, 3.5]
        assert list(points.xData) == [1, 2]
        assert data == original
        assert session._statistics["x"]["max"] == 1
        path = tmp_path / "session.json"
        save_session(path, host="localhost", port=10000, poll_interval_ms=500,
                     history_limit=5000, expressions=["x"], history_expressions=["x"],
                     chart_data=session._last_chart_data, statistics=session._statistics,
                     series_styles=session._series_styles())
        restored.load_document(load_session(path))
        assert restored._series_styles() == {"x": {**style, "description": "", "chart_type": "step_post"}}
        assert list(restored.plot.listDataItems()[0].yData) == [2.5, 3.5, 3.5]
        # Editing presentation settings redraws immediately without altering samples.
        _watch_property(session, root, "Сдвиг по Y").setValue(-1)
        assert list(session.plot.listDataItems()[0].yData) == [-1, 0, 0]
        _watch_property(session, root, "Точки на графике").setChecked(False)
        assert len(session.plot.listDataItems()) == 1
        # Removing a selected property removes its owning expression.
        root.child(1).setSelected(True)
        session._remove_expressions()
        assert session._expressions() == []
        assert session.plot.listDataItems() == []
    finally:
        session.shutdown()
        restored.shutdown()
        session.deleteLater()
        restored.deleteLater()
        application.processEvents()


def test_controller_commands_are_available_in_each_session(
    monkeypatch,
) -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session.controller_command_requested.disconnect(
            session._worker.execute_controller_command
        )
        sent: list[int] = []
        session.controller_command_requested.connect(sent.append)
        assert [session.command_combo.itemData(i)
                for i in range(session.command_combo.count())
                ] == [301, 302, 102, 100, 101, 0]
        assert not session.command_button.isEnabled()

        # Without a connection nothing is sent and no dialog is shown.
        def fail_question(*args, **kwargs):
            raise AssertionError("confirmation shown while disconnected")

        monkeypatch.setattr(QMessageBox, "question", fail_question)
        session._execute_controller_command()
        assert sent == []
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_controller_command_declined_confirmation_sends_nothing(
    monkeypatch,
) -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session.controller_command_requested.disconnect(
            session._worker.execute_controller_command
        )
        sent: list[int] = []
        session.controller_command_requested.connect(sent.append)
        session._connected = True
        session.command_button.setEnabled(True)
        session.host_edit.setText("192.168.0.10")
        session.port_spin.setValue(10_001)
        session.command_combo.setCurrentIndex(session.command_combo.findData(102))

        calls: list[tuple] = []

        def decline(*args):
            calls.append(args)
            return QMessageBox.StandardButton.No

        monkeypatch.setattr(QMessageBox, "question", decline)

        session.command_button.click()

        assert sent == []
        assert session.command_button.isEnabled()
        assert session.command_result.text() == "—"

        assert len(calls) == 1
        parent, title, text, buttons, default = calls[0]
        assert parent is session
        assert title == "Подтверждение команды"
        assert "192.168.0.10:10001" in text
        assert session.command_combo.currentText() in text
        assert buttons == (
            QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No
        )
        assert default == QMessageBox.StandardButton.No
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_controller_command_confirmed_is_sent(monkeypatch) -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session.controller_command_requested.disconnect(
            session._worker.execute_controller_command
        )
        sent: list[int] = []
        session.controller_command_requested.connect(sent.append)
        session._connected = True
        session.command_button.setEnabled(True)
        session.command_combo.setCurrentIndex(session.command_combo.findData(102))
        monkeypatch.setattr(
            QMessageBox,
            "question",
            lambda *args, **kwargs: QMessageBox.StandardButton.Yes,
        )

        session.command_button.click()
        assert sent == [102]
        assert not session.command_button.isEnabled()
        assert session.command_result.text() == "Выполнение…"
        session._on_command_executed(102, {"ok": True, "queued": True})
        assert session.command_button.isEnabled()
        assert session.command_result.text() == "Сохранение запланировано"
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_pulse_counter_is_plotted_and_restored(tmp_path) -> None:
    from ptusa_lua_debugger.session_store import load_session, save_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    restored = DebuggerSessionWidget()
    try:
        definition = PulseDefinition("Партия", "left", "right", 1, 1)
        session._pulse_counters.add(definition)
        session._create_expression("left", history_enabled=True, style={"offset": -0.5})
        session._create_expression("right", history_enabled=True, style={"offset": 1.5})
        session._create_expression(definition.expression, history_enabled=True)
        data = {
            "server_time_ms": 1040,
            "controller_time_unix_ms": 10_000,
            "controller_time_millisec": 1000,
            "series": [
                {"expression": "left", "samples": [
                    {"time_ms": time, "ok": True, "type": "number", "value": value}
                    for time, value in [(1000, 0), (1010, 1), (1020, 0), (1030, 1)]
                ]},
                {"expression": "right", "samples": [
                    {"time_ms": 1000, "ok": True, "type": "number", "value": 0},
                    {"time_ms": 1040, "ok": True, "type": "number", "value": 1},
                ]},
            ],
        }
        session._on_chart_data(data)
        assert session._expressions() == ["left", "right"]
        assert [s["value"] for s in session._last_chart_data["series"][-1]["samples"]] == [2]
        assert session.variables.topLevelItem(2).text(2) == "2"
        assert definition.expression in session.history_model.expressions
        assert any(curve.name() == definition.expression for curve in session.plot.listDataItems())
        path = tmp_path / "pulse.ptlua.json"
        save_session(path, host="localhost", port=10000, poll_interval_ms=500,
                     history_limit=5000, expressions=session._expressions(),
                     history_expressions=session._history_expressions(),
                     chart_data=session._last_chart_data, statistics=session._statistics,
                     series_styles=session._series_styles(),
                     pulse_definitions=[vars(definition)],
                     pulse_state=session._pulse_counters.snapshot())
        restored.load_document(load_session(path))
        assert restored._pulse_counters.states[definition.expression].count == 0
        assert restored._expressions() == ["left", "right"]
        assert any(curve.name() == definition.expression for curve in restored.plot.listDataItems())
        # Removing a source also removes the computed value that uses it.
        restored.variables.topLevelItem(0).setSelected(True)
        restored._remove_expressions()
        assert definition.expression not in restored._pulse_counters.definitions
        assert restored._expressions() == ["right"]
    finally:
        session.shutdown()
        restored.shutdown()
        session.deleteLater()
        restored.deleteLater()
        application.processEvents()


def test_timeline_selector_switches_chart_time_axis() -> None:
    import pyqtgraph as pg
    from datetime import datetime
    from PySide6.QtCore import Qt

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session._create_expression("x", history_enabled=True)
        session._on_chart_data(
            {
                "server_time_ms": 8000,
                "controller_time_unix_ms": 1_789_123_456_000,
                "controller_time_millisec": 1000,
                "client_time_unix_ms": 1_700_000_000_000,
                "client_time_millisec": 1000,
                "series": [
                    {
                        "expression": "x",
                        "samples": [
                            {"time_ms": 1000, "value": 0, "type": "number",
                             "ok": True},
                            {"time_ms": 3000, "value": 1, "type": "number",
                             "ok": True},
                            {"time_ms": 6000, "value": 0, "type": "number",
                             "ok": True},
                        ],
                    }
                ],
            }
        )

        model = session.history_model
        assert model.headerData(0, Qt.Horizontal) == "Реальное время"
        assert model.headerData(1, Qt.Horizontal) == "Время контроллера"
        assert model.headerData(2, Qt.Horizontal) == "x"
        assert model.data(model.index(0, 0)) == datetime.fromtimestamp(
            1_700_000_000_000 / 1000
        ).strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
        assert model.data(model.index(0, 1)) == datetime.fromtimestamp(
            1_789_123_456_000 / 1000
        ).strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]

        # Controller Unix-seconds axis is the default; the last point is the
        # held current-time value at server_time_ms.
        assert session.timeline_combo.currentData() == "controller"
        axis = session.plot.getAxis("bottom")
        assert isinstance(axis, pg.DateAxisItem)
        assert axis.labelText == "Время контроллера"
        curve = session.plot.listDataItems()[0]
        assert list(curve.xData) == [
            1_789_123_456.0,
            1_789_123_458.0,
            1_789_123_461.0,
            1_789_123_463.0,
        ]

        session.timeline_combo.setCurrentIndex(
            session.timeline_combo.findData("real")
        )
        axis = session.plot.getAxis("bottom")
        assert isinstance(axis, pg.DateAxisItem)
        assert axis.labelText == "Реальное время"
        curve = session.plot.listDataItems()[0]
        assert list(curve.xData) == [
            1_700_000_000.0,
            1_700_000_002.0,
            1_700_000_005.0,
            1_700_000_007.0,
        ]
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_periodic_logging_saves_and_starts_new_history(tmp_path) -> None:
    from copy import deepcopy
    from ptusa_lua_debugger.session_store import load_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session._create_expression("x", history_enabled=True)
        session._on_chart_data(
            {
                "server_time_ms": 8000,
                "controller_time_unix_ms": 1_789_123_456_000,
                "controller_time_millisec": 1000,
                "series": [
                    {
                        "expression": "x",
                        "samples": [
                            {"time_ms": 1000, "value": 0, "type": "number",
                             "ok": True},
                            {"time_ms": 3000, "value": 1, "type": "number",
                             "ok": True},
                            {"time_ms": 6000, "value": 0, "type": "number",
                             "ok": True},
                        ],
                    }
                ],
            }
        )
        saved_chart_data = deepcopy(session._last_chart_data)
        saved_statistics = deepcopy(session._statistics)
        session.logging_directory_edit.setText(str(tmp_path))
        session.logging_check.setChecked(True)
        assert session._logging_timer.isActive()
        assert session._logging_timer.interval() == 60 * 60_000

        session._rotate_log()

        files = list(tmp_path.glob("*.ptlua.json"))
        assert len(files) == 1
        document = load_session(files[0])
        assert document["chart_data"] == saved_chart_data
        assert document["statistics"] == saved_statistics
        assert session._last_chart_data is None
        assert session._statistics == {}
        assert session.history_model.rows == []
        assert session._logging_timer.isActive()
    finally:
        session.logging_check.setChecked(False)
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_failed_periodic_log_preserves_history_and_stops_logging(
    tmp_path, monkeypatch
) -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    errors: list[str] = []
    try:
        session._create_expression("x", history_enabled=True)
        session._on_chart_data(
            {
                "server_time_ms": 8000,
                "series": [
                    {
                        "expression": "x",
                        "samples": [
                            {"time_ms": 1000, "value": 0, "type": "number",
                             "ok": True},
                            {"time_ms": 3000, "value": 1, "type": "number",
                             "ok": True},
                        ],
                    }
                ],
            }
        )
        saved_chart_data = session._last_chart_data
        saved_statistics = session._statistics
        session.logging_directory_edit.setText(str(tmp_path))
        session.logging_check.setChecked(True)
        assert session._logging_timer.isActive()

        def fail_save(*args, **kwargs):
            raise OSError("disk full")

        monkeypatch.setattr(
            "ptusa_lua_debugger.window.save_session", fail_save
        )
        session._show_error = errors.append

        session._rotate_log()

        assert session._last_chart_data is saved_chart_data
        assert session._statistics is saved_statistics
        assert not session._logging_timer.isActive()
        assert not session.logging_check.isChecked()
        assert len(errors) == 1
        assert "disk full" in errors[0]
    finally:
        session.logging_check.setChecked(False)
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_auto_reconnect_backoff_progression() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session.connect_requested.disconnect(session._worker.connect_to)
        attempts: list[tuple[str, int]] = []
        session.connect_requested.connect(
            lambda host, port: attempts.append((host, port))
        )
        session.host_edit.setText("10.0.0.5")
        session.port_spin.setValue(12_345)
        session.auto_reconnect_check.setChecked(True)

        expected = [1_000 + 5_000 * step for step in range(12)]
        expected += [60_000, 60_000]
        intervals = []
        session._on_disconnected("Обрыв связи")
        intervals.append(session._reconnect_timer.interval())
        assert session._reconnect_timer.isActive()
        assert session.connect_button.isEnabled()
        assert "Обрыв связи · повторное подключение через 1 с" \
            == session.status_label.text()
        while len(intervals) < len(expected):
            session._retry_connection()
            assert not session._reconnect_timer.isActive()
            session._on_disconnected("Обрыв связи")
            intervals.append(session._reconnect_timer.interval())

        assert intervals == expected
        assert attempts == [("10.0.0.5", 12_345)] * (len(expected) - 1)
        assert session._reconnect_timer.isActive()
        assert "повторное подключение через 60 с" in session.status_label.text()
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_auto_reconnect_reset_and_manual_cancellation() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session.connect_requested.disconnect(session._worker.connect_to)
        session.disconnect_requested.disconnect(session._worker.disconnect)
        attempts: list[tuple[str, int]] = []
        disconnects: list[bool] = []
        session.connect_requested.connect(
            lambda host, port: attempts.append((host, port))
        )
        session.disconnect_requested.connect(lambda: disconnects.append(True))
        session.auto_reconnect_check.setChecked(True)

        # A scheduled retry is cancelled by a manual connect, which starts a
        # fresh backoff cycle.
        session._on_disconnected("Обрыв связи")
        assert session._reconnect_timer.isActive()
        session._toggle_connection()
        assert not session._reconnect_timer.isActive()
        assert session._connection_pending
        assert session._reconnect_delay_seconds == 1
        assert attempts == [("127.0.0.1", 10_000)]
        session._on_disconnected("Отказано")
        assert session._reconnect_timer.interval() == 1_000

        # A successful connection cancels the retry and resets the backoff.
        session._retry_connection()
        session._on_connected("session-1")
        assert not session._reconnect_timer.isActive()
        assert not session._connection_pending
        assert session._reconnect_delay_seconds == 1
        assert session._scheduled_reconnect_delay_seconds is None
        assert session._last_disconnect_reason == ""
        assert session.connect_button.text() == "Отключиться"

        # Manual disconnect emits an empty reason and never schedules a retry.
        session._toggle_connection()
        assert disconnects == [True]
        session._on_disconnected("")
        assert not session._reconnect_timer.isActive()
        assert session._last_disconnect_reason == ""
        assert session.status_label.text() == "Не подключено"
        assert session.connect_button.text() == "Подключиться"
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_auto_reconnect_toggle_preserves_pending_delay() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session.connect_requested.disconnect(session._worker.connect_to)

        # A failure with the option off does not schedule anything; enabling
        # the option schedules the first applicable delay.
        session._on_disconnected("Обрыв связи")
        assert not session._reconnect_timer.isActive()
        session.auto_reconnect_check.setChecked(True)
        assert session._reconnect_timer.isActive()
        assert session._reconnect_timer.interval() == 1_000
        assert "повторное подключение через 1 с" in session.status_label.text()

        # Toggling the option off/on reuses the already chosen delay and does
        # not advance the backoff.
        for _ in range(2):
            session.auto_reconnect_check.setChecked(False)
            assert not session._reconnect_timer.isActive()
            assert session.status_label.text() == "Обрыв связи"
            session.auto_reconnect_check.setChecked(True)
            assert session._reconnect_timer.isActive()
            assert session._reconnect_timer.interval() == 1_000

        # The next failed attempt advances to the following delay exactly once.
        session._retry_connection()
        session._on_disconnected("Обрыв связи")
        assert session._reconnect_timer.interval() == 6_000
        assert "повторное подключение через 6 с" in session.status_label.text()
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_shutdown_stops_reconnect_timer() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    session.auto_reconnect_check.setChecked(True)
    session._on_disconnected("Обрыв связи")
    assert session._reconnect_timer.isActive()
    session.shutdown()
    assert not session._reconnect_timer.isActive()
    session.deleteLater()
    application.processEvents()


def test_session_saves_and_restores_message_log(tmp_path) -> None:
    from ptusa_lua_debugger.session_store import load_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    restored = DebuggerSessionWidget()
    try:
        session._on_messages(
            {
                "controller_time_unix_ms": 1_789_123_456_789,
                "controller_time_millisec": 123_456,
                "dropped": 0,
                "messages": [
                    {
                        "id": 1,
                        "time_ms": 123_500,
                        "source": "set_err_msg",
                        "priority": 3,
                        "text": "Тестовая авария",
                    },
                    {
                        "id": 2,
                        "time_ms": 123_600,
                        "source": "debugger",
                        "priority": 7,
                        "text": "Отладочное сообщение",
                    },
                ],
            }
        )
        assert session.messages_table.rowCount() == 2
        expected = [
            [session.messages_table.item(row, column).text()
             for column in range(4)]
            for row in range(2)
        ]
        # The converted timestamp and the severity label are stored as shown.
        assert re.fullmatch(
            r"\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{3}", expected[0][0]
        )
        assert expected[0][1:] == ["set_err_msg", "ERROR", "Тестовая авария"]
        assert expected[1][2] == "DEBUG"

        path = tmp_path / "messages.ptlua.json"
        session._save_session_to(path)
        document = load_session(path)
        assert document["message_log"] == [
            dict(zip(("time", "source", "level", "text"), row))
            for row in expected
        ]

        restored.load_document(document)
        assert restored.messages_table.rowCount() == 2
        for row, values in enumerate(expected):
            for column, value in enumerate(values):
                assert restored.messages_table.item(row, column).text() == value

        # Loading a session without a saved log clears the displayed rows.
        restored.load_document({**document, "message_log": []})
        assert restored.messages_table.rowCount() == 0
    finally:
        session.shutdown()
        restored.shutdown()
        session.deleteLater()
        restored.deleteLater()
        application.processEvents()


@pytest.mark.parametrize("confirmed", [False, True])
def test_individual_object_reload_uses_selected_id(monkeypatch, confirmed) -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session.controller_command_requested.disconnect(session._worker.execute_controller_command)
        session.reload_objects_requested.disconnect(session._worker.refresh_reload_objects)
        sent = []
        refreshed = []
        session.controller_command_requested.connect(sent.append)
        session.reload_objects_requested.connect(lambda: refreshed.append(True))
        assert not session.reload_object_button.isEnabled()
        session._connected = True
        session._on_reload_objects([
            {"id": 1, "name": "Первый", "lua_name": "OBJECT1", "idle": False},
            {"id": 42, "name": "Второй", "lua_name": "OBJECT42", "idle": True},
        ])
        assert not session.reload_object_button.isEnabled()
        session.reload_object_combo.setCurrentIndex(1)
        assert session.reload_object_button.isEnabled()
        calls = []
        def confirm(*args):
            calls.append(args[2])
            return (QMessageBox.StandardButton.Yes if confirmed
                    else QMessageBox.StandardButton.No)
        monkeypatch.setattr(QMessageBox, "question", confirm)
        session.reload_object_button.click()
        assert "OBJECT42" in calls[0]
        assert sent == ([1030042] if confirmed else [])
        if confirmed:
            assert not session.reload_object_button.isEnabled()
            assert not session.command_button.isEnabled()
            session._on_command_executed(1030042, {"ok": True, "result": 0})
            assert "42" in session.command_result.text()
            assert "перезагружен" in session.command_result.text()
            assert refreshed == [True]
        assert session.reload_object_button.isEnabled()
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_object_reload_failure_and_disconnect_restore_controls(monkeypatch) -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session._connected = True
        session._on_reload_objects([{"id": 3, "name": "Tank", "idle": True}])
        session._command_pending = True
        session.command_result.setText("Выполнение…")
        monkeypatch.setattr(QMessageBox, "warning", lambda *args: None)
        session._show_error("Changed object.par_float; cold restart required")
        assert "Changed object.par_float" in session.command_result.text()
        assert session.reload_object_button.isEnabled()
        session._on_disconnected("")
        assert session.reload_object_combo.count() == 0
        assert not session.reload_object_button.isEnabled()
        # Late response from a disconnected session must not repopulate the list.
        session._on_reload_objects([{"id": 3, "idle": True}])
        assert session.reload_object_combo.count() == 0
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_evaluate_console_keeps_window_compact() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        baseline = session.minimumSizeHint().width()
        session.show()
        application.processEvents()
        size_before = session.size()
        huge = "x" * 200_000 + "\n" + "y" * 5_000
        session._on_evaluated("f()", {"ok": True, "type": "string", "value": huge})
        session._on_evaluated("g()", {"ok": False, "error": huge})
        application.processEvents()
        assert huge in session.evaluate_result.toPlainText()
        assert ">>> g()" in session.evaluate_result.toPlainText()
        assert "Ошибка:" in session.evaluate_result.toPlainText()
        assert session.minimumSizeHint().width() <= baseline + 50
        assert session.size() == size_before
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_browser_watch_adds_and_removes_owned_expressions() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    sent = []
    session.expressions_requested.connect(lambda exprs: sent.append(list(exprs)))
    try:
        session._connected = True
        session._on_browser_watch("_G.v", True)
        assert session._expressions() == ["_G.v"]
        assert sent[-1] == ["_G.v"]
        # Deduplication: watching an existing expression does not add it.
        session._on_browser_watch("_G.v", True)
        assert session._expressions() == ["_G.v"]
        # Browser-owned expression is removed on unwatch.
        session._on_browser_watch("_G.v", False)
        assert session._expressions() == []
        assert sent[-1] == []
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_browser_watch_respects_expression_limit() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session._connected = True
        for index in range(16):
            session._create_expression(f"e{index}")
        session._on_browser_watch("_G.extra", True)
        assert "_G.extra" not in session._expressions()
        assert len(session._expressions()) == 16
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_session_widgets_have_isolated_browsers() -> None:
    application = QApplication.instance() or QApplication([])
    first = DebuggerSessionWidget()
    second = DebuggerSessionWidget()
    try:
        first._connected = True
        first._on_browser_watch("_G.v", True)
        assert first._expressions() == ["_G.v"]
        assert second._expressions() == []
        assert first._variable_browser is not second._variable_browser
    finally:
        first.shutdown()
        second.shutdown()
        first.deleteLater()
        second.deleteLater()
        application.processEvents()


def test_browser_state_saved_and_restored(tmp_path) -> None:
    from ptusa_lua_debugger.session_store import load_session, save_session

    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    restored = DebuggerSessionWidget()
    try:
        session._connected = True
        session._variable_browser.root_edit.setText("OBJECT1")
        session._on_browser_watch("OBJECT1.level", True)
        # Simulate ownership that the browser checkbox path records.
        session._variable_browser._owned.add("OBJECT1.level")
        path = tmp_path / "browser.ptlua.json"
        session._save_session_to(path)
        document = load_session(path)
        assert document["variable_browser"] == {
            "root": "OBJECT1",
            "watched_expressions": ["OBJECT1.level"],
        }
        restored.load_document(document)
        assert restored._variable_browser.root_edit.text() == "OBJECT1"
        assert restored._variable_browser.watched_browser_expressions() == [
            "OBJECT1.level"
        ]
        assert "OBJECT1.level" in restored._expressions()
    finally:
        session.shutdown()
        restored.shutdown()
        session.deleteLater()
        restored.deleteLater()
        application.processEvents()


def test_browser_target_tracks_connection() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session.host_edit.setText("10.0.0.5")
        session.port_spin.setValue(20001)
        session._on_connected("abc")
        assert session._variable_browser._target == "10.0.0.5:20001"
        assert session._variable_browser._connected
        session._on_disconnected("")
        assert not session._variable_browser._connected
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()


def test_browser_unwatch_keeps_pulse_counter_dependency() -> None:
    application = QApplication.instance() or QApplication([])
    session = DebuggerSessionWidget()
    try:
        session._connected = True
        # The browser adds and owns this watch.
        session._on_browser_watch("_G.sensor", True)
        session._variable_browser._owned.add("_G.sensor")
        # A pulse counter depends on it as the counted sensor.
        definition = PulseDefinition(
            "pulses", "_G.sensor", "_G.pump", 1, 1
        )
        session._pulse_counters.add(definition)
        session._create_expression("_G.pump")
        session._create_expression(definition.expression)
        # Unchecking in the browser must keep the watch and the counter.
        session._on_browser_watch("_G.sensor", False)
        assert "_G.sensor" in session._expressions()
        assert definition.expression in session._pulse_counters.definitions
        assert definition.expression in [
            item.text(0) for item in session._expression_items()
        ]
        # The explicit remove button still cascades counters as before.
        targets = {item for item in session._expression_items()
                   if item.text(0) == "_G.sensor"}
        session._remove_expression_items(targets)
        assert "_G.sensor" not in session._expressions()
        assert definition.expression not in (
            session._pulse_counters.definitions
        )
    finally:
        session.shutdown()
        session.deleteLater()
        application.processEvents()
