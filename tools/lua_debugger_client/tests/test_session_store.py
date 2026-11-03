import json

from ptusa_lua_debugger.session_store import load_session, save_session
import pytest


def test_pending_burst_roundtrip(tmp_path):
    path = tmp_path / "pending.json"
    expression = "[Импульсы] Левый"
    state = {"count": 12, "values": {}, "last_samples": {}}
    save_session(path, host="localhost", port=10000, poll_interval_ms=500,
                 history_limit=5000, expressions=["left", "right"],
                 history_expressions=[expression], chart_data=None,
                 pulse_definitions=[{"name": "Левый", "source": "left",
                                     "dependent": "right", "source_value": 1,
                                     "dependent_value": 1}],
                 pulse_state={expression: state})
    assert load_session(path)["pulse_state"][expression]["count"] == 12


@pytest.mark.parametrize("invalid_type", ["spline", None, [], 42])
def test_invalid_chart_type_is_rejected(tmp_path, invalid_type) -> None:
    path = tmp_path / "invalid.json"
    save_session(path, host="localhost", port=10000, poll_interval_ms=500,
                 history_limit=5000, expressions=["x"], history_expressions=["x"],
                 chart_data=None, series_styles={"x": {"chart_type": invalid_type}})
    with pytest.raises(ValueError, match="настройки линий"):
        load_session(path)


@pytest.mark.parametrize("description", [None, 42, [], {}])
def test_invalid_expression_description_is_rejected(tmp_path, description):
    path = tmp_path / "invalid-description.ptlua.json"
    save_session(path, series_styles={"x": {"description": description}},
                 **_base_session_kwargs())
    with pytest.raises(ValueError, match="настройки линий"):
        load_session(path)


def test_expression_description_roundtrip_and_old_session(tmp_path):
    path = tmp_path / "description.ptlua.json"
    save_session(path, series_styles={"x": {"description": "Температура продукта"}},
                 **_base_session_kwargs())
    assert load_session(path)["series_styles"]["x"]["description"] == "Температура продукта"
    save_session(path, series_styles={"x": {"name": "График"}},
                 **_base_session_kwargs())
    assert load_session(path)["series_styles"]["x"] == {"name": "График"}


def test_session_roundtrip(tmp_path) -> None:
    path = tmp_path / "line.ptlua.json"
    save_session(
        path,
        host="192.168.1.10",
        port=10_000,
        poll_interval_ms=250,
        history_limit=7_500,
        expressions=["TE1:get_value()"],
        history_expressions=["TE1:get_value()"],
        chart_data={"ok": True, "series": []},
        display_seconds=120,
        auto_follow=False,
        auto_reconnect=True,
        message_log=[
            {
                "time": "2026-06-25 12:00:00.500",
                "source": "set_err_msg",
                "level": "ERROR",
                "text": "Тестовая авария",
            }
        ],
        statistics={
            "TE1:get_value()": {
                "min": 1.5,
                "max": 8.0,
                "_sum": 9.5,
                "_count": 2,
                "_last_sample": {
                    "time_ms": 2,
                    "ok": True,
                    "type": "number",
                    "value": 8.0,
                },
            }
        },
    )

    document = load_session(path)
    assert document["connection"] == {"host": "192.168.1.10", "port": 10_000}
    assert document["poll_interval_ms"] == 250
    assert document["history_limit"] == 7_500
    assert document["display_seconds"] == 120
    assert document["auto_follow"] is False
    assert document["auto_reconnect"] is True
    assert document["message_log"] == [
        {
            "time": "2026-06-25 12:00:00.500",
            "source": "set_err_msg",
            "level": "ERROR",
            "text": "Тестовая авария",
        }
    ]
    assert document["statistics"]["TE1:get_value()"] == {
        "min": 1.5,
        "max": 8.0,
        "_sum": 9.5,
        "_count": 2,
        "_last_sample": {
            "time_ms": 2,
            "ok": True,
            "type": "number",
            "value": 8.0,
        },
    }
    assert document["expressions"] == ["TE1:get_value()"]
    assert document["history_expressions"] == ["TE1:get_value()"]


