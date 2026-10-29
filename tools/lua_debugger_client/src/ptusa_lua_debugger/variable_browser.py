from __future__ import annotations

import math
import re
from typing import Any

from PySide6.QtCore import Qt, Signal, Slot
from PySide6.QtGui import QBrush, QColor
from PySide6.QtWidgets import (
    QAbstractItemView,
    QComboBox,
    QDialog,
    QDialogButtonBox,
    QFormLayout,
    QHBoxLayout,
    QHeaderView,
    QLabel,
    QLineEdit,
    QMessageBox,
    QPlainTextEdit,
    QPushButton,
    QSizePolicy,
    QTreeWidget,
    QTreeWidgetItem,
    QVBoxLayout,
    QWidget,
)

MAX_WATCH_EXPRESSIONS = 16
VALUE_PREVIEW_LIMIT = 512
CONFIRM_VALUE_LIMIT = 300

_ENTRY = "entry"
_MORE = "more"
_RETRY = "retry"
_PLACEHOLDER = "placeholder"

_NUMBER_PATTERN = re.compile(r"-?(?:\d+\.?\d*|\.\d+)(?:[eE][+-]?\d+)?")
_SCALAR_TYPES = ("number", "boolean", "string")
_LUA_IDENTIFIER = r"[A-Za-z_][A-Za-z0-9_]*"
_LUA_NAME_PATH = re.compile(
    rf"{_LUA_IDENTIFIER}(?:\.{_LUA_IDENTIFIER}|"
    rf"\[(?:\"{_LUA_IDENTIFIER}\"|'{_LUA_IDENTIFIER}'|"
    r"-?[0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?|true|false)\])*"
)
_LUA_KEYWORDS = frozenset(
    "and break do else elseif end false for function if in local nil "
    "not or repeat return then true until while".split()
)


