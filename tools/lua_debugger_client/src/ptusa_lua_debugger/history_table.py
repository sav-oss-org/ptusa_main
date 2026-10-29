from __future__ import annotations

from datetime import datetime
from typing import Any

from PySide6.QtCore import QAbstractTableModel, QModelIndex, Qt

from .history import HistoryRow, build_history_rows


class HistoryTableModel(QAbstractTableModel):
    def __init__(self) -> None:
        super().__init__()
        self.expressions: list[str] = []
        self.rows: list[HistoryRow] = []
        self.absolute_time = False

    def set_chart_data(
        self, data: dict[str, Any] | None, expressions: list[str]
    ) -> None:
        rows, absolute_time = build_history_rows(data, expressions)
        self.beginResetModel()
        self.expressions = list(expressions)
        self.rows = rows
        self.absolute_time = absolute_time
        self.endResetModel()

    def rowCount(self, _parent: QModelIndex = QModelIndex()) -> int:
        return len(self.rows)

    def columnCount(self, _parent: QModelIndex = QModelIndex()) -> int:
        return 2 + len(self.expressions)

    def data(self, index: QModelIndex, role: int = Qt.DisplayRole) -> Any:
        if not index.isValid() or role not in {Qt.DisplayRole, Qt.TextAlignmentRole}:
            return None
        if role == Qt.TextAlignmentRole:
            return int(Qt.AlignRight | Qt.AlignVCenter)

        row = self.rows[index.row()]
        if index.column() < 2:
            timestamp = (
                row.real_time_ms
                if index.column() == 0
                else row.controller_time_ms
            )
            if timestamp is None:
                return "—"
            return datetime.fromtimestamp(timestamp / 1000).strftime(
                "%Y-%m-%d %H:%M:%S.%f"
            )[:-3]

        sample = row.samples.get(self.expressions[index.column() - 2])
        if sample is None:
            return None
        return str(sample.get("value", ""))

    def headerData(
        self, section: int, orientation: Qt.Orientation, role: int = Qt.DisplayRole
    ) -> Any:
        if role != Qt.DisplayRole:
            return None
        if orientation == Qt.Horizontal:
            if section == 0:
                return "Реальное время"
            if section == 1:
                return "Время контроллера"
            return self.expressions[section - 2]
        return section + 1
