from __future__ import annotations

import json
import re
from collections.abc import Callable
from dataclasses import replace
from datetime import datetime
from pathlib import Path
from typing import Any

import pyqtgraph as pg
from PySide6.QtCore import (
    QMetaObject, QStringListModel, Qt, QThread, QTimer, Signal, Slot,
)
from PySide6.QtGui import (
    QAction, QCloseEvent, QKeyEvent, QKeySequence, QTextCursor,
)
from PySide6.QtWidgets import (
    QAbstractItemView,
    QCheckBox,
    QComboBox,
    QCompleter,
    QColorDialog,
    QDoubleSpinBox,
    QDialog,
    QFormLayout,
    QTreeWidget,
    QTreeWidgetItem,
    QFileDialog,
    QHBoxLayout,
    QHeaderView,
    QLabel,
    QLineEdit,
    QMainWindow,
    QMessageBox,
    QPlainTextEdit,
    QPushButton,
    QSpinBox,
    QSplitter,
    QTableWidget,
    QTableWidgetItem,
    QTableView,
    QTabWidget,
    QToolButton,
    QVBoxLayout,
    QWidget,
)
from xlsxwriter.exceptions import XlsxWriterException

from .excel_export import export_history_xlsx
from .chart_styles import CHART_TYPES, DEFAULT_CHART_TYPE
from .history import (
    DEFAULT_DISPLAY_SECONDS,
    DEFAULT_HISTORY_LIMIT,
    MAX_DISPLAY_SECONDS,
    MAX_HISTORY_LIMIT,
    client_timestamp_ms,
    controller_timestamp_ms,
    merge_chart_data,
    merge_statistics,
    trim_chart_data,
)
from .history_table import HistoryTableModel
from .protocol import MAX_REQUEST_PATH_BYTES
from .pulse_counter import PulseCounters, PulseDefinition
from .session_store import load_session, save_session
from .setter import setter_code
from .variable_browser import MAX_WATCH_EXPRESSIONS, VariableBrowserWidget
from .worker import DebuggerWorker

CONTROLLER_COMMANDS = (
    (301, "PHOENIX Modbus UDP: включить"),
    (302, "PHOENIX Modbus UDP: выключить"),
    (102, "Принудительно сохранить параметры"),
    (100, "Перезагрузить ограничения"),
    (101, "Сбросить параметры"),
    (0, "Очистить код результата"),
)

DEFAULT_LOG_INTERVAL_MINUTES = 60
MAX_LOG_INTERVAL_MINUTES = 7 * 24 * 60

RECONNECT_INITIAL_DELAY_SECONDS = 1
RECONNECT_DELAY_STEP_SECONDS = 5
RECONNECT_MAX_DELAY_SECONDS = 60


class LuaConsoleEdit(QPlainTextEdit):
    _IDENTIFIER = r"[A-Za-z_][A-Za-z0-9_]*"
    _KEY = (r'''(?:"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|'''
            r"-?[0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?|true|false)")
    _PREFIX = re.compile(
        rf"(?<![A-Za-z0-9_]){_IDENTIFIER}"
        rf"(?:\.{_IDENTIFIER}|\[{_KEY}\])*"
        rf"(?:[.:](?:{_IDENTIFIER})?|\[)?\Z"
    )
    _LEXEME = re.compile(r'''--|\[(=*)\[|["']''')

    def __init__(self, expressions: Callable[[], list[str]]) -> None:
        super().__init__()
        self._expressions = expressions
        self._model = QStringListModel(self)
        self._completer = QCompleter(self._model, self)
        self._completer.setWidget(self)
        self._completer.setCaseSensitivity(Qt.CaseSensitive)
        self._completer.setCompletionMode(QCompleter.PopupCompletion)
        self._completer.activated[str].connect(self._insert_completion)

    def _completion_prefix(self) -> str:
        cursor = self.textCursor()
        if cursor.hasSelection():
            return ""
        cursor.setPosition(0, QTextCursor.KeepAnchor)
        source = cursor.selectedText().replace("\u2029", "\n")
        position = 0
        while token := self._LEXEME.search(source, position):
            start = token.end()
            opening = (re.match(r"\[(=*)\[", source[start:])
                       if token.group(0) == "--" else None)
            level = (opening.group(1) if opening else token.group(1))
            if level is not None:
                closing = "]" + level + "]"
                end = source.find(closing, start +
                                  (opening.end() if opening else 0))
                if end < 0:
                    return ""
                position = end + len(closing)
            elif token.group(0) == "--":
                end = source.find("\n", start)
                if end < 0:
                    return ""
                position = end + 1
            else:
                while start < len(source):
                    if source[start] == "\\":
                        start += 2
                    elif source[start] == token.group(0):
                        break
                    else:
                        start += 1
                if start >= len(source):
                    return ""
                position = start + 1
        match = self._PREFIX.search(source)
        return match.group(0) if match else ""

    @Slot(str)
    def _insert_completion(self, completion: str) -> None:
        prefix = self._completer.completionPrefix()
        if not prefix or self._completion_prefix() != prefix:
            return
        cursor = self.textCursor()
        start = cursor.position() - len(prefix.encode("utf-16-le")) // 2
        end = cursor.position()
        cursor.movePosition(QTextCursor.EndOfBlock, QTextCursor.KeepAnchor)
        suffix = re.match(r"[A-Za-z0-9_]*", cursor.selectedText()).group(0)
        cursor.setPosition(start)
        cursor.setPosition(end + len(suffix), QTextCursor.KeepAnchor)
        cursor.insertText(completion)
        self.setTextCursor(cursor)
        self._completer.popup().hide()

    def keyPressEvent(self, event: QKeyEvent) -> None:
        popup = self._completer.popup()
        if popup.isVisible() and event.key() in (
            Qt.Key_Tab, Qt.Key_Return, Qt.Key_Enter, Qt.Key_Escape,
        ):
            if event.key() == Qt.Key_Escape:
                popup.hide()
            else:
                completion = popup.currentIndex().data()
                if completion:
                    self._insert_completion(completion)
            event.accept()
            return
        popup.hide()
        if event.key() == Qt.Key_Tab and event.modifiers() == Qt.NoModifier:
            prefix = self._completion_prefix()
            suffix_pattern = (self._KEY + r"\]" if prefix.endswith("[")
                              else r"[A-Za-z0-9_]*")
            matches = sorted({
                expression for expression in self._expressions()
                if prefix and expression.startswith(prefix)
                and re.fullmatch(suffix_pattern, expression[len(prefix):])
            })
            if matches:
                self._model.setStringList(matches)
                self._completer.setCompletionPrefix(prefix)
                if len(matches) == 1:
                    self._insert_completion(matches[0])
                else:
                    popup.setCurrentIndex(
                        self._completer.completionModel().index(0, 0)
                    )
                    rectangle = self.cursorRect()
                    rectangle.setWidth(max(280, popup.sizeHintForColumn(0)
                                           + popup.verticalScrollBar()
                                           .sizeHint().width()))
                    self._completer.complete(rectangle)
                event.accept()
                return
        super().keyPressEvent(event)