class VariableBrowserWidget(QWidget):
    """UaExpert-style tree of Lua variables. All controller access goes
    through browse_requested/assignment_requested signals handled by the
    session worker; the widget owns no sockets or timers."""

    browse_requested = Signal(str, int, int)
    assignment_requested = Signal(str, str, str, int)
    watch_requested = Signal(str, bool)

    def __init__(self) -> None:
        super().__init__()
        self._connected = False
        self._target = ""
        self._root_object_id = ""
        self._root_expression = ""
        self._generation = 0
        self._next_request_id = 0
        # request_id -> (expression, offset, tree item or None for root,
        # generation)
        self._requests: dict[
            int, tuple[str, int, QTreeWidgetItem | None, int]
        ] = {}
        self._expr_items: dict[str, list[QTreeWidgetItem]] = {}
        self._watched: set[str] = set()
        self._owned: set[str] = set()
        # (request_id, expression, generation) while a write is in flight
        self._pending_assignment: tuple[int, str, int] | None = None
        self._suppress_check = False

        self.root_edit = QLineEdit("_G")
        self.root_edit.setPlaceholderText("Например: _G, OBJECT1 или OBJECTS[1]")
        self.root_edit.returnPressed.connect(self.load_root)
        self.load_button = QPushButton("Загрузить")
        self.load_button.clicked.connect(self.load_root)
        self.refresh_button = QPushButton("Обновить")
        self.refresh_button.setToolTip(
            "Повторно прочитать выбранную ветку или корень"
        )
        self.refresh_button.clicked.connect(self.refresh_selected)
        self.status_label = QLabel("Нет подключения")
        self.status_label.setTextFormat(Qt.PlainText)
        self.status_label.setSizePolicy(
            QSizePolicy.Ignored, QSizePolicy.Preferred
        )
        self.status_label.setMinimumWidth(0)
        self.status_label.setToolTip("Нет подключения")

        top = QHBoxLayout()
        top.addWidget(QLabel("Корень:"))
        top.addWidget(self.root_edit, 1)
        top.addWidget(self.load_button)
        top.addWidget(self.refresh_button)
        top.addWidget(self.status_label, 1)

        self.tree = QTreeWidget()
        self.tree.setColumnCount(4)
        self.tree.setHeaderLabels(["Имя", "Тип", "Значение", "Наблюдать"])
        self.tree.setSelectionMode(QAbstractItemView.SingleSelection)
        header = self.tree.header()
        header.setSectionResizeMode(0, QHeaderView.Interactive)
        header.setSectionResizeMode(1, QHeaderView.ResizeToContents)
        header.setSectionResizeMode(2, QHeaderView.Interactive)
        header.setSectionResizeMode(3, QHeaderView.ResizeToContents)
        self.tree.setColumnWidth(0, 260)
        self.tree.setColumnWidth(2, 320)
        self.tree.setAlternatingRowColors(True)
        self.tree.setTextElideMode(Qt.ElideRight)
        self.tree.itemExpanded.connect(self._on_item_expanded)
        self.tree.itemClicked.connect(self._on_item_clicked)
        self.tree.itemDoubleClicked.connect(self._on_item_double_clicked)
        self.tree.itemChanged.connect(self._on_item_changed)
        self.tree.currentItemChanged.connect(self._show_details)

        self.edit_button = QPushButton("Изменить значение…")
        self.edit_button.setToolTip(
            "Записать скалярное значение в выбранную переменную"
        )
        self.edit_button.clicked.connect(self.edit_selected)

        self.detail = QPlainTextEdit()
        self.detail.setReadOnly(True)
        self.detail.setPlaceholderText(
            "Предпросмотр значения и выражение выбранной переменной"
        )
        self.detail.setMaximumHeight(100)
        self.detail.setLineWrapMode(QPlainTextEdit.WidgetWidth)

        bottom = QHBoxLayout()
        bottom.addWidget(self.edit_button)
        bottom.addStretch(1)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(8, 8, 0, 0)
        layout.addLayout(top)
        layout.addWidget(self.tree, 1)
        layout.addWidget(self.detail)
        layout.addLayout(bottom)
        self._update_enabled()

    # ------------------------------------------------------------------
    # Connection state and request bookkeeping

    @Slot(bool)
    def set_connected(self, connected: bool) -> None:
        if connected == self._connected:
            self._update_enabled()
            return
        self._connected = connected
        if not connected:
            self._generation += 1
            # Cancel the branch's busy state along with its request so
            # it can be refreshed after reconnecting.
            for _expr, _offset, item, _generation in self._requests.values():
                if self._item_alive(item):
                    data = self._data(item)
                    if data is not None:
                        data["loading"] = False
                        item.setData(0, Qt.UserRole, data)
            self._requests.clear()
            self._pending_assignment = None
            self._set_status(
                "Нет подключения — снимок дерева устарел"
            )
        else:
            self._set_status("Подключено — загрузите переменные")
        self._update_enabled()

    def set_target(self, target: str) -> None:
        self._target = target

    def _set_status(self, text: str) -> None:
        self.status_label.setText(text)
        self.status_label.setToolTip(text)

    def _update_enabled(self) -> None:
        # Navigation is locked while a write is in flight: a root or
        # branch reload would advance the generation and orphan the
        # pending write context forever.
        enabled = self._connected and self._pending_assignment is None
        self.load_button.setEnabled(enabled)
        self.refresh_button.setEnabled(enabled)
        self.root_edit.setEnabled(enabled)
        self.edit_button.setEnabled(
            enabled and self._selected_writable() is not None
        )

    def _item_alive(self, item: QTreeWidgetItem | None) -> bool:
        if item is None:
            return False
        try:
            return item.treeWidget() is self.tree
        except RuntimeError:
            return False

    # ------------------------------------------------------------------
    # Loading

    @Slot()
    def load_root(self) -> None:
        if not self._connected or self._pending_assignment is not None:
            return
        expression = self.root_edit.text().strip() or "_G"
        self._generation += 1
        self._requests.clear()
        self.tree.clear()
        self._expr_items.clear()
        self._root_object_id = ""
        self._root_expression = expression
        placeholder = QTreeWidgetItem(self.tree, ["Загрузка…", "", "", ""])
        placeholder.setData(0, Qt.UserRole, {"kind": _PLACEHOLDER})
        # Set the status before emitting the request so a synchronous
        # reply cannot be overwritten by the loading text.
        self._set_status(f"Загрузка {expression}…")
        self._request(expression, 0, None)

    @Slot()
    def refresh_selected(self) -> None:
        if not self._connected or self._pending_assignment is not None:
            return
        item = self.tree.currentItem()
        data = self._data(item)
        if data and data["kind"] == _ENTRY:
            if data["expandable"]:
                self._request_children(item, data["expression"], 0)
                return
            parent = item.parent()
            if parent is not None:
                parent_data = self._data(parent)
                if parent_data and parent_data["kind"] == _ENTRY \
                        and parent_data["expandable"]:
                    self._request_children(
                        parent, parent_data["expression"], 0
                    )
                    return
        self.load_root()

    def _request(self, expression: str, offset: int,
                 item: QTreeWidgetItem | None) -> None:
        self._next_request_id += 1
        request_id = self._next_request_id
        # A new request for the same root/branch slot supersedes earlier
        # ones: a late reply to them must not populate the refreshed view.
        for rid, (_expr, _off, req_item, _gen) in list(
            self._requests.items()
        ):
            if req_item is item:
                del self._requests[rid]
        self._requests[request_id] = (
            expression, offset, item, self._generation
        )
        self.browse_requested.emit(expression, offset, request_id)

    def _request_children(self, item: QTreeWidgetItem, expression: str,
                          offset: int) -> None:
        if self._pending_assignment is not None:
            return
        data = self._data(item)
        if data is None or data.get("loading"):
            return
        if offset == 0 and not data.get("loaded"):
            placeholder = QTreeWidgetItem(item, ["Загрузка…", "", "", ""])
            placeholder.setData(0, Qt.UserRole, {"kind": _PLACEHOLDER})
        data["loading"] = True
        item.setData(0, Qt.UserRole, data)
        self._request(expression, offset, item)

    def _on_item_expanded(self, item: QTreeWidgetItem) -> None:
        if not self._connected:
            return
        data = self._data(item)
        if not data or data["kind"] != _ENTRY or not data["expandable"]:
            return
        if data["loaded"] or data.get("loading"):
            return
        self._request_children(item, data["expression"], 0)

    def _on_item_clicked(self, item: QTreeWidgetItem, _column: int) -> None:
        if not self._connected or self._pending_assignment is not None:
            return
        data = self._data(item)
        if not data:
            return
        if data["kind"] == _MORE:
            parent = item.parent()
            if parent is not None and self._item_alive(parent):
                self._unregister(
                    parent.takeChild(parent.indexOfChild(item))
                )
                self._request_children(
                    parent, data["expression"], data["next_offset"]
                )
            elif parent is None:
                self._unregister(
                    self.tree.takeTopLevelItem(
                        self.tree.indexOfTopLevelItem(item)
                    )
                )
                self._request(
                    data["expression"], data["next_offset"], None
                )
        elif data["kind"] == _RETRY:
            parent = item.parent()
            if parent is not None:
                self._unregister(
                    parent.takeChild(parent.indexOfChild(item))
                )
            else:
                self._unregister(
                    self.tree.takeTopLevelItem(
                        self.tree.indexOfTopLevelItem(item)
                    )
                )
                placeholder = QTreeWidgetItem(
                    self.tree, ["Загрузка…", "", "", ""]
                )
                placeholder.setData(0, Qt.UserRole, {"kind": _PLACEHOLDER})
            self._request(data["expression"], data["offset"], parent)

    # ------------------------------------------------------------------
    # Browse results

    @Slot(str, int, int, dict)
    def show_browse_result(self, expression: str, offset: int,
                           request_id: int,
                           result: dict[str, Any]) -> None:
        request = self._requests.get(request_id)
        if request is None:
            return
        expected_expression, expected_offset, item, generation = request
        # A mismatched or stale reply is ignored without consuming the
        # outstanding request context.
        if (
            expected_expression != expression
            or expected_offset != offset
            or generation != self._generation
            or not self._connected
        ):
            return
        if item is not None and not self._item_alive(item):
            return
        del self._requests[request_id]
        entries = result.get("entries", [])
        truncated = bool(result.get("truncated"))
        next_offset = result.get("next_offset")
        if item is None:
            if offset == 0:
                self.tree.clear()
                self._expr_items.clear()
                self._root_object_id = str(result.get("object_id", ""))
            else:
                for index in range(
                    self.tree.topLevelItemCount() - 1, -1, -1
                ):
                    top = self.tree.topLevelItem(index)
                    child = self._data(top)
                    if child and child["kind"] in (
                        _MORE, _PLACEHOLDER, _RETRY
                    ):
                        self._unregister(
                            self.tree.takeTopLevelItem(index)
                        )
            parent: QTreeWidgetItem | None = None
            self._set_status(
                f"{expression}: {self._describe_root(result)}"
            )
        else:
            parent = item
            data = self._data(item) or {}
            data["object_id"] = str(result.get("object_id", ""))
            data["loaded"] = True
            data["loading"] = False
            item.setData(0, Qt.UserRole, data)
            if offset == 0:
                self._take_children(item)
            else:
                # Remove the node the new page replaces.
                for index in range(item.childCount() - 1, -1, -1):
                    if self._data(item.child(index)) and \
                            self._data(item.child(index))["kind"] in (
                                _MORE, _PLACEHOLDER, _RETRY):
                        self._unregister(item.takeChild(index))
        for entry in entries:
            self._add_entry(parent, entry)
        if next_offset is not None:
            more = QTreeWidgetItem(parent if parent is not None else self.tree,
                                   [f"… ещё (смещение {next_offset})", "", "", ""])
            more.setData(0, Qt.UserRole, {
                "kind": _MORE,
                "expression": expression,
                "next_offset": int(next_offset),
            })
            more.setForeground(0, QBrush(QColor("#7f8c8d")))
        if truncated:
            note = QTreeWidgetItem(parent if parent is not None else self.tree,
                                   ["… список усечён сервером", "", "", ""])
            note.setData(0, Qt.UserRole, {"kind": _PLACEHOLDER})
            note.setForeground(0, QBrush(QColor("#e67e22")))
        if parent is not None and not parent.isExpanded():
            parent.setExpanded(True)

    @Slot(str, int, int, str)
    def show_browse_error(self, expression: str, offset: int,
                          request_id: int, message: str) -> None:
        request = self._requests.get(request_id)
        if request is None:
            return
        expected_expression, expected_offset, item, generation = request
        if (
            expected_expression != expression
            or expected_offset != offset
            or generation != self._generation
        ):
            return
        del self._requests[request_id]
        self._set_status(f"{expression}: {message}")
        if item is not None and not self._item_alive(item):
            self.detail.setPlainText(f"{expression}: {message}")
            return
        if item is not None:
            data = self._data(item) or {}
            data["loading"] = False
            item.setData(0, Qt.UserRole, data)
            for index in range(item.childCount() - 1, -1, -1):
                child = self._data(item.child(index))
                if child and child["kind"] == _PLACEHOLDER:
                    self._unregister(item.takeChild(index))
        elif offset == 0:
            # Only a failed first page replaces the whole tree; later
            # page errors keep already loaded pages visible.
            self.tree.clear()
            self._expr_items.clear()
        retry = QTreeWidgetItem(item if item is not None else self.tree,
                                [f"Ошибка: {message} — повторить", "", "", ""])
        retry.setData(0, Qt.UserRole, {
            "kind": _RETRY,
            "expression": expression,
            "offset": offset,
        })
        retry.setForeground(0, QBrush(QColor("#e74c3c")))
        # The elided status shows a prefix only; the wrapped detail
        # panel carries the complete contextual error text.
        self.detail.setPlainText(f"{expression}: {message}")

    @staticmethod
    def _describe_root(result: dict[str, Any]) -> str:
        root_type = str(result.get("type", ""))
        count = len(result.get("entries", []))
        if root_type in ("table", "userdata"):
            text = f"{root_type}, {count} элементов"
        else:
            text = f"{root_type} = {VariableBrowserWidget._display_value(result)}"
        if result.get("truncated"):
            text += " (усечено)"
        if result.get("value_truncated"):
            text += " (значение усечено)"
        if result.get("value_lossy"):
            text += " (не-UTF8 байты)"
        return text

    def _take_children(self, item: QTreeWidgetItem) -> None:
        while item.childCount():
            self._unregister(item.takeChild(0))

    def _unregister(self, item: QTreeWidgetItem) -> None:
        data = self._data(item)
        if data and data["kind"] == _ENTRY:
            expression = data.get("expression", "")
            items = self._expr_items.get(expression)
            if items and item in items:
                items.remove(item)
        for index in range(item.childCount()):
            self._unregister(item.child(index))

    # ------------------------------------------------------------------
    # Entries

    def _add_entry(self, parent: QTreeWidgetItem | None,
                   entry: dict[str, Any]) -> QTreeWidgetItem:
        name = str(entry.get("name", "?"))
        value_type = str(entry.get("type", ""))
        value_text = self._display_value(entry)
        object_id = str(entry.get("object_id", ""))
        reference = ""
        if object_id and value_type in ("table", "userdata"):
            ancestor = parent
            while ancestor is not None:
                ancestor_data = self._data(ancestor) or {}
                if ancestor_data.get("object_id") == object_id:
                    reference = ancestor_data.get("expression", "")
                    break
                ancestor = ancestor.parent()
            if not reference and object_id == self._root_object_id:
                reference = self._root_expression
        if reference:
            value_text = f"↩ Циклическая ссылка: {reference}"
        item = QTreeWidgetItem(
            [name, value_type, value_text[:VALUE_PREVIEW_LIMIT], ""]
        )
        if parent is not None:
            parent.addChild(item)
        else:
            self.tree.addTopLevelItem(item)
        expression = str(entry.get("expression", ""))
        data = {
            "kind": _ENTRY,
            "expression": expression,
            "type": value_type,
            "value": value_text,
            "value_truncated": bool(entry.get("value_truncated")),
            "value_lossy": bool(entry.get("value_lossy")),
            "object_id": object_id,
            "reference": reference,
            "expandable": (bool(entry.get("expandable"))
                           and bool(expression) and not reference),
            "writable": bool(entry.get("writable")),
            "loaded": False,
            "loading": False,
            "generation": self._generation,
        }
        item.setData(0, Qt.UserRole, data)
        item.setToolTip(0, name)
        item.setToolTip(2, value_text)
        if expression:
            self._expr_items.setdefault(expression, []).append(item)
        watchable = (
            value_type in _SCALAR_TYPES
            and bool(expression)
            and value_type != "error"
        )
        if watchable:
            item.setFlags(item.flags() | Qt.ItemIsUserCheckable)
            self._set_check(item, expression in self._watched)
        else:
            item.setFlags(item.flags() & ~Qt.ItemIsUserCheckable)
        if data["expandable"]:
            dummy = QTreeWidgetItem(item, ["…", "", "", ""])
            dummy.setData(0, Qt.UserRole, {"kind": _PLACEHOLDER})
        return item

    @staticmethod
    def _display_value(entry: dict[str, Any]) -> str:
        value = entry.get("value")
        if value is None:
            return "nil" if entry.get("type") == "nil" else ""
        if isinstance(value, bool):
            return "true" if value else "false"
        return str(value)

    @staticmethod
    def _data(item: QTreeWidgetItem | None) -> dict[str, Any] | None:
        if item is None:
            return None
        data = item.data(0, Qt.UserRole)
        return data if isinstance(data, dict) else None

    # ------------------------------------------------------------------
    # Watch checkboxes

    def _set_check(self, item: QTreeWidgetItem, checked: bool) -> None:
        self._suppress_check = True
        try:
            item.setCheckState(
                3, Qt.Checked if checked else Qt.Unchecked
            )
        finally:
            self._suppress_check = False

    def _on_item_changed(self, item: QTreeWidgetItem, column: int) -> None:
        if self._suppress_check or column != 3:
            return
        data = self._data(item)
        if not data or data["kind"] != _ENTRY:
            return
        expression = data.get("expression", "")
        if not expression:
            return
        checked = item.checkState(3) == Qt.Checked
        if checked:
            if expression in self._watched:
                return
            if len(self._watched) >= MAX_WATCH_EXPRESSIONS:
                self._set_check(item, False)
                self._set_status(
                    f"Лимит наблюдений: {MAX_WATCH_EXPRESSIONS}"
                )
                return
            self._owned.add(expression)
            self.watch_requested.emit(expression, True)
            return
        if expression in self._owned:
            self._owned.discard(expression)
            self.watch_requested.emit(expression, False)
            return
        if expression in self._watched:
            self._set_check(item, True)
            self._set_status(
                "Выражение наблюдается в списке величин слева"
            )

    @Slot(list)
    def set_watched_expressions(self, expressions: list[str]) -> None:
        self._watched = set(expressions)
        self._owned &= self._watched
        for expression in list(self._expr_items):
            items = [item for item in self._expr_items[expression]
                     if self._item_alive(item)]
            if items:
                self._expr_items[expression] = items
            else:
                del self._expr_items[expression]
                continue
            watched = expression in self._watched
            for item in items:
                if item.flags() & Qt.ItemIsUserCheckable:
                    self._set_check(item, watched)

    def watched_browser_expressions(self) -> list[str]:
        return sorted(self._owned)

    # ------------------------------------------------------------------
    # Assignment

    def _selected_writable(self) -> tuple[QTreeWidgetItem, dict[str, Any]] | None:
        item = self.tree.currentItem()
        data = self._data(item)
        if item is None or not data or data["kind"] != _ENTRY:
            return None
        if not data.get("writable") or not data.get("expression"):
            return None
        # Only values loaded in the current connection/generation can be
        # edited; a stale snapshot is never written to a new controller.
        if data.get("generation") != self._generation:
            return None
        return item, data

    @Slot()
    def edit_selected(self) -> None:
        target = self._selected_writable()
        if not self._connected or self._pending_assignment or not target:
            return
        self._open_edit_dialog(*target)

    def _on_item_double_clicked(self, item: QTreeWidgetItem,
                                column: int) -> None:
        if column != 2 or not self._connected or self._pending_assignment:
            return
        target = self._selected_writable()
        if target and target[0] is item:
            self._open_edit_dialog(*target)

    def _open_edit_dialog(self, item: QTreeWidgetItem,
                          data: dict[str, Any]) -> None:
        expression = data["expression"]
        generation = data.get("generation", self._generation)
        target = self._target
        dialog = _EditValueDialog(
            self,
            expression,
            data.get("type", ""),
            data.get("value", ""),
            bool(data.get("value_truncated")),
            bool(data.get("value_lossy")),
        )
        if dialog.exec() != QDialog.Accepted:
            return
        # The connection or controller may have changed while the modal
        # dialog was open; never even offer to write a stale snapshot.
        if (
            not self._connected
            or self._generation != generation
            or self._target != target
        ):
            return
        try:
            value_type, value = dialog.result_value()
        except ValueError as exc:
            QMessageBox.warning(self, "Изменить значение", str(exc))
            return
        host = self._target or "контроллер"
        shown = value if len(value) <= CONFIRM_VALUE_LIMIT else (
            value[:CONFIRM_VALUE_LIMIT] + "…")
        shown_target = expression if len(expression) <= CONFIRM_VALUE_LIMIT else (
            expression[:CONFIRM_VALUE_LIMIT] + "…")
        nil_warning = (
            "ВНИМАНИЕ: nil удалит поле таблицы или глобальную "
            "переменную.\n\n" if value_type == "nil" else ""
        )
        answer = QMessageBox.question(
            self,
            "Запись переменной",
            (
                nil_warning
                + f"Записать значение на {host}?\n\n"
                f"Цель: {shown_target}\n"
                f"Тип: {value_type}\n"
                f"Значение: {shown if shown else '(пусто)'}\n\n"
                "Значение будет применено в следующем цикле контроллера "
                "и может изменить ход управляющей программы."
            ),
            QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No,
            QMessageBox.StandardButton.No,
        )
        # Recheck again: the confirmation was modal too.
        if (
            answer != QMessageBox.StandardButton.Yes
            or not self._connected
            or self._generation != generation
            or self._target != target
        ):
            return
        self._next_request_id += 1
        request_id = self._next_request_id
        self._pending_assignment = (request_id, expression, generation)
        self._update_enabled()
        self.assignment_requested.emit(
            expression, value_type, value, request_id
        )

    @Slot(str, int, dict)
    def show_assignment_result(self, expression: str, request_id: int,
                               result: dict[str, Any]) -> None:
        pending = self._pending_assignment
        if (
            pending is None
            or pending[0] != request_id
            or pending[1] != expression
            or pending[2] != self._generation
            or not self._connected
        ):
            return
        self._pending_assignment = None
        self._update_enabled()
        value_type = str(result.get("type", ""))
        shown = self._display_value(result)
        self._set_status(
            f"Записано {expression} = {shown[:200]} ({value_type})"
        )
        value_truncated = bool(result.get("value_truncated"))
        value_lossy = bool(result.get("value_lossy"))
        items = [item for item in self._expr_items.get(expression, [])
                 if self._item_alive(item)]
        for item in items:
            data = self._data(item)
            if not data:
                continue
            data["type"] = value_type
            data["value"] = shown
            data["value_truncated"] = value_truncated
            data["value_lossy"] = value_lossy
            data["writable"] = value_type in _SCALAR_TYPES
            item.setData(0, Qt.UserRole, data)
            item.setText(1, value_type)
            item.setText(2, shown[:VALUE_PREVIEW_LIMIT])
            item.setToolTip(2, shown)
            if item is self.tree.currentItem():
                self._show_details(item, None)
        if value_type == "nil":
            self._refresh_parent(expression)

    @Slot(str, int, str)
    def show_assignment_error(self, expression: str, request_id: int,
                              message: str) -> None:
        pending = self._pending_assignment
        if (
            pending is None
            or pending[0] != request_id
            or pending[1] != expression
            or pending[2] != self._generation
        ):
            return
        self._pending_assignment = None
        self._update_enabled()
        self._set_status(f"{expression}: {message}")
        self.detail.setPlainText(f"{expression}: {message}")

    def _refresh_parent(self, expression: str) -> None:
        items = [item for item in self._expr_items.get(expression, [])
                 if self._item_alive(item)]
        parent = items[0].parent() if items else None
        if parent is None:
            self.load_root()
            return
        data = self._data(parent)
        if data and data["expandable"]:
            self._request_children(parent, data["expression"], 0)

    # ------------------------------------------------------------------
    # Details and persistence

    def _show_details(self, item: QTreeWidgetItem | None,
                      _previous: QTreeWidgetItem | None) -> None:
        # A Qt item may already be deleted when the selection changes.
        data = self._data(item) if item is None or self._item_alive(item) \
            else None
        if not data or data["kind"] != _ENTRY:
            self.detail.setPlainText("")
            self._update_enabled()
            return
        lines = [f"Выражение: {data.get('expression', '')}",
                 f"Тип: {data.get('type', '')}",
                 f"Значение: {data.get('value', '')}"]
        if data.get("value_truncated"):
            lines.append(
                "Показано сокращённое значение — полный текст недоступен "
                "(используйте разовое вычисление выражения)"
            )
        if data.get("value_lossy"):
            lines.append(
                "Предпросмотр содержит экранированные байты и не совпадает "
                "с исходным значением — для записи введите новое значение"
            )
        if data.get("generation") != self._generation or not self._connected:
            lines.append("Снимок устарел — обновите узел")
        if data.get("writable"):
            lines.append("Запись разрешена (двойной щелчок по значению)")
        self.detail.setPlainText("\n".join(lines))
        self._update_enabled()

    def completion_expressions(self) -> list[str]:
        expressions = set()
        for expression, items in self._expr_items.items():
            records = [self._data(item) for item in items
                       if self._item_alive(item)]
            records = [data for data in records
                       if data and data.get("type") != "error"]
            if not expression or not records:
                continue
            normalized = expression
            if _LUA_NAME_PATH.fullmatch(expression):
                normalized = re.sub(
                    rf"\[([\"'])({_LUA_IDENTIFIER})\1\]",
                    lambda match: (match.group(0)
                                   if match.group(2) in _LUA_KEYWORDS
                                   else "." + match.group(2)),
                    expression,
                )
            paths = {expression, normalized}
            field = re.search(
                rf"\[([\"'])({_LUA_IDENTIFIER})\1\]\Z", expression,
            )
            if field and field.group(2) not in _LUA_KEYWORDS:
                paths.add(expression[:field.start()] + "." + field.group(2))
            if normalized.startswith("_G."):
                paths.add(normalized[3:])
            if any(data.get("type") == "function" for data in records):
                for path in tuple(paths):
                    parent, separator, name = path.rpartition(".")
                    if separator and re.fullmatch(_LUA_IDENTIFIER, name):
                        paths.add(parent + ":" + name)
            expressions.update(paths)
        if expressions and self._root_expression:
            expressions.add(self._root_expression)
        return sorted(expressions)

    def snapshot_state(self) -> dict[str, Any]:
        return {
            "root": self.root_edit.text().strip() or "_G",
            "watched_expressions": sorted(self._owned),
        }

    def restore_state(self, state: dict[str, Any] | None) -> list[str]:
        """Applies a saved browser section and returns the watch
        expressions that the session widget should re-register."""
        if not isinstance(state, dict):
            return []
        root = state.get("root", "_G")
        self.root_edit.setText(root if isinstance(root, str) and root else "_G")
        owned = [item for item in state.get("watched_expressions", [])
                 if isinstance(item, str)][:MAX_WATCH_EXPRESSIONS]
        self._owned = set(owned)
        return owned


