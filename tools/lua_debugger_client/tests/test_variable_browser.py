from __future__ import annotations

import os

import pytest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtWidgets import QApplication, QDialog, QMessageBox, QTreeWidgetItem
from PySide6.QtCore import Qt

from ptusa_lua_debugger.variable_browser import (
    VariableBrowserWidget,
    _EditValueDialog,
    _ENTRY,
    _MORE,
    _RETRY,
)
from ptusa_lua_debugger.protocol import ProtocolError
from ptusa_lua_debugger.worker import DebuggerWorker


def make_browser() -> VariableBrowserWidget:
    application = QApplication.instance() or QApplication([])
    return VariableBrowserWidget()


def result(expression: str, entries: list[dict], offset: int = 0,
           next_offset=None, truncated: bool = False,
           type_name: str = "table") -> dict:
    return {
        "ok": True,
        "expression": expression,
        "type": type_name,
        "entries": entries,
        "offset": offset,
        "next_offset": next_offset,
        "truncated": truncated,
    }


def entry(name: str, expression: str, type_name: str = "number",
          value: str = "1", expandable: bool = False,
          writable: bool = False) -> dict:
    return {
        "name": name,
        "expression": expression,
        "type": type_name,
        "value": value,
        "expandable": expandable,
        "writable": writable,
    }


def find_item(browser: VariableBrowserWidget, name: str,
              parent: QTreeWidgetItem | None = None) -> QTreeWidgetItem | None:
    if parent is None:
        for index in range(browser.tree.topLevelItemCount()):
            item = browser.tree.topLevelItem(index)
            found = find_item(browser, name, item)
            if found is not None:
                return found
        return None
    if parent.text(0) == name:
        return parent
    for index in range(parent.childCount()):
        found = find_item(browser, name, parent.child(index))
        if found is not None:
            return found
    return None


def request_id(browser: VariableBrowserWidget, expression: str,
               offset: int) -> int:
    matches = [
        rid
        for rid, (expr, off, _item, _gen) in browser._requests.items()
        if expr == expression and off == offset
    ]
    assert matches, f"no pending request for {expression} @ {offset}"
    return matches[-1]


def show_result(browser: VariableBrowserWidget, expression: str,
                offset: int, data: dict) -> None:
    browser.show_browse_result(
        expression, offset, request_id(browser, expression, offset), data
    )


def show_error(browser: VariableBrowserWidget, expression: str,
               offset: int, message: str) -> None:
    browser.show_browse_error(
        expression, offset, request_id(browser, expression, offset), message
    )


def make_pending(browser: VariableBrowserWidget, expression: str) -> int:
    browser._next_request_id += 1
    rid = browser._next_request_id
    browser._pending_assignment = (rid, expression, browser._generation)
    return rid


def test_completions_use_loaded_pages_and_methods_without_requests() -> None:
    browser = make_browser()
    requests = []
    browser.browse_requested.connect(
        lambda expression, offset, rid: requests.append((expression, offset))
    )
    try:
        assert browser.completion_expressions() == []
        browser.set_connected(True)
        browser.load_root()
        show_result(browser, "_G", 0, result("_G", [
            entry("LINE1V1", '_G["LINE1V1"]', "userdata",
                  expandable=True),
            entry("end", '_G["end"]'),
            entry("odd key", '_G["odd key"]'),
        ], next_offset=128))
        completions = browser.completion_expressions()
        assert "LINE1V1" in completions
        assert "_G.LINE1V1" in completions
        assert '_G["end"]' in completions and "end" not in completions
        assert '_G["odd key"]' in completions
        assert not any("get_value" in name for name in completions)
        valve = find_item(browser, "LINE1V1")
        valve.setExpanded(True)
        show_result(browser, '_G["LINE1V1"]', 0, result("LINE1V1", [
            entry("get_value", '_G["LINE1V1"]["get_value"]', "function"),
            entry("value", '_G["LINE1V1"]["value"]'),
            entry("broken", '_G["LINE1V1"]["broken"]', "error"),
        ]))
        before = list(requests)
        completions = browser.completion_expressions()
        assert "LINE1V1.get_value" in completions
        assert "LINE1V1:get_value" in completions
        assert '_G["LINE1V1"].get_value' in completions
        assert '_G["LINE1V1"]:get_value' in completions
        assert "LINE1V1.value" in completions
        assert "LINE1V1:value" not in completions
        assert not any("broken" in name for name in completions)
        assert completions == sorted(set(completions))
        assert requests == before
        browser.tree.setCurrentItem(valve)
        browser.refresh_selected()
        show_result(browser, '_G["LINE1V1"]', 0, result("LINE1V1", [
            entry("set_value", '_G["LINE1V1"]["set_value"]', "function"),
        ]))
        assert "LINE1V1:get_value" not in browser.completion_expressions()
        assert "LINE1V1:set_value" in browser.completion_expressions()
        browser.root_edit.setText("OBJECT1")
        browser.load_root()
        assert browser.completion_expressions() == []
    finally:
        browser.deleteLater()


def test_completions_preserve_bracket_keys_and_indices() -> None:
    browser = make_browser()
    try:
        browser._root_expression = "OBJECTS[1]"
        browser._add_entry(None, entry("level", 'OBJECTS[1]["level"]'))
        unusual = 'OBJECTS[1]["odd[\'key\']"]'
        browser._add_entry(None, entry("odd['key']", unusual))
        completions = browser.completion_expressions()
        assert "OBJECTS[1]" in completions
        assert "OBJECTS[1].level" in completions
        assert unusual in completions
        assert 'OBJECTS[1]["odd.key"]' not in completions
    finally:
        browser.deleteLater()