def test_old_session_uses_default_history_limit(tmp_path) -> None:
    path = tmp_path / "old.ptlua.json"
    path.write_text(
        '{"version":1,"connection":{},"poll_interval_ms":500,"expressions":[]}',
        encoding="utf-8",
    )

    document = load_session(path)
    assert document["history_limit"] == 5_000
    assert document["display_seconds"] == 60
    assert document["auto_follow"] is True
    assert document["auto_reconnect"] is False
    assert document["message_log"] == []
    assert document["statistics"] == {}
    assert document["history_expressions"] == []
    assert document["timeline"] == "controller"


@pytest.mark.parametrize("value", ["yes", 1, None, [], {}])
def test_invalid_auto_reconnect_is_rejected(tmp_path, value) -> None:
    path = tmp_path / "bad-auto-reconnect.ptlua.json"
    path.write_text(
        json.dumps(
            {
                "version": 1,
                "connection": {},
                "poll_interval_ms": 500,
                "expressions": [],
                "auto_reconnect": value,
            }
        ),
        encoding="utf-8",
    )

    with pytest.raises(TypeError, match="переподключения"):
        load_session(path)


def test_auto_reconnect_defaults_to_false_when_not_saved(tmp_path) -> None:
    path = tmp_path / "default-auto-reconnect.ptlua.json"
    save_session(
        path,
        host="localhost",
        port=10_000,
        poll_interval_ms=500,
        history_limit=5_000,
        expressions=[],
        history_expressions=[],
        chart_data=None,
    )

    assert load_session(path)["auto_reconnect"] is False


@pytest.mark.parametrize("message_log", [
    "not-a-list",
    ["not-a-dict"],
    [{"time": "t", "source": "s", "level": "l"}],
    [{"time": "t", "source": "s", "level": "l", "text": "x", "id": "1"}],
    [{"time": "t", "source": "s", "level": "l", "text": 42}],
])
def test_invalid_message_log_is_rejected(tmp_path, message_log) -> None:
    path = tmp_path / "bad-message-log.ptlua.json"
    path.write_text(
        json.dumps(
            {
                "version": 1,
                "connection": {},
                "poll_interval_ms": 500,
                "expressions": [],
                "message_log": message_log,
            }
        ),
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="журнал сообщений"):
        load_session(path)


@pytest.mark.parametrize("timeline", ["real", "controller"])
def test_timeline_roundtrip(tmp_path, timeline) -> None:
    path = tmp_path / "timeline.ptlua.json"
    save_session(
        path,
        host="localhost",
        port=10_000,
        poll_interval_ms=500,
        history_limit=5_000,
        expressions=[],
        history_expressions=[],
        chart_data=None,
        timeline=timeline,
    )

    assert load_session(path)["timeline"] == timeline


def test_invalid_timeline_is_rejected(tmp_path) -> None:
    path = tmp_path / "bad-timeline.ptlua.json"
    path.write_text(
        '{"version":1,"connection":{},"poll_interval_ms":500,'
        '"expressions":[],"timeline":"client"}',
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="шкал"):
        load_session(path)


def test_old_session_enables_history_for_existing_expressions(tmp_path) -> None:
    path = tmp_path / "old-with-expression.ptlua.json"
    path.write_text(
        '{"version":1,"connection":{},"poll_interval_ms":500,'
        '"expressions":["x"]}',
        encoding="utf-8",
    )

    document = load_session(path)

    assert document["history_expressions"] == ["x"]


def test_old_session_accepts_legacy_min_max_statistics(tmp_path) -> None:
    path = tmp_path / "old-statistics.ptlua.json"
    path.write_text(
        '{"version":1,"connection":{},"poll_interval_ms":500,'
        '"expressions":["x"],"statistics":{"x":{"min":1,"max":9}}}',
        encoding="utf-8",
    )

    document = load_session(path)

    assert document["statistics"] == {"x": {"min": 1, "max": 9}}


