from ptusa_lua_debugger.history import (
    build_history_rows,
    client_timestamp_ms,
    controller_timestamp_ms,
    merge_chart_data,
    merge_statistics,
    trim_chart_data,
)


def chart(*values: int) -> dict:
    return {
        "ok": True,
        "server_time_ms": values[-1] if values else 0,
        "controller_time_unix_ms": 1_789_123_456_000,
        "controller_time_millisec": 1_000,
        "client_time_unix_ms": 1_700_000_000_000,
        "client_time_millisec": 1_000,
        "series": [
            {
                "expression": "x",
                "samples": [
                    {"time_ms": value, "ok": True, "type": "number", "value": value}
                    for value in values
                ],
            }
        ],
    }


def test_merge_rolling_server_cache_without_duplicates() -> None:
    result = merge_chart_data(chart(1, 2, 3), chart(2, 3, 4), 5_000)

    assert [sample["value"] for sample in result["series"][0]["samples"]] == [
        1,
        2,
        3,
        4,
    ]


def test_history_is_limited_per_expression() -> None:
    result = merge_chart_data(chart(1, 2, 3), chart(3, 4, 5), 3)
    assert [sample["value"] for sample in result["series"][0]["samples"]] == [3, 4, 5]

    trimmed = trim_chart_data(result, 2)
    assert [sample["value"] for sample in trimmed["series"][0]["samples"]] == [4, 5]


def test_statistics_are_kept_after_chart_history_is_trimmed() -> None:
    statistics = merge_statistics({}, chart(1, 9))
    history = merge_chart_data(None, chart(1, 9), 1)
    statistics = merge_statistics(statistics, chart(4, 5))

    assert [sample["value"] for sample in history["series"][0]["samples"]] == [9]
    assert statistics["x"]["min"] == 1.0
    assert statistics["x"]["max"] == 9.0
    assert statistics["x"]["_sum"] / statistics["x"]["_count"] == 4.75


def test_statistics_do_not_recount_overlapping_server_cache() -> None:
    statistics = merge_statistics({}, chart(1, 3, 5))
    statistics = merge_statistics(statistics, chart(3, 5, 9))
    statistics = merge_statistics(statistics, chart(3, 5, 9))

    assert statistics["x"]["_count"] == 4
    assert statistics["x"]["_sum"] / statistics["x"]["_count"] == 4.5
    assert set(statistics["x"]) == {
        "min",
        "max",
        "_sum",
        "_count",
        "_last_sample",
    }
    assert "average" not in statistics["x"]
    assert "median" not in statistics["x"]
    assert "_values" not in statistics["x"]


def test_controller_timestamp_uses_session_anchor_across_counter_wrap() -> None:
    data = {
        "controller_time_unix_ms": 1_789_123_456_000,
        "controller_time_millisec": 0xFFFFFFF0,
    }

    assert controller_timestamp_ms(data, 0x00000010) == 1_789_123_456_032


def test_new_session_anchor_discards_previous_session_samples() -> None:
    previous = chart(1, 2)
    current = chart(3)
    current["controller_time_unix_ms"] += 10_000

    result = merge_chart_data(previous, current, 5_000)

    assert [sample["value"] for sample in result["series"][0]["samples"]] == [3]


def test_history_is_kept_only_for_selected_expressions() -> None:
    previous = chart(1, 2, 3)
    current = chart(2, 3, 4)

    result = merge_chart_data(previous, current, 5_000, set())

    assert [sample["value"] for sample in result["series"][0]["samples"]] == [3, 4]


def test_builds_sparse_summary_history_table() -> None:
    data = chart(1, 3)
    data["series"].append(
        {
            "expression": "y",
            "samples": [
                {"time_ms": 2, "ok": True, "type": "number", "value": 20},
                {"time_ms": 3, "ok": True, "type": "number", "value": 30},
            ],
        }
    )

    rows, absolute_time = build_history_rows(data, ["x", "y"])

    assert absolute_time is True
    assert [row.controller_time_ms for row in rows] == [
        controller_timestamp_ms(data, millisec) for millisec in (1, 2, 3)
    ]
    assert [row.real_time_ms for row in rows] == [
        client_timestamp_ms(data, millisec) for millisec in (1, 2, 3)
    ]
    assert [set(row.samples) for row in rows] == [{"x"}, {"y"}, {"x", "y"}]


def test_history_rows_stay_ordered_across_counter_wrap() -> None:
    data = {
        "controller_time_unix_ms": 1_789_123_456_000,
        "controller_time_millisec": 0xFFFFFFF0,
        "client_time_unix_ms": 1_700_000_000_000,
        "client_time_millisec": 0xFFFFFFF0,
        "series": [
            {
                "expression": "x",
                "samples": [
                    {
                        "time_ms": 0xFFFFFFFE,
                        "ok": True,
                        "type": "number",
                        "value": 1,
                    },
                    {
                        "time_ms": 0x00000002,
                        "ok": True,
                        "type": "number",
                        "value": 2,
                    },
                ],
            }
        ],
    }

    rows, absolute_time = build_history_rows(data, ["x"])

    assert absolute_time is True
    assert rows[0].controller_time_ms == controller_timestamp_ms(data, 0xFFFFFFFE)
    assert rows[0].real_time_ms == client_timestamp_ms(data, 0xFFFFFFFE)
    assert rows[1].controller_time_ms - rows[0].controller_time_ms == 4
    assert rows[1].real_time_ms - rows[0].real_time_ms == 4
