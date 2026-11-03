"""Scalar setter values are data, never user-supplied Lua expressions."""
from __future__ import annotations

import json
import math
from typing import Any


def scalar(text: str) -> Any:
    try:
        value = json.loads(text)
    except (ValueError, TypeError) as exc:
        raise ValueError('Введите число, true, false или строку в кавычках') from exc
    try:
        finite = not isinstance(value, (int, float)) or math.isfinite(value)
    except OverflowError:
        finite = False
    if not isinstance(value, (str, bool, int, float)) or not finite:
        raise ValueError("Допустимы только конечные числа, строки и boolean")
    return value


def allowed_values(text: str) -> list[Any]:
    text = text.strip()
    if not text:
        return []
    if text.startswith("(") and text.endswith(")"):
        text = text[1:-1].strip()
    if not text:
        raise ValueError("Список допустимых значений пуст")
    values = []
    decoder = json.JSONDecoder()
    while text:
        try:
            _, end = decoder.raw_decode(text)
        except ValueError as exc:
            raise ValueError('Ограничения: (0;1;2), (true;false), ("a";"b")') from exc
        values.append(scalar(text[:end]))
        text = text[end:].strip()
        if not text:
            break
        if not text.startswith(";") or not text[1:].strip():
            raise ValueError("Значения должны разделяться точкой с запятой")
        text = text[1:].strip()
    return values


def lua_literal(value: Any) -> str:
    if isinstance(value, str):
        # Decimal byte escapes work in Lua 5.1, including NUL and UTF-8.
        return '"' + "".join(f"\\{byte:03d}" for byte in value.encode("utf-8")) + '"'
    if isinstance(value, bool):
        return "true" if value else "false"
    return repr(value)


def setter_code(getter: str, setter: str, limits: str, new_value: str) -> str:
    if not getter.strip() or "<newvalue>" not in setter:
        raise ValueError("Setter должен содержать <newvalue>")
    value = scalar(new_value)
    allowed = allowed_values(limits)
    # Python considers True == 1; Lua does not.
    if allowed and not any(
        isinstance(value, bool) == isinstance(candidate, bool)
        and value == candidate for candidate in allowed
    ):
        raise ValueError("Значение не входит в список допустимых")
    literal = lua_literal(value)
    guard = ""
    if allowed:
        # Check on the controller as well, before running the setter.
        condition = " or ".join(f"({literal} == {lua_literal(v)})" for v in allowed)
        guard = f'if not ({condition}) then error("Value is not allowed") end\n'
    # A separate function prevents a setter's return/local variables from
    # skipping or shadowing the getter. The setter runs exactly once.
    code = (guard + "do\nlocal function apply()\n"
            + setter.replace("<newvalue>", literal)
            + "\nend\napply()\nend\nreturn (" + getter + "\n)")
    if len(code.encode("utf-8")) > 16384:
        raise ValueError("Lua-код setter превышает 16384 байта")
    return code