def test_lazy_child_request_only_once_until_refresh() -> None:
    browser = make_browser()
    requests = []
    browser.browse_requested.connect(
        lambda expression, offset, rid: requests.append((expression, offset))
    )
    try:
        browser.set_connected(True)
        browser.load_root()
        assert requests == [("_G", 0)]
        show_result(browser, "_G", 0, result("_G", [
            entry("t", "_G.t", "table", "<table>", expandable=True),
        ]))
        item = find_item(browser, "t")
        item.setExpanded(True)
        assert requests == [("_G", 0), ("_G.t", 0)]
        show_result(browser, "_G.t", 0, result("_G.t", [
            entry("x", "_G.t.x"),
        ]))
        item.setExpanded(False)
        item.setExpanded(True)
        assert requests == [("_G", 0), ("_G.t", 0)]

        browser.tree.setCurrentItem(item)
        browser.refresh_selected()
        assert requests == [("_G", 0), ("_G.t", 0), ("_G.t", 0)]
    finally:
        browser.deleteLater()


def test_pagination_appends_next_page() -> None:
    browser = make_browser()
    requests = []
    browser.browse_requested.connect(
        lambda expression, offset, rid: requests.append((expression, offset))
    )
    try:
        browser.set_connected(True)
        browser.load_root()
        page1 = [entry(f"k{i}", f"_G.k{i}") for i in range(128)]
        show_result(browser, "_G", 0, result("_G", page1, next_offset=128))
        more = None
        for index in range(browser.tree.topLevelItemCount()):
            item = browser.tree.topLevelItem(index)
            data = item.data(0, Qt.UserRole)
            if data and data["kind"] == _MORE:
                more = item
        assert more is not None
        browser._on_item_clicked(more, 0)
        assert requests[-1] == ("_G", 128)
        show_result(
            browser, "_G", 128,
            result("_G", [entry("last", "_G.last")], offset=128),
        )
        names = [
            browser.tree.topLevelItem(i).text(0)
            for i in range(browser.tree.topLevelItemCount())
        ]
        assert names[0] == "k0" and names[128] == "last"
        assert all(
            "ещё" not in name and "Загрузка" not in name for name in names
        )
    finally:
        browser.deleteLater()


def test_stale_reply_is_ignored() -> None:
    browser = make_browser()
    try:
        browser.set_connected(True)
        browser.load_root()
        old_id = request_id(browser, "_G", 0)
        browser.root_edit.setText("OBJECT1"); browser.load_root()
        browser.show_browse_result(
            "_G", 0, old_id, result("_G", [entry("old", "_G.old")])
        )
        assert browser.tree.topLevelItemCount() == 1
        assert "Загрузка" in browser.tree.topLevelItem(0).text(0)
        # Wrong offset for the live request is ignored without consuming
        # the pending context.
        pending_id = request_id(browser, "OBJECT1", 0)
        browser.show_browse_result(
            "OBJECT1", 64, pending_id,
            result("OBJECT1", [entry("stale", "OBJECT1.x")]),
        )
        assert find_item(browser, "stale") is None
        assert pending_id in browser._requests
        # A fabricated request id is ignored too.
        browser.show_browse_result(
            "OBJECT1", 0, pending_id + 1000,
            result("OBJECT1", [entry("bogus", "OBJECT1.y")]),
        )
        assert find_item(browser, "bogus") is None
        assert pending_id in browser._requests
        # The matching reply still lands.
        browser.show_browse_result(
            "OBJECT1", 0, pending_id,
            result("OBJECT1", [entry("fresh", "OBJECT1.fresh")]),
        )
        assert find_item(browser, "fresh") is not None
    finally:
        browser.deleteLater()


def test_same_root_reload_ignores_old_reply() -> None:
    browser = make_browser()
    try:
        browser.set_connected(True)
        browser.load_root()
        first_id = request_id(browser, "_G", 0)
        browser.load_root()
        second_id = request_id(browser, "_G", 0)
        assert second_id != first_id
        # The stale first reply is ignored.
        browser.show_browse_result(
            "_G", 0, first_id, result("_G", [entry("old", "_G.old")])
        )
        assert find_item(browser, "old") is None
        browser.show_browse_result(
            "_G", 0, second_id, result("_G", [entry("new", "_G.new")])
        )
        assert find_item(browser, "new") is not None
    finally:
        browser.deleteLater()


def test_root_switch_back_ignores_old_same_root_reply() -> None:
    browser = make_browser()
    try:
        browser.set_connected(True)
        browser.load_root()
        old_id = request_id(browser, "_G", 0)
        browser.root_edit.setText("OBJECT1"); browser.load_root()
        show_result(browser, "OBJECT1", 0,
                    result("OBJECT1", [entry("s", "OBJECT1.s")]))
        browser.root_edit.setText("_G"); browser.load_root()
        new_id = request_id(browser, "_G", 0)
        # A reply for the previous _G request id must not fill this root.
        browser.show_browse_result(
            "_G", 0, old_id, result("_G", [entry("old", "_G.old")])
        )
        assert find_item(browser, "old") is None
        browser.show_browse_result(
            "_G", 0, new_id, result("_G", [entry("fresh", "_G.fresh")])
        )
        assert find_item(browser, "fresh") is not None
    finally:
        browser.deleteLater()