class DebuggerSessionWidget(QWidget):
    connect_requested = Signal(str, int)
    disconnect_requested = Signal()
    expressions_requested = Signal(list)
    interval_requested = Signal(int)
    evaluate_requested = Signal(str)
    execute_requested = Signal(str)
    lua_executed = Signal(str, dict)
    controller_command_requested = Signal(int)
    reload_objects_requested = Signal()
    clear_requested = Signal()
    title_changed = Signal(str)

    def __init__(self) -> None:
        super().__init__()
        self._connected = False
        self._command_pending = False
        self._lua_pending = False
        self._shutting_down = False
        self._connection_pending = False
        self._connection_generation = 0
        self._reconnect_delay_seconds = RECONNECT_INITIAL_DELAY_SECONDS
        self._scheduled_reconnect_delay_seconds: int | None = None
        self._last_disconnect_reason = ""
        self._last_chart_data: dict[str, Any] | None = None
        self._statistics: dict[str, dict[str, Any]] = {}
        self._pulse_counters = PulseCounters()

        self._thread = QThread(self)
        self._worker = DebuggerWorker()
        self._worker.moveToThread(self._thread)
        self.connect_requested.connect(self._worker.connect_to)
        self.disconnect_requested.connect(self._worker.disconnect)
        self.expressions_requested.connect(self._worker.set_expressions)
        self.interval_requested.connect(self._worker.set_interval)
        self.evaluate_requested.connect(self._worker.evaluate)
        self.execute_requested.connect(self._worker.execute)
        self._worker.executed.connect(self._on_executed)
        self.controller_command_requested.connect(
            self._worker.execute_controller_command
        )
        self.reload_objects_requested.connect(self._worker.refresh_reload_objects)
        self._worker.reload_objects_loaded.connect(self._on_reload_objects)
        self._worker.reload_objects_failed.connect(self._on_reload_objects_error)
        self._variable_browser = VariableBrowserWidget()
        self._variable_browser.browse_requested.connect(
            self._worker.browse_variables
        )
        self._variable_browser.assignment_requested.connect(
            self._worker.set_variable
        )
        self._variable_browser.watch_requested.connect(
            self._on_browser_watch
        )
        self._worker.browse_loaded.connect(
            self._variable_browser.show_browse_result
        )
        self._worker.browse_failed.connect(
            self._variable_browser.show_browse_error
        )
        self._worker.assignment_done.connect(
            self._variable_browser.show_assignment_result
        )
        self._worker.assignment_failed.connect(
            self._variable_browser.show_assignment_error
        )
        self.clear_requested.connect(self._worker.clear_chart_data)
        self._worker.connected.connect(self._on_connected)
        self._worker.disconnected.connect(self._on_disconnected)
        self._worker.chart_data.connect(self._on_chart_data)
        self._worker.messages.connect(self._on_messages)
        self._worker.evaluated.connect(self._on_evaluated)
        self._worker.command_executed.connect(self._on_command_executed)
        self._worker.error.connect(self._show_error)
        self._thread.start()

        self._logging_timer = QTimer(self)
        self._logging_timer.timeout.connect(self._rotate_log)

        self._reconnect_timer = QTimer(self)
        self._reconnect_timer.setSingleShot(True)
        self._reconnect_timer.timeout.connect(self._retry_connection)

        self._build_ui()

    def _build_ui(self) -> None:
        self.host_edit = QLineEdit("127.0.0.1")
        self.host_edit.setMinimumWidth(150)
        self.host_edit.setMaximumWidth(280)
        self.host_edit.textChanged.connect(self._update_title)
        self.port_spin = QSpinBox()
        self.port_spin.setRange(1, 65_535)
        self.port_spin.setValue(10_000)
        self.port_spin.valueChanged.connect(self._update_title)
        self.interval_spin = QSpinBox()
        self.interval_spin.setRange(100, 9_000)
        self.interval_spin.setSingleStep(100)
        self.interval_spin.setSuffix(" мс")
        self.interval_spin.setValue(500)
        self.interval_spin.valueChanged.connect(self.interval_requested)
        self.history_limit_spin = QSpinBox()
        self.history_limit_spin.setRange(1, MAX_HISTORY_LIMIT)
        self.history_limit_spin.setSingleStep(1_000)
        self.history_limit_spin.setValue(DEFAULT_HISTORY_LIMIT)
        self.history_limit_spin.setToolTip("Максимум точек на каждое выражение")
        self.history_limit_spin.valueChanged.connect(self._history_limit_changed)
        self.display_seconds_spin = QSpinBox()
        self.display_seconds_spin.setRange(1, MAX_DISPLAY_SECONDS)
        self.display_seconds_spin.setSuffix(" с")
        self.display_seconds_spin.setValue(DEFAULT_DISPLAY_SECONDS)
        self.display_seconds_spin.setToolTip(
            "Интервал, отображаемый на графике"
        )
        self.display_seconds_spin.valueChanged.connect(self._display_settings_changed)
        self.auto_follow_check = QCheckBox("Авто")
        self.auto_follow_check.setChecked(True)
        self.auto_follow_check.setToolTip(
            "Автоматически показывать последние N секунд"
        )
        self.auto_follow_check.toggled.connect(self._display_settings_changed)
        self.auto_reconnect_check = QCheckBox("Переподключаться")
        self.auto_reconnect_check.setToolTip(
            "Автоматически повторять подключение через 1–60 секунд"
        )
        self.auto_reconnect_check.toggled.connect(self._auto_reconnect_toggled)
        self.connect_button = QPushButton("Подключиться")
        self.connect_button.setProperty("primary", True)
        self.connect_button.clicked.connect(self._toggle_connection)

        connection = QHBoxLayout()
        connection.addWidget(QLabel("Контроллер:"))
        connection.addWidget(self.host_edit, 1)
        connection.addWidget(QLabel("Порт:"))
        connection.addWidget(self.port_spin)
        connection.addWidget(QLabel("Опрос:"))
        connection.addWidget(self.interval_spin)
        connection.addWidget(self.auto_reconnect_check)
        connection.addStretch(1)
        connection.addWidget(self.connect_button)

        self.logging_check = QCheckBox("Логирование")
        self.logging_check.setToolTip(
            "Периодически сохранять сессию и начинать новую историю"
        )
        self.logging_interval_spin = QSpinBox()
        self.logging_interval_spin.setRange(1, MAX_LOG_INTERVAL_MINUTES)
        self.logging_interval_spin.setValue(DEFAULT_LOG_INTERVAL_MINUTES)
        self.logging_interval_spin.setSuffix(" мин")
        self.logging_interval_spin.valueChanged.connect(
            self._logging_interval_changed
        )
        self.logging_directory_edit = QLineEdit()
        self.logging_directory_edit.setPlaceholderText(
            "Каталог файлов журнала"
        )
        logging_browse_button = QPushButton("Обзор...")
        logging_browse_button.clicked.connect(self._choose_logging_directory)
        self.logging_check.toggled.connect(self._logging_toggled)

        logging = QHBoxLayout()
        logging.addWidget(self.logging_check)
        logging.addWidget(QLabel("Период:"))
        logging.addWidget(self.logging_interval_spin)
        logging.addWidget(QLabel("Каталог:"))
        logging.addWidget(self.logging_directory_edit, 1)
        logging.addWidget(logging_browse_button)

        self.expression_edit = QLineEdit()
        self.expression_edit.setPlaceholderText("Например: TE1:get_value()")
        self.expression_edit.returnPressed.connect(self._add_expression)
        add_button = QPushButton("Добавить")
        add_button.setProperty("primary", True)
        add_button.setToolTip("Добавить выражение (Enter)")
        add_button.clicked.connect(self._add_expression)
        pulse_button = QPushButton("Счётчик импульсов…")
        pulse_button.clicked.connect(self._add_pulse_counter)
        remove_button = QPushButton("Удалить")
        remove_button.clicked.connect(self._remove_expressions)
        apply_button = QPushButton("Применить")
        apply_button.clicked.connect(self._apply_expressions)
        clear_button = QPushButton("Очистить историю")
        clear_button.clicked.connect(self._clear_charts)

        expression_buttons = QHBoxLayout()
        expression_buttons.addWidget(pulse_button)
        expression_buttons.addWidget(remove_button)
        expression_buttons.addWidget(apply_button)
        expression_buttons.addStretch(1)
        expression_buttons.addWidget(clear_button)

        self.variables = QTreeWidget()
        self.variables.setColumnCount(4)
        self.variables.setHeaderLabels(
            ["Lua-выражение / свойство", "История", "Значение", "Состояние"]
        )
        self.variables.setSelectionMode(QAbstractItemView.ExtendedSelection)
        self.variables.header().setSectionResizeMode(0, QHeaderView.Interactive)
        self.variables.setColumnWidth(0, 230)
        self.variables.setAlternatingRowColors(True)
        self.variables.setTextElideMode(Qt.ElideRight)
        self.variables.header().setSectionResizeMode(1, QHeaderView.ResizeToContents)
        self.variables.header().setSectionResizeMode(2, QHeaderView.Interactive)
        self.variables.header().setSectionResizeMode(3, QHeaderView.Interactive)
        self.variables.setColumnWidth(2, 160)
        self.variables.setColumnWidth(3, 110)
        self.variables.setMinimumWidth(420)
        self.variables.itemChanged.connect(self._history_changed)

        left = QWidget()
        left_layout = QVBoxLayout(left)
        left_layout.setContentsMargins(0, 0, 0, 0)
        expression_entry = QHBoxLayout()
        expression_entry.addWidget(self.expression_edit, 1)
        expression_entry.addWidget(add_button)
        left_layout.addLayout(expression_entry)
        left_layout.addLayout(expression_buttons)
        left_layout.addWidget(self.variables)

        pg.setConfigOptions(antialias=True, foreground="#CFD8DC")
        self.plot = pg.PlotWidget(
            background="#1C252A",
            axisItems={"bottom": pg.DateAxisItem(orientation="bottom")},
        )
        self._time_axis = "controller"
        self.plot.addLegend()
        self.plot.showGrid(x=True, y=True, alpha=0.2)
        self.plot.setLabel("bottom", "Время контроллера")

        self.timeline_combo = QComboBox()
        self.timeline_combo.addItem("Реальное время", "real")
        self.timeline_combo.addItem("Время контроллера", "controller")
        self.timeline_combo.setCurrentIndex(
            self.timeline_combo.findData("controller")
        )
        self.timeline_combo.currentIndexChanged.connect(self._redraw_chart)

        self.history_model = HistoryTableModel()
        self.history_table = QTableView()
        self.history_table.setModel(self.history_model)
        self.history_table.setAlternatingRowColors(True)
        self.history_table.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.history_table.setColumnWidth(0, 190)
        self.history_table.setColumnWidth(1, 150)
        self.history_table.horizontalHeader().setStretchLastSection(True)
        export_button = QPushButton("Экспорт в Excel…")
        export_button.clicked.connect(self._export_history)
        history_page = QWidget()
        history_layout = QVBoxLayout(history_page)
        history_layout.addWidget(export_button, 0, Qt.AlignRight)
        history_layout.addWidget(self.history_table, 1)

        self.messages_table = QTableWidget(0, 4)
        self.messages_table.setHorizontalHeaderLabels(
            ["Время", "Источник", "Уровень", "Сообщение"]
        )
        self.messages_table.setEditTriggers(QAbstractItemView.NoEditTriggers)
        self.messages_table.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.messages_table.horizontalHeader().setSectionResizeMode(
            0, QHeaderView.ResizeToContents
        )
        self.messages_table.horizontalHeader().setSectionResizeMode(
            1, QHeaderView.ResizeToContents
        )
        self.messages_table.horizontalHeader().setSectionResizeMode(
            2, QHeaderView.ResizeToContents
        )
        self.messages_table.horizontalHeader().setSectionResizeMode(
            3, QHeaderView.Stretch
        )
        clear_messages_button = QPushButton("Очистить")
        clear_messages_button.clicked.connect(
            lambda: self.messages_table.setRowCount(0)
        )
        messages_page = QWidget()
        messages_layout = QVBoxLayout(messages_page)
        messages_layout.addWidget(clear_messages_button, 0, Qt.AlignRight)
        messages_layout.addWidget(self.messages_table, 1)

        chart_page = QWidget()
        chart_layout = QVBoxLayout(chart_page)
        chart_layout.setContentsMargins(8, 8, 0, 0)
        timeline_row = QHBoxLayout()
        timeline_row.addWidget(QLabel("Шкала времени:"))
        timeline_row.addWidget(self.timeline_combo)
        timeline_row.addStretch(1)
        chart_layout.addLayout(timeline_row)
        chart_settings = QHBoxLayout()
        chart_settings.addWidget(QLabel("Окно:"))
        chart_settings.addWidget(self.display_seconds_spin)
        chart_settings.addWidget(self.auto_follow_check)
        chart_settings.addStretch(1)
        chart_settings.addWidget(QLabel("Точек:"))
        chart_settings.addWidget(self.history_limit_spin)
        chart_layout.addLayout(chart_settings)
        chart_layout.addWidget(self.plot, 1)

        self.output_tabs = QTabWidget()
        self.output_tabs.addTab(chart_page, "График")
        self.output_tabs.addTab(history_page, "История")
        self.output_tabs.addTab(messages_page, "Сообщения")
        self.output_tabs.addTab(self._variable_browser, "Переменные")

        splitter = QSplitter()
        splitter.setChildrenCollapsible(False)
        splitter.setHandleWidth(8)
        splitter.addWidget(left)
        splitter.addWidget(self.output_tabs)
        splitter.setSizes([470, 710])

        self.evaluate_edit = QLineEdit()
        self.evaluate_edit.setPlaceholderText("Разовое Lua-выражение")
        self.evaluate_edit.returnPressed.connect(self._evaluate_once)
        evaluate_button = QPushButton("Вычислить")
        evaluate_button.clicked.connect(self._evaluate_once)
        evaluation = QHBoxLayout()
        evaluation.addWidget(self.evaluate_edit, 1)
        evaluation.addWidget(evaluate_button)
        execute_button = QPushButton("Выполнить Lua…")
        execute_button.clicked.connect(self._open_lua_console)
        evaluation.addWidget(execute_button)

        self.evaluate_result = QPlainTextEdit()
        self.evaluate_result.setObjectName("evaluateResult")
        self.evaluate_result.setReadOnly(True)
        self.evaluate_result.setPlaceholderText(
            "История разовых вычислений"
        )
        self.evaluate_result.setLineWrapMode(QPlainTextEdit.WidgetWidth)
        self.evaluate_result.setMaximumBlockCount(200)
        self.evaluate_result.setMinimumHeight(48)
        self.evaluate_result.setMaximumHeight(120)

        self.command_combo = QComboBox()
        for command_id, label in CONTROLLER_COMMANDS:
            self.command_combo.addItem(label, command_id)
        self.command_button = QPushButton("Выполнить")
        self.command_button.setEnabled(False)
        self.command_button.clicked.connect(self._execute_controller_command)
        self.command_result = QLabel("—")
        commands = QHBoxLayout()
        commands.addWidget(QLabel("Команда контроллера:"))
        commands.addWidget(self.command_combo)
        commands.addWidget(self.command_button)
        commands.addWidget(self.command_result, 1)

        self.reload_object_combo = QComboBox()
        self.reload_object_combo.setMinimumWidth(260)
        self.reload_object_combo.currentIndexChanged.connect(self._update_reload_controls)
        self.reload_objects_button = QPushButton("Обновить список")
        self.reload_objects_button.setEnabled(False)
        self.reload_objects_button.clicked.connect(self.reload_objects_requested.emit)
        self.reload_object_button = QPushButton("Перезагрузить объект")
        self.reload_object_button.setEnabled(False)
        self.reload_object_button.clicked.connect(self._reload_selected_object)
        self.reload_hint = QLabel("Нет подключения")
        reload_row = QHBoxLayout()
        reload_row.addWidget(QLabel("Объект:"))
        reload_row.addWidget(self.reload_object_combo)
        reload_row.addWidget(self.reload_objects_button)
        reload_row.addWidget(self.reload_object_button)
        reload_row.addWidget(self.reload_hint, 1)

        root_layout = QVBoxLayout(self)
        root_layout.setContentsMargins(12, 8, 12, 8)
        root_layout.setSpacing(8)
        root_layout.addLayout(connection)
        root_layout.addLayout(logging)
        root_layout.addWidget(splitter, 1)
        root_layout.addLayout(evaluation)
        root_layout.addWidget(self.evaluate_result)
        root_layout.addLayout(commands)
        root_layout.addLayout(reload_row)
        self.status_label = QLabel("Не подключено")
        self.status_label.setObjectName("sessionStatus")
        root_layout.addWidget(self.status_label)

    @Slot()
    def _toggle_connection(self) -> None:
        if self._connected:
            self._reset_reconnect()
            self.disconnect_requested.emit()
            return
        self._reset_reconnect()
        self._start_connection()

    def _reset_reconnect(self) -> None:
        self._reconnect_timer.stop()
        self._reconnect_delay_seconds = RECONNECT_INITIAL_DELAY_SECONDS
        self._scheduled_reconnect_delay_seconds = None
        self._last_disconnect_reason = ""

    def _schedule_reconnect(self) -> None:
        if self._scheduled_reconnect_delay_seconds is None:
            self._scheduled_reconnect_delay_seconds = self._reconnect_delay_seconds
            self._reconnect_delay_seconds = min(
                self._reconnect_delay_seconds + RECONNECT_DELAY_STEP_SECONDS,
                RECONNECT_MAX_DELAY_SECONDS,
            )
        delay = self._scheduled_reconnect_delay_seconds
        self._reconnect_timer.start(delay * 1_000)
        self.status_label.setText(
            f"{self._last_disconnect_reason} · "
            f"повторное подключение через {delay} с"
        )

    def _start_connection(self) -> None:
        if self._connected or self._connection_pending or self._shutting_down:
            return
        self._reconnect_timer.stop()
        self._scheduled_reconnect_delay_seconds = None
        self._connection_pending = True
        self.connect_button.setEnabled(False)
        self.status_label.setText("Подключение…")
        self.interval_requested.emit(self.interval_spin.value())
        self.connect_requested.emit(
            self.host_edit.text().strip(), self.port_spin.value()
        )

    @Slot()
    def _retry_connection(self) -> None:
        self._start_connection()

    @Slot(bool)
    def _auto_reconnect_toggled(self, enabled: bool) -> None:
        if enabled:
            if (not self._connected and not self._connection_pending
                    and self._last_disconnect_reason):
                self._schedule_reconnect()
            return
        self._reconnect_timer.stop()
        if not self._connected and self._last_disconnect_reason:
            self.status_label.setText(self._last_disconnect_reason)

    @Slot(str)
    def _on_connected(self, session_id: str) -> None:
        self._connected = True
        self._connection_generation += 1
        self._connection_pending = False
        self._reset_reconnect()
        self.connect_button.setEnabled(True)
        self.connect_button.setText("Отключиться")
        self.command_button.setEnabled(True)
        self._command_pending = False
        self._update_reload_controls()
        self.reload_hint.setText("Загрузка списка…")
        self.reload_objects_requested.emit()
        self.status_label.setText(f"Подключено · сессия {session_id}")
        self._variable_browser.set_target(
            f"{self.host_edit.text().strip()}:{self.port_spin.value()}"
        )
        self._variable_browser.set_connected(True)
        self._update_title()
        self._apply_expressions()

    @Slot(str)
    def _on_disconnected(self, reason: str) -> None:
        self._connected = False
        self._connection_generation += 1
        self._lua_pending = False
        self._connection_pending = False
        self._variable_browser.set_connected(False)
        self.connect_button.setEnabled(True)
        self.connect_button.setText("Подключиться")
        self.command_button.setEnabled(False)
        self._command_pending = False
        self.reload_object_combo.clear()
        self.reload_hint.setText("Нет подключения")
        self._update_reload_controls()
        if self.command_result.text() == "Выполнение…":
            self.command_result.setText(reason or "Нет подключения")
        if reason:
            self._last_disconnect_reason = reason
            if (self.auto_reconnect_check.isChecked()
                    and not self._shutting_down):
                self._schedule_reconnect()
            else:
                self.status_label.setText(reason)
        else:
            self._reset_reconnect()
            self.status_label.setText("Не подключено")
        self._update_title()

    @Slot()
    def _add_expression(self) -> None:
        expression = self.expression_edit.text().strip()
        if not expression:
            return
        if expression in [self._item_expression(item) for item in self._expression_items()]:
            self.expression_edit.clear()
            return
        self._create_expression(expression)
        self.expression_edit.clear()
        self._apply_expressions()

    @Slot()
    def _add_pulse_counter(self) -> None:
        dialog = QDialog(self)
        dialog.setWindowTitle("Счётчик импульсов")
        form = QFormLayout(dialog)
        name = QLineEdit()
        name.setPlaceholderText("Например: левый датчик")
        source = QComboBox()
        dependent = QComboBox()
        for combo in (source, dependent):
            combo.setEditable(True)
            combo.addItems(self._expressions())
            combo.setCurrentText("")
        source_value = QLineEdit("1")
        dependent_value = QLineEdit("1")
        for edit in (source_value, dependent_value):
            edit.setToolTip('Число, true/false или строка в кавычках, например "RUN"')
        form.addRow("Имя", name)
        form.addRow("Считаемый датчик", source)
        form.addRow("Её значение", source_value)
        form.addRow("Ведомый датчик", dependent)
        form.addRow("Её значение", dependent_value)
        form.addRow(QLabel("Первый импульс ведомого датчика фиксирует число импульсов\n"
                           "считаемого датчика. График показывает итог последней серии.\n"
                           "Указывайте значения датчиков без сдвига линии по Y."))
        create = QPushButton("Создать")
        form.addRow(create)

        def accept() -> None:
            try:
                definition = PulseDefinition(
                    name.text().strip(), source.currentText().strip(),
                    dependent.currentText().strip(), json.loads(source_value.text()),
                    json.loads(dependent_value.text()),
                )
                if definition.expression in [self._item_expression(item) for item in self._expression_items()]:
                    raise ValueError("Такое имя уже есть в списке величин")
                self._pulse_counters.add(definition)
            except (ValueError, json.JSONDecodeError) as exc:
                QMessageBox.warning(dialog, "Счётчик импульсов", str(exc))
                return
            for expression in (definition.source, definition.dependent):
                if expression not in self._expressions():
                    self._create_expression(expression, history_enabled=True)
            self._create_expression(definition.expression, history_enabled=True,
                                    style={"points": True})
            if self._last_chart_data is not None:
                self._pulse_counters.seed(
                    definition.expression, self._last_chart_data)
            self._apply_expressions()
            dialog.accept()

        create.clicked.connect(accept)
        dialog.exec()

    @Slot()
    def _remove_expressions(self) -> None:
        items = set()
        for item in self.variables.selectedItems():
            while item.parent() is not None:
                item = item.parent()
            items.add(item)
        self._remove_expression_items(items)

    def _remove_expression_items(self, items: set[QTreeWidgetItem]) -> None:
        removed = {self._item_expression(item) for item in items}
        for expression, definition in self._pulse_counters.definitions.items():
            if definition.source in removed or definition.dependent in removed:
                for item in self._expression_items():
                    if self._item_expression(item) == expression:
                        items.add(item)
                        break
        for item in items:
            self._pulse_counters.remove(self._item_expression(item))
            self.variables.takeTopLevelItem(self.variables.indexOfTopLevelItem(item))
        self._history_changed(None)
        self._apply_expressions()

    @Slot(str, bool)
    def _on_browser_watch(self, expression: str, watched: bool) -> None:
        if watched:
            if expression in self._expressions():
                self._variable_browser.set_watched_expressions(
                    self._expressions()
                )
                return
            if len(self._expressions()) >= MAX_WATCH_EXPRESSIONS:
                self._variable_browser.set_watched_expressions(
                    self._expressions()
                )
                self.status_label.setText(
                    f"Лимит выражений: {MAX_WATCH_EXPRESSIONS}"
                )
                return
            self._create_expression(expression)
            self._apply_expressions()
        else:
            pulse_users = [
                name
                for name, definition
                in self._pulse_counters.definitions.items()
                if expression in (definition.source, definition.dependent)
            ]
            if pulse_users:
                # The expression feeds a pulse counter; removing the
                # watch would silently delete the counter, so keep it
                # watched and reflect that back to the browser.
                self.status_label.setText(
                    f"{expression} используется счётчиком импульсов "
                    f"{pulse_users[0]} — оставлено в наблюдении"
                )
            else:
                targets = {item for item in self._expression_items()
                           if self._item_expression(item) == expression}
                if targets:
                    self._remove_expression_items(targets)
        self._variable_browser.set_watched_expressions(self._expressions())

    def _expression_items(self) -> list[QTreeWidgetItem]:
        return [self.variables.topLevelItem(i)
                for i in range(self.variables.topLevelItemCount())]

    @staticmethod
    def _item_expression(item: QTreeWidgetItem) -> str:
        return item.data(0, Qt.UserRole)

    def _expressions(self) -> list[str]:
        return [self._item_expression(item) for item in self._expression_items()
                if self._item_expression(item) not in self._pulse_counters.definitions]

    def _create_expression(self, expression: str, *, history_enabled: bool = False,
                           style: dict[str, Any] | None = None,
                           setter: dict[str, str] | None = None) -> None:
        style = style or {}
        self.variables.blockSignals(True)
        try:
            description_text = str(style.get("description", ""))
            item = QTreeWidgetItem(self.variables, [
                description_text.strip() or expression, "", "—", "ожидание"])
            item.setData(0, Qt.UserRole, expression)
            item.setToolTip(0, expression)
            item.setFlags(item.flags() | Qt.ItemIsUserCheckable)
            item.setCheckState(1, Qt.Checked if history_enabled else Qt.Unchecked)
            for label in ("Предыдущее", "Min", "Max", "Среднее"):
                QTreeWidgetItem(item, [label, "", "—"])
            description_row = QTreeWidgetItem(item, ["Описание"])
            description = QLineEdit(description_text)
            description.setPlaceholderText("Название в таблице вместо выражения")
            self.variables.setItemWidget(description_row, 2, description)
            description.textChanged.connect(lambda text: item.setText(
                0, text.strip() or self._item_expression(item)))
            expression_row = QTreeWidgetItem(item, ["Lua-выражение"])
            expression_edit = QLineEdit(expression)
            expression_edit.setReadOnly(expression in self._pulse_counters.definitions)
            expression_edit.setToolTip("Применяется по Enter или при выходе из поля")
            self.variables.setItemWidget(expression_row, 2, expression_edit)
            expression_edit.editingFinished.connect(
                lambda: self._edit_expression(item, expression_edit))
            name_row = QTreeWidgetItem(item, ["Имя на графике"])
            name = QLineEdit(str(style.get("name", "")))
            name.setPlaceholderText(expression)
            self.variables.setItemWidget(name_row, 2, name)
            color_row = QTreeWidgetItem(item, ["Цвет линии"])
            color = QPushButton()
            initial_color = style.get("color") or pg.intColor(
                self.variables.topLevelItemCount() - 1).name()
            self._set_color_button(color, initial_color)
            self.variables.setItemWidget(color_row, 2, color)
            offset_row = QTreeWidgetItem(item, ["Сдвиг по Y"])
            offset = QDoubleSpinBox()
            offset.setRange(-1e12, 1e12)
            offset.setDecimals(6)
            offset.setValue(style.get("offset", 0.0))
            offset.setToolTip("На графике: исходное значение + сдвиг. История и статистика не меняются.")
            self.variables.setItemWidget(offset_row, 2, offset)
            points_row = QTreeWidgetItem(item, ["Точки на графике"])
            points = QCheckBox()
            points.setChecked(style.get("points", False))
            points.setToolTip("Показывать точки полученных изменений значения")
            self.variables.setItemWidget(points_row, 2, points)
            type_row = QTreeWidgetItem(item, ["Тип графика"])
            chart_type = QComboBox()
            for key, label in CHART_TYPES.items():
                chart_type.addItem(label, key)
            chart_type.setCurrentIndex(max(0, chart_type.findData(
                style.get("chart_type", DEFAULT_CHART_TYPE))))
            chart_type.setToolTip(
                "После точки: значение действует до следующего измерения.\n"
                "До точки: значение действует от предыдущего измерения.\n"
                "По середине: переход между значениями в середине интервала."
            )
            self.variables.setItemWidget(type_row, 2, chart_type)
            chart_type.currentIndexChanged.connect(self._redraw_chart)
            name.textChanged.connect(self._redraw_chart)
            color.clicked.connect(lambda: self._choose_color(color))
            offset.valueChanged.connect(self._redraw_chart)
            points.toggled.connect(self._redraw_chart)
            if expression not in self._pulse_counters.definitions:
                settings = setter or {}
                for label, key, placeholder in (
                    ("Setter", "code", "LINE1V1:set_value(<newvalue>)"),
                    ("Допустимые значения", "limits", "(0;1;2); пусто — без ограничения"),
                ):
                    row = QTreeWidgetItem(item, [label])
                    edit = QLineEdit(settings.get(key, ""))
                    edit.setPlaceholderText(placeholder)
                    self.variables.setItemWidget(row, 2, edit)
                row = QTreeWidgetItem(item, ["Новое значение"])
                edit = QLineEdit()
                edit.setPlaceholderText('Число, true/false или "строка"')
                self.variables.setItemWidget(row, 2, edit)
                button = QPushButton("Установить…")
                button.clicked.connect(lambda: self._set_watch_value(item))
                self.variables.setItemWidget(row, 3, button)
        finally:
            self.variables.blockSignals(False)

    def _edit_expression(self, item: QTreeWidgetItem, editor: QLineEdit) -> None:
        previous = self._item_expression(item)
        expression = editor.text().strip()
        if previous in self._pulse_counters.definitions or expression == previous:
            editor.setText(previous)
            return
        if (not expression or len(expression.encode("utf-8")) > MAX_REQUEST_PATH_BYTES
                or any(character in expression for character in ("\n", "\r", "\0"))):
            editor.setText(previous)
            self._show_error("Lua-выражение должно содержать от 1 до 1024 байт без переносов строк")
            return
        if expression in [self._item_expression(row) for row in self._expression_items()]:
            editor.setText(previous)
            self._show_error("Такое выражение уже есть в списке величин")
            return
        affected = {previous, expression}
        for name, definition in list(self._pulse_counters.definitions.items()):
            if previous in (definition.source, definition.dependent):
                self._pulse_counters.remove(name)
                self._pulse_counters.add(replace(
                    definition,
                    source=expression if definition.source == previous else definition.source,
                    dependent=expression if definition.dependent == previous else definition.dependent,
                ))
                affected.add(name)
        item.setData(0, Qt.UserRole, expression)
        item.setToolTip(0, expression)
        description = self.variables.itemWidget(item.child(4), 2).text().strip()
        item.setText(0, description or expression)
        self.variables.itemWidget(item.child(6), 2).setPlaceholderText(expression)
        editor.setText(expression)
        for row in self._expression_items():
            if self._item_expression(row) in affected:
                row.setText(2, "—")
                row.setToolTip(2, "—")
                row.setText(3, "ожидание")
                row.setToolTip(3, "ожидание")
                row.child(0).setText(2, "—")
        for key in affected:
            self._statistics.pop(key, None)
        if self._last_chart_data is not None:
            self._last_chart_data = {
                **self._last_chart_data,
                "series": [series for series in self._last_chart_data.get("series", [])
                           if series.get("expression") not in affected],
            }
        self._history_changed(None)
        self._refresh_table_statistics()
        self._apply_expressions()

    @staticmethod
    def _set_color_button(button: QPushButton, color: str) -> None:
        button.setProperty("lineColor", color)
        button.setText(color)
        button.setStyleSheet(f"QPushButton {{ border: 3px solid {color}; }}")

    def _choose_color(self, button: QPushButton) -> None:
        color = QColorDialog.getColor(pg.mkColor(button.property("lineColor")), self,
                                     "Цвет линии")
        if color.isValid():
            self._set_color_button(button, color.name())
            self._redraw_chart()

    def _series_styles(self) -> dict[str, dict[str, Any]]:
        styles = {}
        for item in self._expression_items():
            widget = lambda index: self.variables.itemWidget(item.child(index), 2)
            styles[self._item_expression(item)] = {
                "description": widget(4).text(),
                "name": widget(6).text(),
                "color": widget(7).property("lineColor"),
                "offset": widget(8).value(),
                "points": widget(9).isChecked(),
                "chart_type": widget(10).currentData(),
            }
        return styles

    def _setter_settings(self) -> dict[str, dict[str, str]]:
        return {
            self._item_expression(item): {
                "code": self.variables.itemWidget(item.child(11), 2).text(),
                "limits": self.variables.itemWidget(item.child(12), 2).text(),
            }
            for item in self._expression_items()
            if self._item_expression(item) not in self._pulse_counters.definitions
        }

    def _set_watch_value(self, item: QTreeWidgetItem) -> None:
        if not self._connected or self._lua_pending:
            self._show_error("Нет подключения или предыдущий Lua-запрос ещё выполняется")
            return
        expression = self._item_expression(item)
        settings = self._setter_settings()[expression]
        new_value = self.variables.itemWidget(item.child(13), 2).text()
        try:
            code = setter_code(expression, settings["code"], settings["limits"], new_value)
        except ValueError as exc:
            self._show_error(str(exc))
            return
        self._submit_lua(code, f"Getter: {expression}\n"
                         f"Setter: {settings['code']}\nНовое значение: {new_value}\n"
                         f"Допустимые значения: {settings['limits'] or 'без ограничения'}")

    def _submit_lua(self, code: str, description: str) -> None:
        if not self._connected or self._lua_pending:
            self._show_error("Нет подключения или предыдущий Lua-запрос ещё выполняется")
            return
        if not code.strip() or len(code.encode("utf-8")) > 16384:
            self._show_error("Lua-код должен содержать от 1 до 16384 байт")
            return
        generation = self._connection_generation
        answer = QMessageBox.question(
            self, "Выполнить Lua на контроллере",
            f"Контроллер: {self.host_edit.text()}:{self.port_spin.value()}\n\n{description}",
            QMessageBox.Yes | QMessageBox.No, QMessageBox.No,
        )
        if answer != QMessageBox.Yes:
            return
        if (not self._connected or generation != self._connection_generation
                or self._lua_pending or self._shutting_down):
            self._show_error("Подключение изменилось; подтвердите Lua-запрос заново")
            return
        self._lua_pending = True
        self.execute_requested.emit(code)

    def _open_lua_console(self) -> None:
        dialog = QDialog(self)
        dialog.setWindowTitle("Выполнить Lua на контроллере")
        dialog.resize(640, 360)
        layout = QVBoxLayout(dialog)
        editor = LuaConsoleEdit(self._variable_browser.completion_expressions)
        editor.setPlaceholderText("LINE1V1:set_value(1)\nreturn LINE1V1:get_value()")
        editor.setToolTip(
            "Tab — дополнить из загруженного дерева переменных.\n"
            "Для полей и методов сначала раскройте ветку объекта."
        )
        layout.addWidget(editor)
        button = QPushButton("Выполнить…")
        button.clicked.connect(lambda: self._submit_lua(
            editor.toPlainText(), editor.toPlainText()))
        layout.addWidget(button)
        output = QPlainTextEdit()
        output.setReadOnly(True)
        output.setMaximumBlockCount(200)
        layout.addWidget(output)

        def show_result(code: str, result: dict[str, Any]) -> None:
            output.appendPlainText(f">>> {code}\n" + json.dumps(result, ensure_ascii=False))

        self.lua_executed.connect(show_result)
        try:
            dialog.exec()
        finally:
            self.lua_executed.disconnect(show_result)
            dialog.deleteLater()

    @Slot(str, dict)
    def _on_executed(self, code: str, result: dict[str, Any]) -> None:
        self._lua_pending = False
        self._on_evaluated(code, result)
        self.lua_executed.emit(code, result)

    def _redraw_chart(self, _value: object = None) -> None:
        if self._last_chart_data is not None:
            self._draw_chart(self._last_chart_data)

    def _history_expressions(self) -> list[str]:
        return [self._item_expression(item) for item in self._expression_items()
                if item.checkState(1) == Qt.Checked]

    def _history_changed(self, item: QTreeWidgetItem | None, column: int = 1) -> None:
        if item is not None and (item.parent() is not None or column != 1):
            return
        self._last_chart_data = trim_chart_data(
            self._last_chart_data,
            self.history_limit_spin.value(),
            set(self._history_expressions()),
        )
        if self._last_chart_data is not None:
            self._draw_chart(self._last_chart_data)
        else:
            self.plot.clear()
        self._refresh_history_table()

    @Slot()
    def _apply_expressions(self) -> None:
        if self._connected:
            self.expressions_requested.emit(self._expressions())
        self._variable_browser.set_watched_expressions(self._expressions())

    @Slot()
    def _evaluate_once(self) -> None:
        expression = self.evaluate_edit.text().strip()
        if expression:
            self.evaluate_requested.emit(expression)

    @Slot(str, dict)
    def _on_evaluated(self, expression: str, result: dict[str, Any]) -> None:
        lines = [f">>> {expression}"]
        if result.get("ok"):
            value = result.get("value")
            if isinstance(value, (dict, list)):
                rendered = json.dumps(
                    value, ensure_ascii=False, indent=2
                )
            else:
                rendered = str(value)
            lines.append(f"{result.get('type')}: {rendered}")
        else:
            lines.append(f"Ошибка: {result.get('error', 'Ошибка')}")
        self.evaluate_result.appendPlainText("\n".join(lines))
        scrollbar = self.evaluate_result.verticalScrollBar()
        scrollbar.setValue(scrollbar.maximum())

    @Slot()
    def _execute_controller_command(self) -> None:
        if not self._connected or self._command_pending:
            return
        answer = QMessageBox.question(
            self,
            "Подтверждение команды",
            (
                f"Отправить команду на контроллер "
                f"{self.host_edit.text().strip()}:{self.port_spin.value()}?\n\n"
                f"{self.command_combo.currentText()}"
            ),
            QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No,
            QMessageBox.StandardButton.No,
        )
        if answer != QMessageBox.StandardButton.Yes:
            return
        command_id = int(self.command_combo.currentData())
        self._command_pending = True
        self._update_reload_controls()
        self.command_button.setEnabled(False)
        self.command_result.setText("Выполнение…")
        self.controller_command_requested.emit(command_id)

    @Slot()
    def _update_reload_controls(self) -> None:
        available = self._connected and not self._command_pending
        obj = self.reload_object_combo.currentData()
        self.reload_objects_button.setEnabled(available)
        self.reload_object_combo.setEnabled(available)
        self.reload_object_button.setEnabled(bool(available and obj and obj.get("idle")))

    @Slot(list)
    def _on_reload_objects(self, objects: list[dict[str, Any]]) -> None:
        if not self._connected:
            return
        previous = self.reload_object_combo.currentData() or {}
        self.reload_object_combo.blockSignals(True)
        self.reload_object_combo.clear()
        selected = 0
        for obj in objects:
            if not isinstance(obj.get("id"), int) or not 1 <= obj["id"] <= 999:
                continue
            state = "простой" if obj.get("idle") else "в работе"
            label = f"{obj.get('lua_name', 'OBJECT' + str(obj['id']))} · {obj.get('name', '')} · {state}"
            self.reload_object_combo.addItem(label, obj)
            if obj["id"] == previous.get("id"):
                selected = self.reload_object_combo.count() - 1
        self.reload_object_combo.setCurrentIndex(selected)
        self.reload_object_combo.blockSignals(False)
        self.reload_hint.setText("Только в простое" if objects else "Объектов нет")
        self._update_reload_controls()

    @Slot(str)
    def _on_reload_objects_error(self, message: str) -> None:
        self.reload_object_combo.clear()
        self.reload_hint.setText(message)
        self._update_reload_controls()

    @Slot()
    def _reload_selected_object(self) -> None:
        obj = self.reload_object_combo.currentData()
        if not self._connected or self._command_pending or not obj or not obj.get("idle"):
            return
        object_id = obj["id"]
        answer = QMessageBox.question(
            self, "Перезагрузка объекта",
            f"Перезагрузить {obj.get('lua_name', 'OBJECT' + str(object_id))} "
            f"({obj.get('name', '')}) на {self.host_edit.text().strip()}:{self.port_spin.value()}?",
            QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No,
            QMessageBox.StandardButton.No,
        )
        if answer != QMessageBox.StandardButton.Yes:
            return
        self._command_pending = True
        self.command_button.setEnabled(False)
        self._update_reload_controls()
        self.command_result.setText("Выполнение…")
        self.controller_command_requested.emit(1_030_000 + object_id)

    @Slot(int, dict)
    def _on_command_executed(
        self, command_id: int, result: dict[str, Any]
    ) -> None:
        self._command_pending = False
        self.command_button.setEnabled(self._connected)
        self._update_reload_controls()
        if 1_030_000 < command_id < 1_031_000:
            self.command_result.setText(f"Объект {command_id - 1_030_000} перезагружен")
            self.reload_objects_requested.emit()
        elif result.get("queued"):
            self.command_result.setText("Сохранение запланировано")
        else:
            self.command_result.setText(
                f"Команда {command_id} выполнена (код {result.get('result', 0)})"
            )

    @Slot(dict)
    def _on_chart_data(self, data: dict[str, Any]) -> None:
        if self._pulse_counters.definitions:
            data = {**data, "series": [*data.get("series", []),
                                     *self._pulse_counters.process(data)]}
        self._statistics = merge_statistics(self._statistics, data)
        self._last_chart_data = merge_chart_data(
            self._last_chart_data,
            data,
            self.history_limit_spin.value(),
            set(self._history_expressions()),
        )
        self._refresh_values()

    def _refresh_values(self) -> None:
        if self._last_chart_data is None:
            return
        by_expression = {
            item.get("expression"): item
            for item in self._last_chart_data.get("series", [])
        }
        for item in self._expression_items():
            samples = by_expression.get(self._item_expression(item), {}).get("samples", [])
            last = samples[-1] if samples else None
            previous = samples[-2] if len(samples) > 1 else None
            value_text = "—" if last is None else str(last.get("value"))
            item.setText(2, value_text)
            item.setToolTip(2, value_text)
            item.child(0).setText(2, "—" if previous is None else str(previous.get("value")))
            status = "—" if last is None else (
                "OK" if last.get("ok") else str(last.get("value", "ошибка")))
            item.setText(3, status)
            item.setToolTip(3, status)
        self._refresh_table_statistics()
        self._draw_chart(self._last_chart_data)
        self._refresh_history_table()

    @Slot(dict)
    def _on_messages(self, data: dict[str, Any]) -> None:
        priority_names = {
            0: "EMERG",
            1: "ALERT",
            2: "CRIT",
            3: "ERROR",
            4: "WARNING",
            5: "NOTICE",
            6: "INFO",
            7: "DEBUG",
        }
        dropped = int(data.get("dropped", 0) or 0)
        if dropped:
            self.status_label.setText(
                f"Пропущено сообщений отладчика: {dropped}"
            )
        for message in data.get("messages", []):
            if not isinstance(message, dict):
                continue
            row = self.messages_table.rowCount()
            self.messages_table.insertRow(row)
            timestamp = controller_timestamp_ms(
                data, int(message.get("time_ms", 0))
            )
            time_text = (
                datetime.fromtimestamp(timestamp / 1000).strftime(
                    "%Y-%m-%d %H:%M:%S.%f"
                )[:-3]
                if timestamp is not None
                else str(message.get("time_ms", ""))
            )
            priority = int(message.get("priority", 6))
            values = (
                time_text,
                str(message.get("source", "")),
                priority_names.get(priority, str(priority)),
                str(message.get("text", "")),
            )
            for column, value in enumerate(values):
                self.messages_table.setItem(
                    row, column, QTableWidgetItem(value)
                )
        while self.messages_table.rowCount() > 5_000:
            self.messages_table.removeRow(0)
        if data.get("messages"):
            self.messages_table.scrollToBottom()

    def _message_log(self) -> list[dict[str, str]]:
        keys = ("time", "source", "level", "text")

        def cell(row: int, column: int) -> str:
            item = self.messages_table.item(row, column)
            return item.text() if item is not None else ""

        return [
            {key: cell(row, column) for column, key in enumerate(keys)}
            for row in range(self.messages_table.rowCount())
        ]

    def _restore_message_log(self, message_log: list[dict[str, str]]) -> None:
        keys = ("time", "source", "level", "text")
        self.messages_table.setRowCount(0)
        for entry in message_log:
            row = self.messages_table.rowCount()
            self.messages_table.insertRow(row)
            for column, key in enumerate(keys):
                self.messages_table.setItem(
                    row, column, QTableWidgetItem(entry[key])
                )
        if message_log:
            self.messages_table.scrollToBottom()

    def _refresh_table_statistics(self) -> None:
        for item in self._expression_items():
            statistics = self._statistics.get(self._item_expression(item), {})
            count = int(statistics.get("_count", 0))
            average = (
                float(statistics["_sum"]) / count
                if count and "_sum" in statistics
                else None
            )
            values = (statistics.get("min"), statistics.get("max"), average)
            for index, value in enumerate(values, start=1):
                item.child(index).setText(2, self._format_stat(value))

    @staticmethod
    def _format_stat(value: float | None) -> str:
        return "—" if value is None else f"{value:.12g}"

    @Slot(int)
    def _history_limit_changed(self, limit: int) -> None:
        self._last_chart_data = trim_chart_data(
            self._last_chart_data, limit, set(self._history_expressions())
        )
        if self._last_chart_data is not None:
            self._refresh_table_statistics()
            self._draw_chart(self._last_chart_data)
            self._refresh_history_table()

    def _display_settings_changed(self, _value: int | bool) -> None:
        if self._last_chart_data is not None:
            self._refresh_table_statistics()
            self._draw_chart(self._last_chart_data)

    @Slot()
    def _clear_charts(self) -> None:
        self._last_chart_data = None
        self._statistics = {}
        self._pulse_counters.reset()
        self.plot.clear()
        self._refresh_history_table()
        self._refresh_table_statistics()
        if self._connected:
            self.clear_requested.emit()

    def _draw_chart(self, data: dict[str, Any]) -> None:
        self.plot.clear()
        server_time = int(data.get("server_time_ms", 0))
        prepared: list[tuple[dict[str, Any], list[dict[str, Any]]]] = []
        history_expressions = set(self._history_expressions())
        for series in data.get("series", []):
            if series.get("expression") not in history_expressions:
                continue
            numeric = [
                sample for sample in series.get("samples", [])
                if sample.get("ok") and sample.get("type") in {"number", "boolean"}
            ]
            if numeric:
                prepared.append((series, numeric))
        if not prepared:
            return

        base = int(prepared[0][1][0]["time_ms"])
        requested = self.timeline_combo.currentData()
        converter = (
            client_timestamp_ms if requested == "real" else controller_timestamp_ms
        )
        mode = requested if requested == "real" else "controller"
        if converter(data, base) is None:
            converter = controller_timestamp_ms
            mode = "controller"
            if converter(data, base) is None:
                converter = None
                mode = "counter"
        self._set_time_axis(mode)
        base_seconds = base / 1000
        max_x: float | None = None
        styles = self._series_styles()
        for series, numeric in prepared:
            expression = str(series.get("expression", ""))
            style = styles[expression]
            if converter is not None:
                timestamps = [
                    converter(data, int(sample["time_ms"]))
                    for sample in numeric
                ]
                x_values = [
                    int(timestamp) / 1000
                    for timestamp in timestamps
                    if timestamp is not None
                ]
            else:
                x_values = [
                    base_seconds
                    + ((int(sample["time_ms"]) - base) & 0xFFFFFFFF) / 1000
                    for sample in numeric
                ]
            y_values = [float(sample["value"]) + style["offset"] for sample in numeric]
            point_x, point_y = x_values.copy(), y_values.copy()
            current_time = converter(data, server_time) if converter else None
            current_x = (
                current_time / 1000
                if current_time is not None
                else base_seconds + ((server_time - base) & 0xFFFFFFFF) / 1000
            )
            if current_x > x_values[-1]:
                x_values.append(current_x)
                y_values.append(y_values[-1])
            max_x = x_values[-1] if max_x is None else max(max_x, x_values[-1])
            chart_type = style["chart_type"]
            step_mode = {"step_post": "right", "step_pre": "left"}.get(chart_type)
            if chart_type == "step_mid":
                # Center mode needs N+1 bin edges for N values. Keep actual
                # sample positions for markers, including the single-sample case.
                x_values = [x_values[0]] + [
                    left + (right - left) / 2
                    for left, right in zip(point_x, point_x[1:])
                ] + [x_values[-1]]
                y_values = point_y
                step_mode = "center"
            elif chart_type == "scatter":
                x_values, y_values = point_x, point_y
            self.plot.plot(
                x_values,
                y_values,
                name=style["name"].strip() or expression,
                pen=None if chart_type == "scatter" else pg.mkPen(style["color"], width=2),
                stepMode=step_mode,
                symbol="o" if chart_type == "scatter" else None,
                symbolSize=6,
                symbolBrush=style["color"],
                symbolPen=style["color"],
            )
            if style["points"] and chart_type != "scatter":
                self.plot.plot(point_x, point_y, pen=None, symbol="o", symbolSize=6,
                               symbolBrush=style["color"], symbolPen=style["color"])
        if self.auto_follow_check.isChecked() and max_x is not None:
            left = max_x - self.display_seconds_spin.value()
            self.plot.setXRange(left, max_x, padding=0)

    def _set_time_axis(self, mode: str) -> None:
        if mode == self._time_axis:
            return
        axis = (
            pg.AxisItem(orientation="bottom")
            if mode == "counter"
            else pg.DateAxisItem(orientation="bottom")
        )
        self.plot.setAxisItems({"bottom": axis})
        self.plot.setLabel(
            "bottom",
            "Реальное время" if mode == "real" else "Время контроллера",
            units="s" if mode == "counter" else None,
        )
        self._time_axis = mode

    def _refresh_history_table(self) -> None:
        self.history_model.set_chart_data(
            self._last_chart_data, self._history_expressions()
        )

    @Slot()
    def _export_history(self) -> None:
        if not self.history_model.rows:
            QMessageBox.information(
                self, "Lua debugger", "Нет накопленной истории для экспорта"
            )
            return
        path, _ = QFileDialog.getSaveFileName(
            self, "Экспорт истории", "debugger_history.xlsx", "Excel (*.xlsx)"
        )
        if not path:
            return
        if not path.lower().endswith(".xlsx"):
            path += ".xlsx"
        try:
            export_history_xlsx(
                path,
                self.history_model.rows,
                self.history_model.expressions,
            )
            self.status_label.setText(f"История экспортирована: {path}")
        except (OSError, ValueError, XlsxWriterException) as exc:
            self._show_error(str(exc))

    @Slot(str)
    def _show_error(self, message: str) -> None:
        self._command_pending = False
        self.command_button.setEnabled(self._connected)
        self._update_reload_controls()
        if self.command_result.text() == "Выполнение…":
            self.command_result.setText(f"Ошибка: {message}")
        self.status_label.setText(message)
        QMessageBox.warning(self, "Lua debugger", message)

    @Slot()
    def _choose_logging_directory(self) -> None:
        path = QFileDialog.getExistingDirectory(
            self, "Каталог журнала", self.logging_directory_edit.text()
        )
        if path:
            self.logging_directory_edit.setText(path)

    @Slot(bool)
    def _logging_toggled(self, enabled: bool) -> None:
        if not enabled:
            self._logging_timer.stop()
            return
        if not self.logging_directory_edit.text().strip():
            self._choose_logging_directory()
        directory_text = self.logging_directory_edit.text().strip()
        if not directory_text:
            self.logging_check.blockSignals(True)
            self.logging_check.setChecked(False)
            self.logging_check.blockSignals(False)
            return
        try:
            directory = Path(directory_text).expanduser().resolve()
            directory.mkdir(parents=True, exist_ok=True)
        except OSError as exc:
            self.logging_check.blockSignals(True)
            self.logging_check.setChecked(False)
            self.logging_check.blockSignals(False)
            self._show_error(f"Не удалось включить логирование: {exc}")
            return
        self.logging_directory_edit.setText(str(directory))
        self._logging_timer.start(self.logging_interval_spin.value() * 60_000)
        self.status_label.setText(
            f"Логирование включено: {directory}"
        )

    @Slot(int)
    def _logging_interval_changed(self, minutes: int) -> None:
        if self._logging_timer.isActive():
            self._logging_timer.start(minutes * 60_000)

    def _save_session_to(self, path: str | Path) -> None:
        save_session(
            path,
            host=self.host_edit.text().strip(),
            port=self.port_spin.value(),
            poll_interval_ms=self.interval_spin.value(),
            history_limit=self.history_limit_spin.value(),
            expressions=self._expressions(),
            history_expressions=self._history_expressions(),
            chart_data=self._last_chart_data,
            display_seconds=self.display_seconds_spin.value(),
            auto_follow=self.auto_follow_check.isChecked(),
            auto_reconnect=self.auto_reconnect_check.isChecked(),
            timeline=self.timeline_combo.currentData(),
            statistics=self._statistics,
            series_styles=self._series_styles(),
            pulse_definitions=[
                vars(definition)
                for definition in self._pulse_counters.definitions.values()
            ],
            pulse_state=self._pulse_counters.snapshot(),
            message_log=self._message_log(),
            variable_browser=self._variable_browser.snapshot_state(),
            setter_settings=self._setter_settings(),
        )

    def _next_log_path(self) -> Path:
        directory = Path(self.logging_directory_edit.text().strip())
        directory.mkdir(parents=True, exist_ok=True)
        host = re.sub(r"[^\w.-]+", "_", self.host_edit.text().strip()).strip("._")
        host = host or "session"
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        stem = f"session_{host}_{self.port_spin.value()}_{timestamp}"
        path = directory / f"{stem}.ptlua.json"
        suffix = 1
        while path.exists():
            path = directory / f"{stem}_{suffix:03d}.ptlua.json"
            suffix += 1
        return path

    @Slot()
    def _rotate_log(self) -> None:
        try:
            path = self._next_log_path()
            self._save_session_to(path)
        except OSError as exc:
            self._logging_timer.stop()
            self.logging_check.blockSignals(True)
            self.logging_check.setChecked(False)
            self.logging_check.blockSignals(False)
            self._show_error(f"Логирование остановлено: {exc}")
            return
        self._clear_charts()
        self.status_label.setText(f"Сессия журнала сохранена: {path}")

    @Slot()
    def _save_session(self) -> None:
        path, _ = QFileDialog.getSaveFileName(
            self, "Сохранить сессию", "", "Lua debugger session (*.ptlua.json)"
        )
        if not path:
            return
        try:
            self._save_session_to(path)
        except OSError as exc:
            self._show_error(str(exc))
            return
        self.status_label.setText(f"Сессия сохранена: {path}")

    @Slot()
    def _load_session(self) -> None:
        path, _ = QFileDialog.getOpenFileName(
            self, "Загрузить сессию", "", "Lua debugger session (*.ptlua.json)"
        )
        if not path:
            return
        try:
            self.load_document(load_session(path))
        except (OSError, ValueError, TypeError) as exc:
            self._show_error(str(exc))

    def load_document(self, document: dict[str, Any]) -> None:
        connection = document["connection"]
        self.host_edit.setText(str(connection.get("host", "127.0.0.1")))
        self.port_spin.setValue(int(connection.get("port", 10_000)))
        self.interval_spin.setValue(int(document["poll_interval_ms"]))
        self.history_limit_spin.setValue(int(document["history_limit"]))
        self.display_seconds_spin.setValue(int(document["display_seconds"]))
        self.auto_follow_check.setChecked(bool(document["auto_follow"]))
        self.auto_reconnect_check.setChecked(
            bool(document.get("auto_reconnect", False))
        )
        self._restore_message_log(document.get("message_log", []))
        self._statistics = {
            expression: dict(extrema)
            for expression, extrema in document["statistics"].items()
        }
        history_expressions = set(document["history_expressions"])
        self._pulse_counters = PulseCounters()
        for values in document.get("pulse_definitions", []):
            self._pulse_counters.add(PulseDefinition(**values))
        self.variables.clear()
        for expression in [*document["expressions"], *self._pulse_counters.definitions]:
            self._create_expression(
                expression, history_enabled=expression in history_expressions,
                style=document.get("series_styles", {}).get(expression),
                setter=document.get("setter_settings", {}).get(expression),
            )
        chart_data = document.get("chart_data")
        self._last_chart_data = None
        timeline_index = self.timeline_combo.findData(document["timeline"])
        if timeline_index >= 0:
            self.timeline_combo.setCurrentIndex(timeline_index)
        if isinstance(chart_data, dict):
            self._last_chart_data = chart_data
            self._pulse_counters.restore(
                document.get("pulse_state", {}),
                (chart_data.get("controller_time_unix_ms"),
                 chart_data.get("controller_time_millisec")),
            )
            self._refresh_values()
        else:
            self.plot.clear()
            self._refresh_history_table()
            self._refresh_table_statistics()
        self._variable_browser.restore_state(
            document.get("variable_browser")
        )
        self._apply_expressions()
        self._update_title()

    def _update_title(self, _value: object = None) -> None:
        marker = "● " if self._connected else ""
        host = self.host_edit.text().strip() or "новая сессия"
        self.title_changed.emit(f"{marker}{host}:{self.port_spin.value()}")

    def shutdown(self) -> None:
        if self._shutting_down:
            return
        self._shutting_down = True
        self._logging_timer.stop()
        self._reconnect_timer.stop()
        QMetaObject.invokeMethod(
            self._worker, "shutdown", Qt.BlockingQueuedConnection
        )
        self._thread.quit()
        self._thread.wait(3_000)

    def closeEvent(self, event: QCloseEvent) -> None:
        self.shutdown()
        event.accept()


