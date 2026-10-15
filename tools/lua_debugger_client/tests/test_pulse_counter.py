from ptusa_lua_debugger.pulse_counter import PulseCounters, PulseDefinition


def packet(left, right, *, anchor=1000, now=None):
    def series(expression, entries):
        return {"expression": expression, "samples": [
            {"time_ms": time, "ok": True, "type": "number", "value": value}
            for time, value in entries
        ]}
    return {
        "controller_time_unix_ms": 10_000,
        "controller_time_millisec": anchor,
        "server_time_ms": now if now is not None else max(
            [time for time, _ in left + right], default=anchor),
        "series": [series("left", left), series("right", right)],
    }


def counters():
    engine = PulseCounters()
    engine.add(PulseDefinition("left", "left", "right", 1, 1))
    engine.add(PulseDefinition("right", "right", "left", 1, 1))
    return engine


def bursts():
    left, right = [(1000, 0)], [(1000, 0)]
    time = 1010
    for target, count in [(left, 10), (right, 10), (left, 10), (right, 10),
                          (left, 7), (right, 12), (left, 1)]:
        for _ in range(count):
            target.extend([(time, 1), (time + 2, 0)])
            time += 10
    return left, right


def test_alternating_bursts_publish_equal_and_different_totals():
    engine = counters()
    data = packet(*bursts())
    result = engine.process(data)
    assert [s["value"] for s in result[0]["samples"]] == [10, 10, 7]
    assert [s["value"] for s in result[1]["samples"]] == [10, 10, 12]
    assert all(not item["samples"] for item in engine.process(data))


def test_rolling_cache_and_poll_boundaries_do_not_change_totals():
    left, right = bursts()
    engine = counters()
    observed = [[], []]
    for end in range(1040, 1700, 40):
        data = packet([entry for entry in left if entry[0] <= end][-12:],
                      [entry for entry in right if entry[0] <= end][-12:], now=end)
        for index, series in enumerate(engine.process(data)):
            observed[index].extend(s["value"] for s in series["samples"])
    assert observed == [[10, 10, 7], [10, 10, 12]]


def test_dependent_closes_only_nonempty_bursts_and_same_time_source_starts_next():
    engine = counters()
    engine.process(packet([(1000, 0), (1010, 1), (1012, 0)], [(1000, 0)]))
    result = engine.process(packet([(1030, 1), (1032, 0)],
                                   [(1030, 1), (1032, 0), (1040, 1),
                                    (1042, 0), (1050, 1)]))
    assert [(s["time_ms"], s["value"]) for s in result[0]["samples"]] == [
        (1030, 1), (1040, 1)]


def test_restore_pending_burst_and_reset_on_new_controller_session():
    engine = counters()
    first = packet([(1000, 0), (1010, 1), (1012, 0)], [(1000, 0)])
    assert not engine.process(first)[0]["samples"]
    restored = counters()
    restored.restore(engine.snapshot(), (10_000, 1000))
    assert not restored.process(first)[0]["samples"]
    result = restored.process(packet([(1020, 1), (1022, 0)], [(1030, 1)]))
    assert [s["value"] for s in result[0]["samples"]] == [2]
    restarted = packet([(1000, 0), (1010, 1)], [(1000, 0), (1020, 1)], anchor=999)
    assert [s["value"] for s in restored.process(restarted)[0]["samples"]] == [1]


def test_new_counter_does_not_recount_existing_history():
    engine = counters()
    cached = packet([(1000, 0), (1010, 1), (1020, 0)], [(1000, 0)])
    engine.seed(next(iter(engine.definitions)), cached)
    assert not engine.process(cached)[0]["samples"]
    result = engine.process(packet([(1010, 1), (1020, 0), (1030, 1)], [(1040, 1)]))
    assert [s["value"] for s in result[0]["samples"]] == [1]


def test_events_remain_chronological_across_millisecond_wrap():
    engine = counters()
    result = engine.process(packet(
        [(0xFFFFFFF0, 0), (0xFFFFFFF5, 1), (0xFFFFFFFA, 0), (2, 1)],
        [(0xFFFFFFF0, 0), (5, 1)], now=6))
    assert [(s["time_ms"], s["value"]) for s in result[0]["samples"]] == [(5, 2)]