def test_reconnect_ignores_old_same_root_reply() -> None:
    browser = make_browser()
    try:
        browser.set_connected(True)
        browser.load_root()
        old_id = request_id(browser, "_G", 0)
        browser.set_connected(False)
        browser.set_connected(True)
        browser.load_root()
        new_id = request_id(browser, "_G", 0)
        browser.show_browse_result(
            "_G", 0, old_id, result("_G", [entry("old", "_G.old")])
        )
        assert find_item(browser, "old") is None
        browser.show_browse_result(
            "_G", 0, new_id, result("_G", [entry("fresh", "_G.fresh")])
        )
        assert find_item(browser, "fresh") is not None
    finally:
        browser.deleteLater()


def test_reconnect_allows_refresh_of_interrupted_branch() -> None:
    browser = make_browser()
    try:
        browser.set_connected(True)
        browser.load_root()
        show_result(browser, "_G", 0, result("_G", [
            entry("t", "_G.t", "table", "<table>", expandable=True),
        ]))
        item = find_item(browser, "t")
        browser.tree.setCurrentItem(item)
        item.setExpanded(True)
        old_id = request_id(browser, "_G.t", 0)
        assert browser._data(item)["loading"]

        browser.set_connected(False)
        browser.set_connected(True)
        browser.refresh_selected()
        new_id = request_id(browser, "_G.t", 0)
        assert new_id != old_id
        browser.show_browse_result(
            "_G.t", 0, old_id, result("_G.t", [entry("old", "_G.t.old")])
        )
        assert find_item(browser, "old") is None
        show_result(browser, "_G.t", 0,
                    result("_G.t", [entry("fresh", "_G.t.fresh")]))
        assert find_item(browser, "fresh") is not None
        assert not browser._data(item)["loading"]
    finally:
        browser.deleteLater()


def test_browse_error_shows_retry_without_disconnect() -> None:
    browser = make_browser()
    try:
        browser.set_connected(True)
        browser.load_root()
        show_error(browser, "_G", 0, "attempt to index a nil value")
        assert browser.tree.topLevelItemCount() == 1
        item = browser.tree.topLevelItem(0)
        assert item.data(0, Qt.UserRole)["kind"] == _RETRY
        assert "повторить" in item.text(0)
        assert browser._connected
    finally:
        browser.deleteLater()


def test_selection_shows_details() -> None:
    browser = make_browser()
    try:
        browser.set_connected(True)
        browser.load_root()
        show_result(browser, "_G", 0, result("_G", [
            entry("v", "_G.v", "number", "3.14", writable=True),
        ]))
        item = find_item(browser, "v")
        browser.tree.setCurrentItem(item)
        text = browser.detail.toPlainText()
        assert "_G.v" in text and "3.14" in text and "number" in text
        assert "Запись разрешена" in text
    finally:
        browser.deleteLater()


def test_scalar_root_shows_value() -> None:
    browser = make_browser()
    try:
        browser.set_connected(True)
        browser.root_edit.setText("1 + 2"); browser.load_root()
        show_result(browser, "1 + 2", 0, {
            "ok": True, "expression": "1 + 2", "type": "number",
            "value": "3", "entries": [], "offset": 0,
            "next_offset": None, "truncated": False,
        })
        assert "number" in browser.status_label.text()
        assert "3" in browser.status_label.text()
    finally:
        browser.deleteLater()


def test_watch_checkbox_emits_and_owns_expressions() -> None:
    browser = make_browser()
    watched = []
    browser.watch_requested.connect(
        lambda expression, state: watched.append((expression, state))
    )
    try:
        browser.set_connected(True)
        browser.load_root()
        show_result(browser, "_G", 0, result("_G", [
            entry("v", "_G.v", "number", "1"),
            entry("t", "_G.t", "table", "<table>", expandable=True),
        ]))
        scalar = find_item(browser, "v")
        table = find_item(browser, "t")
        assert not (table.flags() & Qt.ItemIsUserCheckable)
        scalar.setCheckState(3, Qt.Checked)
        assert watched == [("_G.v", True)]
        assert browser.watched_browser_expressions() == ["_G.v"]
        scalar.setCheckState(3, Qt.Unchecked)
        assert watched == [("_G.v", True), ("_G.v", False)]
        assert browser.watched_browser_expressions() == []
    finally:
        browser.deleteLater()


def test_preexisting_watch_is_not_removed_by_uncheck() -> None:
    browser = make_browser()
    watched = []
    browser.watch_requested.connect(
        lambda expression, state: watched.append((expression, state))
    )
    try:
        browser.set_connected(True)
        browser.set_watched_expressions(["_G.v"])
        browser.load_root()
        show_result(browser, "_G", 0, result("_G", [
            entry("v", "_G.v", "number", "1"),
        ]))
        item = find_item(browser, "v")
        assert item.checkState(3) == Qt.Checked
        item.setCheckState(3, Qt.Unchecked)
        # Pre-existing watch is restored, no removal request emitted.
        assert watched == []
        assert item.checkState(3) == Qt.Checked
        assert "_G.v" not in browser.watched_browser_expressions()
    finally:
        browser.deleteLater()


