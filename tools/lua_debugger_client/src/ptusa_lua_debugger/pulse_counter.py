from __future__ import annotations

from dataclasses import dataclass, field
from math import isfinite
from typing import Any


@dataclass(frozen=True)
class PulseDefinition:
    name: str
    source: str
    dependent: str
    source_value: Any
    dependent_value: Any

    @property
    def expression(self) -> str:
        return f"[Импульсы] {self.name}"


@dataclass
class _State:
    count: int = 0
    values: dict[str, Any] = field(default_factory=dict)
    last_samples: dict[str, dict[str, Any]] = field(default_factory=dict)


class PulseCounters:
    """Publish a source burst's size on the first following dependent edge."""

    def __init__(self) -> None:
        self.definitions: dict[str, PulseDefinition] = {}
        self.states: dict[str, _State] = {}
        self._anchor: tuple[Any, Any] | None = None

    def add(self, definition: PulseDefinition) -> None:
        if not definition.name.strip():
            raise ValueError("Укажите имя счётчика")
        if not definition.source.strip() or not definition.dependent.strip():
            raise ValueError("Укажите оба Lua-выражения")
        if definition.source == definition.dependent:
            raise ValueError("Заданная и зависимая величины должны различаться")
        if any(not _valid_target(value) for value in
               (definition.source_value, definition.dependent_value)):
            raise ValueError("Значения должны быть числом, логическим значением или строкой")
        if definition.expression in self.definitions:
            raise ValueError("Вычисляемая величина с таким именем уже существует")
        self.definitions[definition.expression] = definition
        self.states[definition.expression] = _State()

    def remove(self, expression: str) -> None:
        self.definitions.pop(expression, None)
        self.states.pop(expression, None)

    def reset(self) -> None:
        self.states = {expression: _State() for expression in self.definitions}
        self._anchor = None

    def seed(self, expression: str, data: dict[str, Any]) -> None:
        """Start a new counter at the latest known samples, without replaying history."""
        definition = self.definitions[expression]
        state = self.states[expression]
        self._anchor = (data.get("controller_time_unix_ms"),
                        data.get("controller_time_millisec"))
        for item in data.get("series", []):
            if item.get("expression") not in (definition.source, definition.dependent):
                continue
            samples = item.get("samples", [])
            if samples:
                sample = samples[-1]
                source = item["expression"]
                state.last_samples[source] = sample
                if sample.get("ok"):
                    state.values[source] = sample.get("value")

    def process(self, data: dict[str, Any]) -> list[dict[str, Any]]:
        anchor = (data.get("controller_time_unix_ms"),
                  data.get("controller_time_millisec"))
        if self._anchor is not None and anchor != self._anchor:
            self.reset()
        self._anchor = anchor
        by_expression = {
            item.get("expression"): item.get("samples", [])
            for item in data.get("series", []) if isinstance(item, dict)
        }
        return [self._process_one(definition, self.states[expression], by_expression,
                                  int(data.get("server_time_ms", 0)))
                for expression, definition in self.definitions.items()]

    @staticmethod
    def _new_samples(samples: list[dict[str, Any]],
                     previous: dict[str, Any] | None) -> list[dict[str, Any]]:
        if previous is None:
            return samples
        for index in range(len(samples) - 1, -1, -1):
            if samples[index] == previous:
                return samples[index + 1:]
        # A rolling server cache may have moved past the last observed sample.
        # Millisecond counters wrap, so compare them modulo 2**32.
        old_time = previous.get("time_ms")
        if isinstance(old_time, int):
            return [sample for sample in samples
                    if isinstance(sample.get("time_ms"), int)
                    and 0 < ((sample["time_ms"] - old_time) & 0xFFFFFFFF) < 0x80000000]
        return samples

    def _process_one(self, definition: PulseDefinition, state: _State,
                     by_expression: dict[str, list[dict[str, Any]]],
                     server_time: int) -> dict[str, Any]:
        expressions = (definition.dependent, definition.source)
        events: list[tuple[int, int, str, dict[str, Any]]] = []
        for priority, expression in enumerate(expressions):
            samples = by_expression.get(expression, [])
            new = self._new_samples(samples, state.last_samples.get(expression))
            if samples:
                state.last_samples[expression] = samples[-1]
            for sample in new:
                if isinstance(sample, dict) and isinstance(sample.get("time_ms"), int):
                    events.append((sample["time_ms"], priority, expression, sample))
        # Order both streams by age relative to the poll, including counter wrap.
        # A dependent edge closes the preceding burst before a simultaneous source edge.
        events.sort(key=lambda event: (-((server_time - event[0]) & 0xFFFFFFFF), event[1]))
        result: list[dict[str, Any]] = []
        for time_ms, _priority, expression, sample in events:
            if not sample.get("ok"):
                state.values.pop(expression, None)
                continue
            value = sample.get("value")
            previous = state.values.get(expression)
            state.values[expression] = value
            target = (definition.source_value if expression == definition.source
                      else definition.dependent_value)
            if (previous is not None and not _matches(previous, target)
                    and _matches(value, target)):
                if expression == definition.source:
                    state.count += 1
                elif state.count:
                    # Repeated dependent pulses must not publish empty bursts.
                    # Keep equal totals as distinct history events (10, 10, 10).
                    result.append(_sample(time_ms, state.count))
                    state.count = 0
        return {"expression": definition.expression, "samples": result}

    def snapshot(self) -> dict[str, Any]:
        return {expression: {
            "count": state.count, "values": state.values,
            "last_samples": state.last_samples,
        } for expression, state in self.states.items()}

    def restore(self, snapshot: dict[str, Any], anchor: tuple[Any, Any]) -> None:
        self._anchor = anchor
        for expression, values in snapshot.items():
            if expression in self.states:
                self.states[expression] = _State(**values)


def _sample(time_ms: int, value: int) -> dict[str, Any]:
    return {"time_ms": time_ms, "ok": True, "type": "number", "value": value}


def _valid_target(value: Any) -> bool:
    return isinstance(value, (str, int, float, bool)) and (
        not isinstance(value, float) or isfinite(value)
    )


def _matches(value: Any, target: Any) -> bool:
    if isinstance(value, bool) or isinstance(target, bool):
        return type(value) is type(target) and value == target
    return value == target
