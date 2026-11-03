import pytest

from ptusa_lua_debugger.setter import allowed_values, lua_literal, scalar, setter_code


@pytest.mark.parametrize("text, expected", [
    ("(0;1;2)", [0, 1, 2]), (" -1; 2.5; 1e2 ", [-1, 2.5, 100]),
    ("(true;false)", [True, False]), ('("a;b";"в")', ["a;b", "в"]),
    ("", []),
])
def test_allowed_values(text, expected):
    assert allowed_values(text) == expected


@pytest.mark.parametrize("text", ["()", "0;", "0;;1", "0,1", "(0;1", "NaN", "[]"])
def test_invalid_limits(text):
    with pytest.raises(ValueError):
        allowed_values(text)


@pytest.mark.parametrize("text", ["nil", "null", "{}", "[]", "NaN", "Infinity",
                                 "1e999", "9" * 400, "1); dangerous()"])
def test_values_are_finite_scalar_data(text):
    with pytest.raises(ValueError):
        scalar(text)


def test_setter_guard_and_readback():
    code = setter_code("LINE1V1:get_value()", "LINE1V1:set_value(<newvalue>)", "(0;1;2)", "2")
    assert code.index("if not") < code.index("LINE1V1:set_value(2)")
    assert code.count("LINE1V1:set_value(2)") == 1
    assert code.endswith("return (LINE1V1:get_value()\n)")
    for value in ("3", "true", '"1"'):
        with pytest.raises(ValueError):
            setter_code("x", "f(<newvalue>)", "(0;1;2)", value)
    assert "f(1.0)" in setter_code("x", "f(<newvalue>)", "(0;1;2)", "1.0")


def test_strings_cannot_inject_code():
    import json
    value = '\"); dangerous() --\n\x00Привет'
    code = setter_code("x", "f(<newvalue>)", "", json.dumps(value))
    assert "dangerous" not in code
    assert lua_literal(value) in code


def test_setter_requires_placeholder_and_bounds_generated_code():
    with pytest.raises(ValueError, match="newvalue"):
        setter_code("x", "f(1)", "", "1")
    with pytest.raises(ValueError, match="16384"):
        setter_code("x", "f(<newvalue>)", "", '"' + "x" * 5000 + '"')