def test_watch_state_sync_and_state_roundtrip() -> None:
    browser = make_browser()
    try:
        browser.set_connected(True)
        browser.load_root()
        show_result(browser, "_G", 0, result("_G", [
            entry("v", "_G.v", "number", "1"),
        ]))
        find_item(browser, "v").setCheckState(3, Qt.Checked)
        state = browser.snapshot_state()
        assert state["root"] == "_G"
        assert state["watched_expressions"] == ["_G.v"]

        restored = make_browser()
        try:
            owned = restored.restore_state(state)
            assert owned == ["_G.v"]
            assert restored.root_edit.text() == "_G"
            # Removing the watch elsewhere prunes ownership.
            restored.set_watched_expressions([])
            assert restored.watched_browser_expressions() == []
        finally:
            restored.deleteLater()
    finally:
        browser.deleteLater()


def _writable_scalar(browser: VariableBrowserWidget) -> QTreeWidgetItem:
    browser.set_connected(True)
    browser.load_root()
    show_result(browser, "_G", 0, result("_G", [
        entry("v", "_G.v", "number", "1", writable=True),
    ]))
    return find_item(browser, "v")


def test_edit_dialog_and_confirmation_default_no(monkeypatch) -> None:
    browser = make_browser()
    sent = []
    browser.assignment_requested.connect(
        lambda *args: sent.append(args)
    )
    questions = []
    monkeypatch.setattr(
        QMessageBox, "question",
        lambda *args: questions.append(args)
        or QMessageBox.StandardButton.No,
    )
    monkeypatch.setattr(
        _EditValueDialog, "exec", lambda dialog: QDialog.Accepted
    )
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        browser.edit_selected()
        assert sent == []
        assert len(questions) == 1
        # Buttons and the default answer are the positional args 3 and 4.
        assert questions[0][3] == (
            QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No
        )
        assert questions[0][4] == QMessageBox.StandardButton.No
    finally:
        browser.deleteLater()


def test_edit_cancel_sends_nothing(monkeypatch) -> None:
    browser = make_browser()
    sent = []
    browser.assignment_requested.connect(
        lambda *args: sent.append(args)
    )
    monkeypatch.setattr(
        _EditValueDialog, "exec", lambda dialog: QDialog.Rejected
    )
    monkeypatch.setattr(
        QMessageBox, "question", lambda *args: QMessageBox.StandardButton.Yes
    )
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        browser.edit_selected()
        assert sent == []
    finally:
        browser.deleteLater()


def test_edit_disconnected_sends_nothing(monkeypatch) -> None:
    browser = make_browser()
    sent = []
    browser.assignment_requested.connect(
        lambda *args: sent.append(args)
    )
    monkeypatch.setattr(
        _EditValueDialog, "exec", lambda dialog: QDialog.Accepted
    )
    monkeypatch.setattr(
        QMessageBox, "question", lambda *args: QMessageBox.StandardButton.Yes
    )
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        browser.set_connected(False)
        browser.edit_selected()
        assert sent == []
    finally:
        browser.deleteLater()


def test_edit_accept_sends_typed_payload_and_reread(monkeypatch) -> None:
    browser = make_browser()
    sent = []
    browser.assignment_requested.connect(
        lambda *args: sent.append(args)
    )
    requests = []
    browser.browse_requested.connect(
        lambda expression, offset, rid: requests.append((expression, offset))
    )
    monkeypatch.setattr(
        _EditValueDialog, "exec",
        lambda dialog: (
            dialog.number_edit.setText("42"),
            QDialog.Accepted,
        )[1],
    )
    monkeypatch.setattr(
        QMessageBox, "question", lambda *args: QMessageBox.StandardButton.Yes
    )
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        browser.edit_selected()
        assert sent == [("_G.v", "number", "42", sent[0][3])]
        request_id = sent[0][3]
        # Duplicate pending writes are blocked.
        browser.edit_selected()
        assert len(sent) == 1
        request_count = len(requests)
        # A stale assignment reply (old id) cannot complete this write.
        browser.show_assignment_result("_G.v", request_id + 1000, {
            "ok": True, "expression": "_G.v",
            "type": "number", "value": "999",
        })
        assert browser._pending_assignment is not None
        assert "999" not in find_item(browser, "v").text(2)
        browser.show_assignment_result("_G.v", request_id, {
            "ok": True, "expression": "_G.v",
            "type": "number", "value": "42",
        })
        assert "42" in find_item(browser, "v").text(2)
        assert browser._pending_assignment is None
        # Scalar writes do not re-scan the containing table.
        assert len(requests) == request_count
    finally:
        browser.deleteLater()


def test_nil_write_result_refreshes_parent() -> None:
    browser = make_browser()
    requests = []
    browser.browse_requested.connect(
        lambda expression, offset, rid: requests.append((expression, offset))
    )
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        rid = make_pending(browser, "_G.v")
        browser.show_assignment_result("_G.v", rid, {
            "ok": True, "expression": "_G.v",
            "type": "nil", "value": None,
        })
        # A deleted field triggers a root reload.
        assert requests[-1] == ("_G", 0)
        assert browser._pending_assignment is None
    finally:
        browser.deleteLater()


def test_nil_write_warns_about_removal(monkeypatch) -> None:
    browser = make_browser()
    questions = []
    monkeypatch.setattr(
        QMessageBox, "question",
        lambda *args: questions.append(args)
        or QMessageBox.StandardButton.No,
    )
    monkeypatch.setattr(
        _EditValueDialog, "exec",
        lambda dialog: (
            dialog.type_combo.setCurrentText("nil"),
            QDialog.Accepted,
        )[1],
    )
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        browser.edit_selected()
        assert len(questions) == 1
        assert "nil" in questions[0][2] and "удал" in questions[0][2]
    finally:
        browser.deleteLater()