class _EditValueDialog(QDialog):
    """Value editor for a single scalar variable target."""

    def __init__(self, parent: QWidget, expression: str,
                 current_type: str, current_value: str,
                 value_truncated: bool = False,
                 value_lossy: bool = False) -> None:
        super().__init__(parent)
        self.setWindowTitle("Изменить значение")
        self._expression = expression
        self._current_type = current_type
        # Kept verbatim so an untouched string editor returns exactly
        # the bytes that were read (QPlainTextEdit normalizes CRLF).
        self._original_value = current_value
        # Unreliable previews (truncated or byte-escaped) are never
        # offered as the new value.
        self._value_unreliable = value_truncated or value_lossy
        form = QFormLayout(self)
        target = QPlainTextEdit(expression)
        target.setReadOnly(True)
        target.setMaximumHeight(48)
        form.addRow("Переменная", target)
        form.addRow("Текущий тип", QLabel(current_type))
        current = QPlainTextEdit(current_value)
        current.setReadOnly(True)
        current.setMaximumHeight(70)
        form.addRow("Текущее значение", current)

        self.type_combo = QComboBox()
        for value_type in ("number", "boolean", "string", "nil"):
            self.type_combo.addItem(value_type)
        if current_type in ("number", "boolean", "string"):
            self.type_combo.setCurrentText(current_type)
        form.addRow("Новый тип", self.type_combo)

        self.number_edit = QLineEdit(
            current_value if current_type == "number" else ""
        )
        self.number_edit.setPlaceholderText("Например: 12.5 или 1e3")
        self.boolean_combo = QComboBox()
        self.boolean_combo.addItems(["true", "false"])
        if current_type == "boolean" and current_value in ("true", "false"):
            self.boolean_combo.setCurrentText(current_value)
        # An unreliable preview is never offered as the new value: the
        # editor stays blank so an unchanged partial or escaped string
        # cannot be written back as the complete value.
        self.string_edit = QPlainTextEdit(
            current_value
            if current_type == "string" and not self._value_unreliable
            else ""
        )
        self.string_edit.setMaximumHeight(110)
        # setPlainText does not count as editing; only real changes do.
        self._string_edited = False
        self.string_edit.textChanged.connect(self._mark_string_edited)
        reasons = []
        if value_truncated:
            reasons.append("Текущее значение показано не полностью")
        if value_lossy:
            reasons.append(
                "предпросмотр содержит экранированные байты, "
                "не совпадающие с исходным значением"
            )
        self.truncated_label = QLabel(
            (". ".join(reasons) + ". " if reasons else "")
            + "Введите полное новое значение; для чтения используйте "
            "разовое вычисление выражения."
        )
        self.truncated_label.setWordWrap(True)
        self.truncated_label.setStyleSheet("color: #d35400")
        self.truncated_label.setVisible(self._value_unreliable)
        form.addRow(self.truncated_label)
        self.nil_label = QLabel(
            "nil удаляет поле таблицы или глобальную переменную"
        )
        self._editors = {
            "number": self.number_edit,
            "boolean": self.boolean_combo,
            "string": self.string_edit,
            "nil": self.nil_label,
        }
        self._value_row = QHBoxLayout()
        for widget in self._editors.values():
            self._value_row.addWidget(widget)
        form.addRow("Новое значение", self._value_row)
        self.type_combo.currentTextChanged.connect(self._type_changed)
        self._type_changed(self.type_combo.currentText())

        self.error_label = QLabel("")
        self.error_label.setStyleSheet("color: #e74c3c")
        form.addRow(self.error_label)

        buttons = QDialogButtonBox(
            QDialogButtonBox.Ok | QDialogButtonBox.Cancel
        )
        buttons.accepted.connect(self._accept)
        buttons.rejected.connect(self.reject)
        form.addRow(buttons)

    def _mark_string_edited(self) -> None:
        self._string_edited = True

    def _type_changed(self, value_type: str) -> None:
        for kind, widget in self._editors.items():
            widget.setVisible(kind == value_type)

    def _accept(self) -> None:
        try:
            self._result = self.result_value()
        except ValueError as exc:
            self.error_label.setText(str(exc))
            return
        self.accept()

    def result_value(self) -> tuple[str, str]:
        value_type = self.type_combo.currentText()
        if value_type == "number":
            text = self.number_edit.text().strip()
            if not _NUMBER_PATTERN.fullmatch(text):
                raise ValueError("Введите конечное число (десятичное или 1e3)")
            if not math.isfinite(float(text)):
                raise ValueError("Число должно быть конечным")
            return "number", text
        if value_type == "boolean":
            return "boolean", self.boolean_combo.currentText()
        if value_type == "string":
            edited = (
                self._string_edited
                or self.string_edit.document().isModified()
            )
            if self._value_unreliable and not edited:
                raise ValueError(
                    "Точное текущее значение неизвестно — введите "
                    "новое значение или отмените запись"
                )
            if (
                not edited
                and self._current_type == "string"
                and not self._value_unreliable
            ):
                # An untouched editor returns the original verbatim:
                # the widget's CRLF normalization cannot alter data.
                return "string", self._original_value
            return "string", self.string_edit.toPlainText()
        return "nil", ""