class MainWindow(QMainWindow):
    def __init__(self) -> None:
        super().__init__()
        self.setWindowTitle("ptusa Lua debugger")
        self.resize(1180, 760)

        self.sessions = QTabWidget()
        self.sessions.setDocumentMode(True)
        self.sessions.setMovable(True)
        self.sessions.setTabsClosable(True)
        self.sessions.tabCloseRequested.connect(self._close_session)
        self.setCentralWidget(self.sessions)

        add_button = QToolButton()
        add_button.setText("+")
        add_button.setToolTip("Новая сессия (Ctrl+T)")
        add_button.clicked.connect(self._add_session)
        self.sessions.setCornerWidget(add_button, Qt.TopRightCorner)

        session_menu = self.menuBar().addMenu("Сессия")
        new_action = QAction("Новая вкладка", self)
        new_action.setShortcut(QKeySequence.StandardKey.AddTab)
        new_action.triggered.connect(self._add_session)
        open_action = QAction("Открыть в новой вкладке…", self)
        open_action.setShortcut(QKeySequence.StandardKey.Open)
        open_action.triggered.connect(self._open_session)
        save_action = QAction("Сохранить текущую…", self)
        save_action.setShortcut(QKeySequence.StandardKey.Save)
        save_action.triggered.connect(self._save_current_session)
        close_action = QAction("Закрыть вкладку", self)
        close_action.setShortcut(QKeySequence.StandardKey.Close)
        close_action.triggered.connect(self._close_current_session)
        session_menu.addActions(
            [new_action, open_action, save_action, close_action]
        )

        self._add_session()

    @Slot()
    def _add_session(self) -> DebuggerSessionWidget:
        session = DebuggerSessionWidget()
        index = self.sessions.addTab(session, self._session_title(session))
        session.title_changed.connect(
            lambda title, current=session: self._set_session_title(current, title)
        )
        self.sessions.setCurrentIndex(index)
        return session

    @staticmethod
    def _session_title(session: DebuggerSessionWidget) -> str:
        host = session.host_edit.text().strip() or "новая сессия"
        return f"{host}:{session.port_spin.value()}"

    def _set_session_title(
        self, session: DebuggerSessionWidget, title: str
    ) -> None:
        index = self.sessions.indexOf(session)
        if index >= 0:
            self.sessions.setTabText(index, title)

    @Slot()
    def _open_session(self) -> None:
        path, _ = QFileDialog.getOpenFileName(
            self, "Открыть сессию", "", "Lua debugger session (*.ptlua.json)"
        )
        if not path:
            return
        try:
            document = load_session(path)
        except (OSError, ValueError, TypeError) as exc:
            QMessageBox.warning(self, "Lua debugger", str(exc))
            return
        self._add_session().load_document(document)

    @Slot()
    def _save_current_session(self) -> None:
        session = self.sessions.currentWidget()
        if isinstance(session, DebuggerSessionWidget):
            session._save_session()

    @Slot()
    def _close_current_session(self) -> None:
        index = self.sessions.currentIndex()
        if index >= 0:
            self._close_session(index)

    @Slot(int)
    def _close_session(self, index: int) -> None:
        session = self.sessions.widget(index)
        if not isinstance(session, DebuggerSessionWidget):
            return
        self.sessions.removeTab(index)
        session.shutdown()
        session.deleteLater()
        if self.sessions.count() == 0:
            self._add_session()

    def closeEvent(self, event: QCloseEvent) -> None:
        for index in range(self.sessions.count()):
            session = self.sessions.widget(index)
            if isinstance(session, DebuggerSessionWidget):
                session.shutdown()
        event.accept()