def test_assignment_error_clears_pending() -> None:
    browser = make_browser()
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        rid = make_pending(browser, "_G.v")
        # A stale error for the same expression cannot clear the write.
        browser.show_assignment_error("_G.v", rid + 1000, "old error")
        assert browser._pending_assignment is not None
        browser.show_assignment_error("_G.v", rid, "transport closed")
        assert browser._pending_assignment is None
    finally:
        browser.deleteLater()


def test_disconnect_marks_tree_stale() -> None:
    browser = make_browser()
    try:
        _writable_scalar(browser)
        browser.set_connected(False)
        assert "устарел" in browser.status_label.text()
        assert not browser.load_button.isEnabled()
        assert browser._requests == {}
    finally:
        browser.deleteLater()


def test_refresh_preserves_watch_checks_without_replay() -> None:
    browser = make_browser()
    watched = []
    browser.watch_requested.connect(
        lambda expression, state: watched.append((expression, state))
    )
    try:
        _writable_scalar(browser)
        item = find_item(browser, "v")
        item.setCheckState(3, Qt.Checked)
        browser.set_watched_expressions(["_G.v"])
        assert watched == [("_G.v", True)]
        # A root refresh must not re-emit watch requests for
        # expressions that are already watched.
        browser.load_root()
        show_result(browser, "_G", 0, result("_G", [
            entry("v", "_G.v", "number", "2"),
        ]))
        assert watched == [("_G.v", True)]
        assert find_item(browser, "v").checkState(3) == Qt.Checked
    finally:
        browser.deleteLater()


def test_worker_browse_and_assign_signals() -> None:
    application = QApplication.instance() or QApplication([])

    class Stub:
        connected = True

        def browse_variables(self, expression, offset):
            return {"ok": True, "entries": [], "expression": expression}

        def set_variable(self, expression, kind, value):
            return {"ok": True, "expression": expression, "value": value}

    worker = DebuggerWorker()
    worker._client = Stub()
    loaded = []
    done = []
    worker.browse_loaded.connect(
        lambda expression, offset, rid, result: loaded.append(
            (expression, offset, rid, result)
        )
    )
    worker.assignment_done.connect(
        lambda expression, rid, result: done.append(
            (expression, rid, result)
        )
    )
    try:
        worker.browse_variables("_G", 0, 7)
        worker.set_variable("x", "number", "1", 8)
        assert loaded[0][:3] == ("_G", 0, 7) and loaded[0][3]["ok"]
        assert done == [
            ("x", 8, {"ok": True, "expression": "x", "value": "1"})
        ]
    finally:
        worker._client = None
        worker.deleteLater()


def test_worker_browse_protocol_error_is_contextual() -> None:
    application = QApplication.instance() or QApplication([])

    class Stub:
        connected = True

        def browse_variables(self, expression, offset):
            raise ProtocolError("Unknown command 12")

    worker = DebuggerWorker()
    worker._client = Stub()
    failures = []
    disconnects = []
    worker.browse_failed.connect(
        lambda expression, offset, rid, error: failures.append(
            (expression, offset, rid, error)
        )
    )
    worker.disconnected.connect(lambda reason: disconnects.append(reason))
    try:
        worker.browse_variables("_G", 0, 9)
        assert failures == [(
            "_G", 0, 9,
            "Ядро контроллера не поддерживает просмотр и запись "
            "переменных (команды 12/13) — обновите ядро",
        )]
        assert disconnects == []
    finally:
        worker._client = None
        worker.deleteLater()


def test_worker_disconnected_browse_fails_without_poll() -> None:
    application = QApplication.instance() or QApplication([])

    class Stub:
        connected = False

    worker = DebuggerWorker()
    worker._client = Stub()
    failures = []
    worker.browse_failed.connect(
        lambda expression, offset, rid, error: failures.append(error)
    )
    try:
        worker.browse_variables("_G", 0, 11)
        assert failures == ["Нет подключения к контроллеру"]
    finally:
        worker._client = None
        worker.deleteLater()


def test_modal_reconnect_blocks_pending_write(monkeypatch) -> None:
    browser = make_browser()
    sent = []
    browser.assignment_requested.connect(
        lambda *args: sent.append(args)
    )
    # Reconnect inside the modal: generation advances while it is open.
    monkeypatch.setattr(
        _EditValueDialog, "exec",
        lambda dialog: (
            browser.set_connected(False),
            browser.set_connected(True),
            QDialog.Accepted,
        )[2],
    )
    monkeypatch.setattr(
        QMessageBox, "question", lambda *args: QMessageBox.StandardButton.Yes
    )
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        browser.edit_selected()
        assert sent == []
    finally:
        browser.deleteLater()


def test_stale_entries_are_not_editable_after_reconnect() -> None:
    browser = make_browser()
    sent = []
    browser.assignment_requested.connect(
        lambda *args: sent.append(args)
    )
    requests = []
    browser.browse_requested.connect(
        lambda expression, offset, rid: requests.append((expression, offset))
    )
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        assert browser.edit_button.isEnabled()
        # Disconnect makes the loaded snapshot stale.
        browser.set_connected(False)
        assert browser._selected_writable() is None
        browser.edit_selected()
        assert sent == []
        # Reconnect alone does not revive stale entries.
        browser.set_connected(True)
        browser.tree.setCurrentItem(item)
        assert browser._selected_writable() is None
        assert not browser.edit_button.isEnabled()
        browser.edit_selected()
        assert sent == []
        # A fresh root load produces current-generation entries.
        browser.load_root()
        show_result(browser, "_G", 0, result("_G", [
            entry("v", "_G.v", "number", "1", writable=True),
        ]))
        fresh = find_item(browser, "v")
        browser.tree.setCurrentItem(fresh)
        assert browser._selected_writable() is not None
        assert browser.edit_button.isEnabled()
    finally:
        browser.deleteLater()


