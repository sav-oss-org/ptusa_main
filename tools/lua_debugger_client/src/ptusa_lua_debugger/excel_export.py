from __future__ import annotations

from datetime import datetime
from pathlib import Path

import xlsxwriter

from .history import HistoryRow

MAX_EXCEL_DATA_ROWS = 1_048_575


def export_history_xlsx(
    path: str | Path,
    rows: list[HistoryRow],
    expressions: list[str],
) -> None:
    if len(rows) > MAX_EXCEL_DATA_ROWS:
        raise ValueError(
            "Сводная история превышает лимит Excel в 1 048 575 строк данных"
        )
    workbook = xlsxwriter.Workbook(str(path), {"constant_memory": True})
    try:
        worksheet = workbook.add_worksheet("История")
        header = workbook.add_format(
            {
                "bold": True,
                "font_color": "#FFFFFF",
                "bg_color": "#263238",
                "align": "center",
                "valign": "vcenter",
                "border": 1,
                "border_color": "#546E7A",
            }
        )
        date_format = workbook.add_format({"num_format": "yyyy-mm-dd hh:mm:ss.000"})

        for column, label in enumerate(
            ["Реальное время", "Время контроллера", *expressions]
        ):
            worksheet.write_string(0, column, label, header)
        worksheet.freeze_panes(1, 2)
        worksheet.set_column(0, 0, 24)
        worksheet.set_column(1, 1, 24)
        for column, expression in enumerate(expressions, 2):
            worksheet.set_column(column, column, min(max(len(expression) + 2, 12), 48))

        for row_index, row in enumerate(rows, 1):
            for column, timestamp in enumerate(
                (row.real_time_ms, row.controller_time_ms)
            ):
                if timestamp is not None:
                    worksheet.write_datetime(
                        row_index,
                        column,
                        datetime.fromtimestamp(timestamp / 1000),
                        date_format,
                    )
            for column, expression in enumerate(expressions, 2):
                sample = row.samples.get(expression)
                if sample is None:
                    continue
                value = sample.get("value")
                if isinstance(value, bool):
                    worksheet.write_boolean(row_index, column, value)
                elif isinstance(value, (int, float)):
                    worksheet.write_number(row_index, column, value)
                elif value is not None:
                    worksheet.write_string(row_index, column, str(value))

        if expressions:
            worksheet.autofilter(0, 0, max(len(rows), 1), len(expressions) + 1)
    finally:
        workbook.close()