def test_old_session_statistics_are_normalized_to_compact_fields(
    tmp_path,
) -> None:
    path = tmp_path / "old-full-statistics.ptlua.json"
    path.write_text(
        json.dumps(
            {
                "version": 1,
                "connection": {},
                "poll_interval_ms": 500,
                "expressions": ["x"],
                "statistics": {
                    "x": {
                        "min": 1.5,
                        "max": 8.0,
                        "average": 4.75,
                        "median": 4.75,
                        "_sum": 9.5,
                        "_count": 2,
                        "_values": [1.5, 8.0],
                        "_last_sample": {"time_ms": 2, "value": 8.0},
                    }
                },
            }
        ),
        encoding="utf-8",
    )

    document = load_session(path)

    assert document["statistics"] == {
        "x": {
            "min": 1.5,
            "max": 8.0,
            "_sum": 9.5,
            "_count": 2,
            "_last_sample": {"time_ms": 2, "value": 8.0},
        }
    }


def test_old_session_accepts_interim_statistics_with_average(
    tmp_path,
) -> None:
    path = tmp_path / "interim-statistics.ptlua.json"
    path.write_text(
        json.dumps(
            {
                "version": 1,
                "connection": {},
                "poll_interval_ms": 500,
                "expressions": ["x"],
                "statistics": {
                    "x": {
                        "min": 1.5,
                        "max": 8.0,
                        "average": 4.75,
                        "_sum": 9.5,
                        "_count": 2,
                        "_last_sample": {"time_ms": 2, "value": 8.0},
                    }
                },
            }
        ),
        encoding="utf-8",
    )

    document = load_session(path)

    assert document["statistics"]["x"] == {
        "min": 1.5,
        "max": 8.0,
        "_sum": 9.5,
        "_count": 2,
        "_last_sample": {"time_ms": 2, "value": 8.0},
    }


def test_save_session_strips_legacy_statistics_fields(tmp_path) -> None:
    path = tmp_path / "legacy-statistics.ptlua.json"
    save_session(
        path,
        host="localhost",
        port=10_000,
        poll_interval_ms=500,
        history_limit=5_000,
        expressions=["x"],
        history_expressions=["x"],
        chart_data=None,
        statistics={
            "x": {
                "min": 1.5,
                "max": 8.0,
                "average": 4.75,
                "median": 4.75,
                "_sum": 9.5,
                "_count": 2,
                "_values": [1.5, 8.0],
                "_last_sample": {"time_ms": 2, "value": 8.0},
            }
        },
    )

    saved = json.loads(path.read_text(encoding="utf-8"))

    assert saved["statistics"]["x"] == {
        "min": 1.5,
        "max": 8.0,
        "_sum": 9.5,
        "_count": 2,
        "_last_sample": {"time_ms": 2, "value": 8.0},
    }
    assert "average" not in saved["statistics"]["x"]
    assert "median" not in saved["statistics"]["x"]
    assert "_values" not in saved["statistics"]["x"]


def _base_session_kwargs(**overrides):
    kwargs = {
        "host": "localhost",
        "port": 10_000,
        "poll_interval_ms": 500,
        "history_limit": 5_000,
        "expressions": ["x"],
        "history_expressions": ["x"],
        "chart_data": {"ok": True, "series": []},
    }
    kwargs.update(overrides)
    return kwargs


def test_setter_settings_roundtrip_and_old_session_defaults(tmp_path) -> None:
    path = tmp_path / "setter.ptlua.json"
    settings = {"x": {"code": "set_x(<newvalue>)", "limits": "(0;1;2)"}}
    save_session(path, setter_settings=settings, **_base_session_kwargs())
    assert load_session(path)["setter_settings"] == settings
    document = json.loads(path.read_text(encoding="utf-8"))
    del document["setter_settings"]
    path.write_text(json.dumps(document), encoding="utf-8")
    assert load_session(path)["setter_settings"] == {}