def test_expansion_while_disconnected_sends_nothing() -> None:
    browser = make_browser()
    requests = []
    browser.browse_requested.connect(
        lambda expression, offset, rid: requests.append((expression, offset))
    )
    try:
        browser.set_connected(True)
        browser.load_root()
        show_result(browser, "_G", 0, result("_G", [
            entry("t", "_G.t", "table", "<table>", expandable=True),
        ]))
        item = find_item(browser, "t")
        browser.set_connected(False)
        browser._on_item_expanded(item)
        assert requests == [("_G", 0)]
    finally:
        browser.deleteLater()


def test_refresh_scalar_targets_parent() -> None:
    browser = make_browser()
    requests = []
    browser.browse_requested.connect(
        lambda expression, offset, rid: requests.append((expression, offset))
    )
    try:
        browser.set_connected(True)
        browser.load_root()
        show_result(browser, "_G", 0, result("_G", [
            entry("t", "_G.t", "table", "<table>", expandable=True),
        ]))
        parent = find_item(browser, "t")
        parent.setExpanded(True)
        show_result(browser, "_G.t", 0, result("_G.t", [
            entry("x", "_G.t.x", "number", "1"),
        ]))
        child = find_item(browser, "x")
        browser.tree.setCurrentItem(child)
        browser.refresh_selected()
        # Refreshing a scalar re-reads its containing table, not root.
        assert requests[-1] == ("_G.t", 0)
    finally:
        browser.deleteLater()


def test_page_error_keeps_loaded_pages() -> None:
    browser = make_browser()
    try:
        browser.set_connected(True)
        browser.load_root()
        page1 = [entry(f"k{i}", f"_G.k{i}") for i in range(128)]
        show_result(browser, "_G", 0, result("_G", page1, next_offset=128))
        # Emulate clicking the 'more' node, then the page request fails.
        browser._request("_G", 128, None)
        show_error(browser, "_G", 128, "timeout")
        names = [
            browser.tree.topLevelItem(i).text(0)
            for i in range(browser.tree.topLevelItemCount())
        ]
        assert names[0] == "k0" and "k127" in names
        retry = browser.tree.topLevelItem(
            browser.tree.topLevelItemCount() - 1
        )
        assert retry.data(0, Qt.UserRole)["kind"] == _RETRY
        assert retry.data(0, Qt.UserRole)["offset"] == 128
    finally:
        browser.deleteLater()


def test_truncated_string_editor_requires_fresh_input() -> None:
    browser = make_browser()
    try:
        browser.set_connected(True)
        browser.load_root()
        show_result(browser, "_G", 0, result("_G", [{
            "name": "s",
            "expression": "_G.s",
            "type": "string",
            "value": "x" * 256,
            "expandable": False,
            "writable": True,
            "value_truncated": True,
        }]))
        item = find_item(browser, "s")
        data = item.data(0, Qt.UserRole)
        assert data["value_truncated"]
        dialog = _EditValueDialog(
            browser, "_G.s", "string", data["value"], True
        )
        # The partial preview is never offered as the new value.
        assert dialog.string_edit.toPlainText() == ""
        assert not dialog.truncated_label.isHidden()
        # An untouched blank editor cannot submit.
        with pytest.raises(ValueError):
            dialog.result_value()
        # Deliberate user input submits the replacement verbatim.
        dialog.string_edit.setPlainText("полная строка\nс переносом")
        assert dialog.result_value() == (
            "string", "полная строка\nс переносом"
        )
        dialog.deleteLater()
        # Editing then emptying is an explicit empty replacement.
        second = _EditValueDialog(browser, "_G.s", "string", "x" * 256, True)
        second.string_edit.setPlainText("tmp")
        second.string_edit.setPlainText("")
        assert second.result_value() == ("string", "")
        second.deleteLater()
        # Non-truncated strings are still pre-filled.
        third = _EditValueDialog(browser, "_G.s", "string", "ok", False)
        assert third.string_edit.toPlainText() == "ok"
        assert third.result_value() == ("string", "ok")
        third.deleteLater()
        browser.tree.setCurrentItem(item)
        assert "сокращ" in browser.detail.toPlainText()
    finally:
        browser.deleteLater()


def test_assignment_reread_updates_bool_and_details() -> None:
    browser = make_browser()
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        rid = make_pending(browser, "_G.v")
        browser.show_assignment_result("_G.v", rid, {
            "ok": True, "expression": "_G.v",
            "type": "boolean", "value": False,
        })
        data = item.data(0, Qt.UserRole)
        assert data["type"] == "boolean" and data["value"] == "false"
        assert item.text(2) == "false"
        # The next edit of this node defaults the boolean combo to false.
        dialog = _EditValueDialog(
            browser, "_G.v", data["type"], data["value"], False
        )
        assert dialog.boolean_combo.currentText() == "false"
        dialog.deleteLater()
        assert "false" in browser.detail.toPlainText()
    finally:
        browser.deleteLater()


def test_assignment_reread_carries_truncation_metadata() -> None:
    browser = make_browser()
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        rid = make_pending(browser, "_G.v")
        browser.show_assignment_result("_G.v", rid, {
            "ok": True, "expression": "_G.v",
            "type": "string", "value": "y" * 300,
            "value_truncated": True,
        })
        data = item.data(0, Qt.UserRole)
        assert data["value_truncated"]
    finally:
        browser.deleteLater()


