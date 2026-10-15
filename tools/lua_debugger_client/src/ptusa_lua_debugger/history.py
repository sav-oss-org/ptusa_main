from __future__ import annotations

from dataclasses import dataclass
from typing import Any

DEFAULT_HISTORY_LIMIT = 5_000
MAX_HISTORY_LIMIT = 1_000_000
DEFAULT_DISPLAY_SECONDS = 60
MAX_DISPLAY_SECONDS = 86_400
TIME_ANCHOR_FIELDS = (
    "controller_time_unix_ms",
    "controller_time_millisec",
    "client_time_unix_ms",
    "client_time_millisec",
)


@dataclass
class HistoryRow:
    controller_time_ms: int | None
    real_time_ms: int | None
    samples: dict[str, dict[str, Any]]


def controller_timestamp_ms(data: dict[str, Any], millisec: int) -> int | None:
    """Convert a wrapping PAC millisecond counter to controller Unix time."""
    return _anchored_timestamp_ms(
        data, millisec, "controller_time_unix_ms", "controller_time_millisec"
    )


def client_timestamp_ms(data: dict[str, Any], millisec: int) -> int | None:
    """Convert a wrapping PAC millisecond counter to debugger-computer Unix time."""
    return _anchored_timestamp_ms(
        data, millisec, "client_time_unix_ms", "client_time_millisec"
    )


def _anchored_timestamp_ms(
    data: dict[str, Any], millisec: int, unix_field: str, anchor_field: str
) -> int | None:
    unix_ms = data.get(unix_field)
    anchor_millisec = data.get(anchor_field)
    if (
        not isinstance(unix_ms, int)
        or isinstance(unix_ms, bool)
        or not isinstance(anchor_millisec, int)
        or isinstance(anchor_millisec, bool)
    ):
        return None
    elapsed_ms = (int(millisec) - anchor_millisec) & 0xFFFFFFFF
    return unix_ms + elapsed_ms


def _overlap_size(
    previous: list[dict[str, Any]], current: list[dict[str, Any]]
) -> int:
    """Return the largest previous suffix matching the current prefix."""
    for size in range(min(len(previous), len(current)), 0, -1):
        if previous[-size:] == current[:size]:
            return size
    return 0


def merge_chart_data(
    previous: dict[str, Any] | None,
    current: dict[str, Any],
    limit: int,
    history_expressions: set[str] | None = None,
) -> dict[str, Any]:
    """Merge the server's rolling cache into the longer client-side history."""
    limit = max(1, min(int(limit), MAX_HISTORY_LIMIT))
    if previous is not None and any(
        previous.get(field) != current.get(field) for field in TIME_ANCHOR_FIELDS
    ):
        previous = None
    previous_by_expression = {
        item.get("expression"): item
        for item in (previous or {}).get("series", [])
        if isinstance(item, dict)
    }
    merged_series: list[dict[str, Any]] = []

    for item in current.get("series", []):
        if not isinstance(item, dict):
            continue
        expression = item.get("expression")
        if not isinstance(expression, str):
            continue
        old_item = previous_by_expression.get(expression, {})
        old_samples = list(old_item.get("samples", []))
        new_samples = list(item.get("samples", []))
        overlap = _overlap_size(old_samples, new_samples)
        expression_limit = (
            limit
            if history_expressions is None or expression in history_expressions
            else 2
        )
        samples = (old_samples + new_samples[overlap:])[-expression_limit:]
        merged_series.append({"expression": expression, "samples": samples})

    result = {
        "ok": current.get("ok", True),
        "server_time_ms": current.get("server_time_ms", 0),
        "series": merged_series,
    }
    for field in TIME_ANCHOR_FIELDS:
        if field in current:
            result[field] = current[field]
    return result


def trim_chart_data(
    data: dict[str, Any] | None,
    limit: int,
    history_expressions: set[str] | None = None,
) -> dict[str, Any] | None:
    if data is None:
        return None
    limit = max(1, min(int(limit), MAX_HISTORY_LIMIT))
    trimmed_series: list[dict[str, Any]] = []
    for item in data.get("series", []):
        if not isinstance(item, dict):
            continue
        expression_limit = (
            limit
            if history_expressions is None
            or item.get("expression") in history_expressions
            else 2
        )
        trimmed_series.append(
            {**item, "samples": list(item.get("samples", []))[-expression_limit:]}
        )
    return {
        **data,
        "series": trimmed_series,
    }


def build_history_rows(
    data: dict[str, Any] | None, expressions: list[str]
) -> tuple[list[HistoryRow], bool]:
    """Build a sparse event table with one column per selected expression."""
    if not data or not expressions:
        return [], False

    selected = set(expressions)
    series = [
        item
        for item in data.get("series", [])
        if isinstance(item, dict) and item.get("expression") in selected
    ]
    first_sample = next(
        (
            sample
            for item in series
            for sample in item.get("samples", [])
            if isinstance(sample, dict) and "time_ms" in sample
        ),
        None,
    )
    if first_sample is None:
        return [], False

    base_millisec = int(first_sample["time_ms"])
    absolute_time = controller_timestamp_ms(data, base_millisec) is not None
    rows_by_time: dict[int, HistoryRow] = {}
    for item in series:
        expression = str(item["expression"])
        for sample in item.get("samples", []):
            if not isinstance(sample, dict) or "time_ms" not in sample:
                continue
            millisec = int(sample["time_ms"])
            elapsed = (millisec - base_millisec) & 0xFFFFFFFF
            row = rows_by_time.setdefault(
                elapsed,
                HistoryRow(
                    controller_timestamp_ms(data, millisec),
                    client_timestamp_ms(data, millisec),
                    {},
                ),
            )
            row.samples[expression] = sample

    return [rows_by_time[key] for key in sorted(rows_by_time)], absolute_time


def merge_statistics(
    previous: dict[str, dict[str, Any]], data: dict[str, Any]
) -> dict[str, dict[str, Any]]:
    """Accumulate exact numeric statistics without recounting server cache data."""
    kept_fields = {"min", "max", "_sum", "_count", "_last_sample"}
    result = {
        expression: {
            key: value for key, value in statistics.items() if key in kept_fields
        }
        for expression, statistics in previous.items()
    }
    for series in data.get("series", []):
        if not isinstance(series, dict):
            continue
        expression = series.get("expression")
        if not isinstance(expression, str):
            continue
        samples = [
            sample
            for sample in series.get("samples", [])
            if isinstance(sample, dict)
        ]
        if not samples:
            continue

        accumulated = result.get(expression, {})
        last_sample = accumulated.get("_last_sample")
        start = 0
        if isinstance(last_sample, dict):
            for index in range(len(samples) - 1, -1, -1):
                if samples[index] == last_sample:
                    start = index + 1
                    break
        new_samples = samples[start:]
        values = [
            float(sample["value"])
            for sample in new_samples
            if sample.get("ok") and sample.get("type") in {"number", "boolean"}
        ]
        if not values and not accumulated:
            continue

        total = float(accumulated.get("_sum", 0.0))
        count = int(accumulated.get("_count", 0))
        minimum = (
            float(accumulated["min"]) if "min" in accumulated else None
        )
        maximum = (
            float(accumulated["max"]) if "max" in accumulated else None
        )
        for value in values:
            total += value
            count += 1
            minimum = value if minimum is None else min(minimum, value)
            maximum = value if maximum is None else max(maximum, value)

        if not count or minimum is None or maximum is None:
            continue
        result[expression] = {
            "min": minimum,
            "max": maximum,
            "_sum": total,
            "_count": count,
            "_last_sample": samples[-1],
        }
    return result