@pytest.mark.parametrize("settings", [
    "not-a-dict", [], None,
    {"other": {"code": "f(<newvalue>)", "limits": ""}},
    {"x": "not-a-dict"},
    {"x": {"code": "f(<newvalue>)"}},
    {"x": {"code": 1, "limits": ""}},
    {"x": {"code": "x" * 16385, "limits": ""}},
    {"x": {"code": "f(<newvalue>)", "limits": "я" * 8193}},
])
def test_invalid_setter_settings_are_rejected(tmp_path, settings) -> None:
    path = tmp_path / "invalid-setter.ptlua.json"
    save_session(path, **_base_session_kwargs())
    document = json.loads(path.read_text(encoding="utf-8"))
    document["setter_settings"] = settings
    path.write_text(json.dumps(document), encoding="utf-8")
    with pytest.raises(ValueError, match="setter"):
        load_session(path)


def test_variable_browser_state_roundtrip(tmp_path) -> None:
    path = tmp_path / "browser.ptlua.json"
    save_session(
        path,
        variable_browser={
            "root": "OBJECT1",
            "watched_expressions": ["OBJECT1.level", "OBJECT1.level", "x"],
        },
        **_base_session_kwargs(),
    )
    document = load_session(path)
    assert document["variable_browser"] == {
        "root": "OBJECT1",
        "watched_expressions": ["OBJECT1.level", "x"],
    }


def test_variable_browser_defaults_for_old_sessions(tmp_path) -> None:
    path = tmp_path / "old.ptlua.json"
    save_session(path, **_base_session_kwargs())
    raw = json.loads(path.read_text(encoding="utf-8"))
    del raw["variable_browser"]
    path.write_text(json.dumps(raw), encoding="utf-8")
    document = load_session(path)
    assert document["variable_browser"] == {
        "root": "_G",
        "watched_expressions": [],
    }


@pytest.mark.parametrize("browser", [
    "not-a-dict",
    {"root": 5, "watched_expressions": []},
    {"root": "_G", "watched_expressions": "x"},
    {"root": "_G", "watched_expressions": ["x", 42]},
    {"root": "", "watched_expressions": []},
])
def test_invalid_variable_browser_state_is_rejected(tmp_path, browser) -> None:
    path = tmp_path / "broken.ptlua.json"
    save_session(path, **_base_session_kwargs())
    raw = json.loads(path.read_text(encoding="utf-8"))
    raw["variable_browser"] = browser
    path.write_text(json.dumps(raw), encoding="utf-8")
    with pytest.raises(ValueError, match="браузера переменных"):
        load_session(path)


def test_variable_browser_watched_expressions_capped(tmp_path) -> None:
    path = tmp_path / "many.ptlua.json"
    save_session(path, **_base_session_kwargs())
    raw = json.loads(path.read_text(encoding="utf-8"))
    raw["variable_browser"] = {
        "root": "_G",
        "watched_expressions": [f"e{i}" for i in range(40)],
    }
    path.write_text(json.dumps(raw), encoding="utf-8")
    document = load_session(path)
    assert len(document["variable_browser"]["watched_expressions"]) == 16


def test_variable_browser_paths_use_byte_limits(tmp_path) -> None:
    path = tmp_path / "bytes.ptlua.json"
    save_session(path, **_base_session_kwargs())
    document = json.loads(path.read_text(encoding="utf-8"))
    # 600 two-byte characters exceed the 1024-byte protocol limit.
    document["variable_browser"] = {
        "root": "_G",
        "watched_expressions": ["я" * 600],
    }
    path.write_text(json.dumps(document), encoding="utf-8")
    with pytest.raises(ValueError):
        load_session(path)


def test_variable_browser_newline_paths_rejected(tmp_path) -> None:
    path = tmp_path / "newline.ptlua.json"
    save_session(path, **_base_session_kwargs())
    document = json.loads(path.read_text(encoding="utf-8"))
    document["variable_browser"] = {
        "root": "_G",
        "watched_expressions": ["x\ny"],
    }
    path.write_text(json.dumps(document), encoding="utf-8")
    with pytest.raises(ValueError):
        load_session(path)