def test_status_label_is_bounded() -> None:
    from PySide6.QtWidgets import QSizePolicy
    browser = make_browser()
    try:
        policy = browser.status_label.sizePolicy()
        assert policy.horizontalPolicy() == QSizePolicy.Ignored
        assert browser.status_label.minimumWidth() == 0
        browser.set_connected(True)
        huge = "err" + "x" * 100000
        browser.load_root()
        show_error(browser, "_G", 0, huge)
        assert browser.status_label.toolTip() == f"_G: {huge}"
        # The wrapped detail panel carries the complete error text.
        assert browser.detail.toPlainText() == f"_G: {huge}"
    finally:
        browser.deleteLater()


def test_worker_value_error_is_contextual() -> None:
    application = QApplication.instance() or QApplication([])

    class Stub:
        connected = True

        def browse_variables(self, expression, offset):
            raise ValueError("bad offset")

        def set_variable(self, expression, kind, value):
            raise ValueError("bad type")

    worker = DebuggerWorker()
    worker._client = Stub()
    browse_failures = []
    assign_failures = []
    worker.browse_failed.connect(
        lambda expression, offset, rid, error: browse_failures.append(
            (expression, offset, rid, error)
        )
    )
    worker.assignment_failed.connect(
        lambda expression, rid, error: assign_failures.append(
            (expression, rid, error)
        )
    )
    try:
        worker.browse_variables("_G", -1, 21)
        worker.set_variable("x", "bogus", "1", 22)
        assert browse_failures == [("_G", -1, 21, "bad offset")]
        assert assign_failures == [("x", 22, "bad type")]
    finally:
        worker._client = None
        worker.deleteLater()


def test_pending_write_blocks_navigation_and_reenables() -> None:
    browser = make_browser()
    requests = []
    browser.browse_requested.connect(
        lambda expression, offset, rid: requests.append((expression, offset))
    )
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        rid = make_pending(browser, "_G.v")
        browser._update_enabled()
        assert not browser.load_button.isEnabled()
        assert not browser.refresh_button.isEnabled()
        assert not browser.root_edit.isEnabled()
        assert not browser.edit_button.isEnabled()
        generation = browser._generation
        sent = len(requests)
        # A pending write locks every navigation path.
        browser.load_root()
        browser.refresh_selected()
        browser._request_children(item, "_G", 0)
        more = QTreeWidgetItem(browser.tree)
        more.setData(0, Qt.UserRole, {
            "kind": _MORE, "expression": "_G", "next_offset": 128,
        })
        browser._on_item_clicked(more, 0)
        assert browser._generation == generation
        assert len(requests) == sent
        assert browser._pending_assignment is not None
        # The matching write reply unblocks navigation again.
        browser.show_assignment_result("_G.v", rid, {
            "ok": True, "expression": "_G.v",
            "type": "number", "value": "2",
        })
        assert browser._pending_assignment is None
        assert browser.load_button.isEnabled()
        browser.load_root()
        assert requests[-1] == ("_G", 0)
    finally:
        browser.deleteLater()


def test_disconnect_during_pending_write_clears_it() -> None:
    browser = make_browser()
    try:
        _writable_scalar(browser)
        rid = make_pending(browser, "_G.v")
        browser._update_enabled()
        browser.set_connected(False)
        assert browser._pending_assignment is None
        # The late write reply for the dead request is ignored.
        browser.set_connected(True)
        browser.show_assignment_error("_G.v", rid, "late error")
        browser.show_assignment_result("_G.v", rid, {
            "ok": True, "expression": "_G.v",
            "type": "number", "value": "9",
        })
        assert browser._pending_assignment is None
        assert find_item(browser, "v").text(2) == "1"
    finally:
        browser.deleteLater()


def test_stale_assignment_replies_cannot_complete_new_write() -> None:
    browser = make_browser()
    try:
        _writable_scalar(browser)
        old_id = make_pending(browser, "_G.v")
        browser.set_connected(False)
        browser.set_connected(True)
        browser.load_root()
        show_result(browser, "_G", 0, result("_G", [
            entry("v", "_G.v", "number", "1", writable=True),
        ]))
        new_id = make_pending(browser, "_G.v")
        # Old result AND old error cannot clear the newer same-path write.
        browser.show_assignment_result("_G.v", old_id, {
            "ok": True, "expression": "_G.v",
            "type": "number", "value": "999",
        })
        browser.show_assignment_error("_G.v", old_id, "stale error")
        assert browser._pending_assignment is not None
        assert "999" not in find_item(browser, "v").text(2)
        # The correct reply is still accepted.
        browser.show_assignment_result("_G.v", new_id, {
            "ok": True, "expression": "_G.v",
            "type": "number", "value": "7",
        })
        assert browser._pending_assignment is None
        assert "7" in find_item(browser, "v").text(2)
    finally:
        browser.deleteLater()


def test_disconnect_during_modal_skips_confirmation(monkeypatch) -> None:
    browser = make_browser()
    sent = []
    questions = []
    browser.assignment_requested.connect(
        lambda *args: sent.append(args)
    )
    # Reconnect inside the edit modal: the write must be cancelled
    # before the confirmation question is even shown.
    monkeypatch.setattr(
        _EditValueDialog, "exec",
        lambda dialog: (
            browser.set_connected(False),
            browser.set_connected(True),
            QDialog.Accepted,
        )[2],
    )
    monkeypatch.setattr(
        QMessageBox, "question",
        lambda *args: questions.append(args)
        or QMessageBox.StandardButton.Yes,
    )
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        browser.edit_selected()
        assert questions == []
        assert sent == []
    finally:
        browser.deleteLater()


def test_selection_loss_and_dead_items_do_not_crash() -> None:
    browser = make_browser()
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        assert browser.edit_button.isEnabled()
        # Clearing the selection disables the edit action.
        browser.tree.setCurrentItem(None)
        assert not browser.edit_button.isEnabled()
        # An item deleted with the tree cannot be inspected.
        browser.tree.clear()
        browser._show_details(item, None)
        browser._show_details(None, None)
        assert not browser.edit_button.isEnabled()
    finally:
        browser.deleteLater()


def test_crlf_string_unchanged_returns_verbatim() -> None:
    browser = make_browser()
    try:
        dialog = _EditValueDialog(browser, "_G.s", "string", "a\r\nb")
        # An untouched editor returns the original value, not the
        # widget's LF-normalized copy.
        assert dialog.result_value() == ("string", "a\r\nb")
        dialog.string_edit.setPlainText("a\nb")
        assert dialog.result_value() == ("string", "a\nb")
        dialog.deleteLater()
    finally:
        browser.deleteLater()


def test_lossy_string_editor_requires_explicit_value() -> None:
    browser = make_browser()
    try:
        browser.set_connected(True)
        browser.load_root()
        show_result(browser, "_G", 0, result("_G", [{
            "name": "s",
            "expression": "_G.s",
            "type": "string",
            "value": "\\128raw",
            "expandable": False,
            "writable": True,
            "value_lossy": True,
        }]))
        item = find_item(browser, "s")
        data = item.data(0, Qt.UserRole)
        assert data["value_lossy"]
        browser.tree.setCurrentItem(item)
        assert "экранирован" in browser.detail.toPlainText()
        dialog = _EditValueDialog(
            browser, "_G.s", "string", data["value"], False, True
        )
        # Sanitized bytes are never offered as the new value.
        assert dialog.string_edit.toPlainText() == ""
        assert not dialog.truncated_label.isHidden()
        assert "экранирован" in dialog.truncated_label.text()
        with pytest.raises(ValueError):
            dialog.result_value()
        dialog.string_edit.setPlainText("чистый utf-8")
        assert dialog.result_value() == ("string", "чистый utf-8")
        dialog.deleteLater()
    finally:
        browser.deleteLater()


def test_assignment_reread_marks_lossy() -> None:
    browser = make_browser()
    try:
        item = _writable_scalar(browser)
        browser.tree.setCurrentItem(item)
        rid = make_pending(browser, "_G.v")
        browser.show_assignment_result("_G.v", rid, {
            "ok": True, "expression": "_G.v",
            "type": "string", "value": "\\255x",
            "value_lossy": True,
        })
        data = item.data(0, Qt.UserRole)
        assert data["value_lossy"]
        assert "экранирован" in browser.detail.toPlainText()
    finally:
        browser.deleteLater()


@pytest.mark.parametrize("type_name", ["table", "userdata"])
def test_cycles_stop_at_ancestors_but_sibling_aliases_expand(type_name) -> None:
    browser = make_browser()
    requests = []
    browser.browse_requested.connect(lambda *args: requests.append(args))

    def obj(name, expression, identity):
        return dict(entry(name, expression, type_name, "<object>", True),
                    object_id=identity)

    try:
        browser.set_connected(True)
        browser.load_root()
        show_result(browser, "_G", 0, dict(result("_G", [
            obj("_G", '_G["_G"]', "root"),
            obj("a", "_G.a", "child"),
            obj("b", "_G.b", "child"),
        ]), object_id="root"))
        cycle = find_item(browser, "_G")
        assert cycle.childCount() == 0
        assert "Циклическая ссылка: _G" in cycle.text(2)
        before = len(requests)
        browser._on_item_expanded(cycle)
        assert len(requests) == before
        for name in ("a", "b"):
            assert find_item(browser, name).data(0, Qt.UserRole)["expandable"]
        parent = find_item(browser, "a")
        parent.setExpanded(True)
        show_result(browser, "_G.a", 0, dict(result("_G.a", [
            obj("self", "_G.a.self", "child"),
            obj("back", "_G.a.back", "root"),
            obj("nested", "_G.a.nested", "nested"),
        ]), object_id="child"))
        assert find_item(browser, "self").childCount() == 0
        assert "_G.a" in find_item(browser, "self").text(2)
        assert find_item(browser, "back").childCount() == 0
        nested = find_item(browser, "nested")
        nested.setExpanded(True)
        show_result(browser, "_G.a.nested", 0, dict(result("_G.a.nested", [
            obj("back_to_a", "_G.a.nested.back", "child"),
        ]), object_id="nested"))
        assert find_item(browser, "back_to_a").childCount() == 0
        browser.tree.setCurrentItem(find_item(browser, "back_to_a"))
        assert "Циклическая ссылка: _G.a" in browser.detail.toPlainText()
        # Loading another root clears the old root identity.
        browser.root_edit.setText("other")
        browser.load_root()
        show_result(browser, "other", 0, dict(result("other", [
            obj("old_root", "other.old", "root"),
        ]), object_id="other"))
        assert find_item(browser, "old_root").data(0, Qt.UserRole)["expandable"]
    finally:
        browser.deleteLater()
