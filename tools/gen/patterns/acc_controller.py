"""Pattern acc_controller: CAN-in/CAN-out longitudinal controller (ACC) with E2E P11, timeouts,
driver mode logic, gap/speed control with acceleration and jerk limits, and a closed-loop SIL
harness against the hand-written plant (sil/plant). Rules GEN-060..GEN-064.

The pattern fixes the architecture; the model supplies the DBC interface, the bindings of the
pattern interface to DBC signals, the timing and the calibration."""
from __future__ import annotations

from tools.gen import dbc, loader
from tools.gen.errors import Diagnostics
from tools.gen.patterns.codec import Message, Signal, build_message, to_camel, to_snake

MODES = ["OFF", "STANDBY", "ACTIVE", "OVERRIDE", "FAULT"]  # codes 0..4 are part of the interface

CALIBRATION = {
    "min_activation_speed_kph", "disengage_speed_kph", "max_set_speed_kph", "set_speed_step_kph",
    "time_gaps_s", "standstill_distance_m", "max_target_range_m", "lane_half_width_m",
    "accel_max_mps2", "decel_max_mps2", "jerk_increase_max_mps3", "jerk_decrease_max_mps3",
    "override_pedal_pct", "speed_gain_per_s", "gap_gain_per_s2", "range_rate_gain_per_s",
}

# name -> (direction, discrete?, function(cal) -> physical range the bound signal must cover)
INTERFACE = {
    "target_valid": ("rx", True, lambda c: (0, 1)),
    "target_range": ("rx", False, lambda c: (0, c["max_target_range_m"])),
    "target_range_rate": ("rx", False, lambda c: (-50, 50)),
    "target_lateral": ("rx", False, lambda c: (-2 * c["lane_half_width_m"], 2 * c["lane_half_width_m"])),
    "ego_speed": ("rx", False, lambda c: (0, c["max_set_speed_kph"])),
    "main_switch": ("rx", True, lambda c: (0, 1)),
    "set_button": ("rx", True, lambda c: (0, 1)),
    "resume_button": ("rx", True, lambda c: (0, 1)),
    "cancel_button": ("rx", True, lambda c: (0, 1)),
    "speed_up_button": ("rx", True, lambda c: (0, 1)),
    "speed_down_button": ("rx", True, lambda c: (0, 1)),
    "gap_setting": ("rx", True, lambda c: (1, len(c["time_gaps_s"]))),
    "brake_pedal": ("rx", True, lambda c: (0, 1)),
    "accel_pedal": ("rx", False, lambda c: (0, 100)),
    "status": ("tx", True, lambda c: (0, len(MODES) - 1)),
    "accel_request": ("tx", False, lambda c: (0, c["accel_max_mps2"])),
    "brake_request": ("tx", False, lambda c: (0, c["decel_max_mps2"])),
    "steering_request": ("tx", False, lambda c: (0, 0)),
    "set_speed": ("tx", False, lambda c: (0, c["max_set_speed_kph"])),
    "time_gap_setting": ("tx", True, lambda c: (1, len(c["time_gaps_s"]))),
}

_EPS = 1e-9


def _e2e_layout(msg: Message, e2e: dict, where: str, display: str, diags: Diagnostics) -> dict | None:
    """GEN-062: CRC = one whole byte, counter = one nibble, both plain unsigned integers."""
    sigs = {s.dbc_name or s.name: s for s in msg.signals}
    crc, ctr = sigs.get(e2e["crc"]), sigs.get(e2e["counter"])
    ok = True
    if crc is None or ctr is None:
        diags.error("GEN-062", display, f"{where}: E2E signals '{e2e['crc']}'/'{e2e['counter']}' not in {msg.name}")
        return None
    plain = lambda s: (not s.signed) and s.factor == 1.0 and s.offset == 0.0  # noqa: E731
    if not (plain(crc) and crc.length == 8 and not crc.big_endian and crc.start_bit % 8 == 0):
        diags.error("GEN-062", display, f"{where}: CRC signal must be an unsigned Intel byte")
        ok = False
    if not (plain(ctr) and ctr.length == 4 and not ctr.big_endian and ctr.start_bit % 4 == 0):
        diags.error("GEN-062", display, f"{where}: counter signal must be an unsigned Intel nibble")
        ok = False
    if ok and crc.start_bit // 8 == ctr.start_bit // 8:
        diags.error("GEN-062", display, f"{where}: CRC and counter share a byte")
        ok = False
    if not 2 <= msg.payload_length <= 8:
        diags.error("GEN-062", display, f"{where}: E2E P11 needs a classic CAN payload of 2..8 bytes")
        ok = False
    if not ok:
        return None
    return {
        "data_id": e2e["data_id"], "crc_byte": crc.start_bit // 8, "counter_bit": ctr.start_bit,
        "max_delta": e2e["max_delta_counter"], "crc_name": crc.name, "counter_name": ctr.name,
    }


def build(inputs, diags: Diagnostics):
    from tools.gen.project import Context

    model = inputs.model
    display = inputs.model_display
    comp = model["component"]
    satisfies: list[tuple[str, str]] = []

    def sat(element: str, refs) -> None:
        satisfies.extend((element, r) for r in refs)

    # Messages from DBC files.
    messages: dict[str, Message] = {}
    for rel in model["inputs"]["dbc"]:
        path = inputs.resolve_input(rel, diags)
        if path is None:
            continue
        for m in dbc.parse(loader.read_text(path), inputs.display(path), diags):
            if m["name"] in messages:
                diags.error("GEN-030", inputs.display(path), f"duplicate message '{m['name']}'")
                continue
            msg = build_message(m, inputs.display(path), diags)
            if msg is not None:
                messages[msg.name] = msg

    # Calibration (GEN-063)
    raw_cal = model["calibration"]
    cal: dict = {}
    for key in sorted(set(raw_cal) | CALIBRATION):
        if key not in CALIBRATION:
            diags.error("GEN-063", display, f"unknown calibration '{key}'")
            continue
        if key not in raw_cal:
            diags.error("GEN-063", display, f"missing calibration '{key}'")
            continue
        value = raw_cal[key]["value"]
        if (key == "time_gaps_s") != isinstance(value, list):
            diags.error("GEN-063", display, f"calibration '{key}' has the wrong shape")
            continue
        cal[key] = value
        sat(f"calibration:{key}", raw_cal[key].get("satisfies", []))
    if set(cal) != CALIBRATION:
        return None
    gaps = cal["time_gaps_s"]
    checks = [
        (all(g > 0 for g in gaps) and all(a < b for a, b in zip(gaps, gaps[1:])), "time gaps must be positive and ascending"),
        (0 < cal["disengage_speed_kph"] < cal["min_activation_speed_kph"] <= cal["max_set_speed_kph"],
         "need 0 < disengage < min activation <= max set speed"),
        (cal["set_speed_step_kph"] > 0, "set speed step must be positive"),
        (all(cal[k] > 0 for k in ("accel_max_mps2", "decel_max_mps2", "jerk_increase_max_mps3", "jerk_decrease_max_mps3",
                                  "speed_gain_per_s", "gap_gain_per_s2", "range_rate_gain_per_s", "max_target_range_m",
                                  "lane_half_width_m", "standstill_distance_m")), "limits and gains must be positive"),
        (0 < cal["override_pedal_pct"] < 100, "override threshold must lie in (0, 100) %"),
    ]
    for ok, why in checks:
        if not ok:
            diags.error("GEN-063", display, why)

    # Timing (GEN-064) and rx/tx messages (GEN-060)
    cycle = model["cycle_ms"]
    rx_list = []
    seen_rx: set[str] = set()
    for r in model["rx"]:
        where = f"rx {r['message']}"
        msg = messages.get(r["message"])
        if msg is None or r["message"] in seen_rx:
            diags.error("GEN-060", display, f"{where}: not a (unique) DBC message")
            continue
        seen_rx.add(r["message"])
        if r["period_ms"] % cycle or r["timeout_ms"] % cycle or r["timeout_ms"] < 2 * r["period_ms"]:
            diags.error("GEN-064", display,
                        f"{where}: period and timeout must be multiples of the {cycle} ms cycle, timeout >= 2 periods")
        # A frame arriving right before the timeout follows timeout/period - 1 lost frames, so its
        # counter jumps by timeout/period: a smaller max_delta_counter would reject it and make the
        # effective timeout longer than specified.
        if r["e2e"]["max_delta_counter"] < r["timeout_ms"] // r["period_ms"]:
            diags.error("GEN-064", display,
                        f"{where}: max_delta_counter {r['e2e']['max_delta_counter']} < timeout/period "
                        f"{r['timeout_ms'] // r['period_ms']}; frames before the timeout would fail E2E")
        layout = _e2e_layout(msg, r["e2e"], where, display, diags)
        sat(f"rx:{msg.name}", r.get("satisfies", []))
        rx_list.append({"msg": msg, "var": f"rx_{msg.snake}_", "cname": msg.name, "snake": msg.snake,
                        "timeout_cycles": r["timeout_ms"] // cycle, "e2e": layout})
    tx = model["tx"]
    tx_msg = messages.get(tx["message"])
    tx_e2e = None
    if tx_msg is None or tx["message"] in seen_rx:
        diags.error("GEN-060", display, f"tx {tx['message']}: not a DBC message distinct from the rx messages")
    else:
        tx_e2e = _e2e_layout(tx_msg, tx["e2e"], f"tx {tx_msg.name}", display, diags)
        sat(f"tx:{tx_msg.name}", tx.get("satisfies", []))

    # Bindings (GEN-060 existence/direction, GEN-061 ranges)
    bindings: dict[str, dict] = {}
    rx_by_name = {r["cname"]: r for r in rx_list}
    for name in sorted(set(model["bindings"]) | set(INTERFACE)):
        if name not in INTERFACE:
            diags.error("GEN-060", display, f"'{name}' is not part of the acc_controller interface")
            continue
        if name not in model["bindings"]:
            diags.error("GEN-060", display, f"interface signal '{name}' is not bound")
            continue
        direction, discrete, needed = INTERFACE[name]
        msg_name, sig_name = model["bindings"][name].split(".", 1)
        msg = messages.get(msg_name)
        sig: Signal | None = None
        if msg is not None:
            sig = next((s for s in msg.signals if (s.dbc_name or s.name) == sig_name), None)
        in_dir = (msg_name in rx_by_name) if direction == "rx" else (tx_msg is not None and msg_name == tx_msg.name)
        if sig is None or not in_dir:
            diags.error("GEN-060", display, f"'{name}' must bind to a signal of a {direction} message, not {msg_name}.{sig_name}")
            continue
        e2e = rx_by_name[msg_name]["e2e"] if direction == "rx" else tx_e2e
        if e2e is not None and sig.name in (e2e["crc_name"], e2e["counter_name"]):
            diags.error("GEN-062", display, f"'{name}' is bound to an E2E protection signal")
            continue
        lo, hi = needed(cal)
        if sig.min > lo + _EPS or sig.max < hi - _EPS:
            diags.error("GEN-061", display, f"'{name}' needs [{lo}, {hi}], {msg_name}.{sig_name} covers [{sig.min}, {sig.max}]")
        if discrete and (sig.factor != 1.0 or sig.offset != 0.0):
            diags.error("GEN-061", display, f"'{name}' is discrete and needs factor 1, offset 0")
        bindings[name] = {"msg": msg, "sig": sig, "expr": f"{rx_by_name[msg_name]['var']}.{sig.name}" if direction == "rx" else f"cmd.{sig.name}"}
    if tx_msg is not None and tx_e2e is not None:
        bound_tx = {b["sig"].name for b in bindings.values() if b["msg"] is tx_msg}
        for s in tx_msg.signals:
            if s.name not in bound_tx and s.name not in (tx_e2e["crc_name"], tx_e2e["counter_name"]):
                diags.error("GEN-060", display, f"tx signal {tx_msg.name}.{s.dbc_name or s.name} is not bound")

    # Integer jerk steps in units of the request resolution (GEN-063)
    jerk = {}
    if "accel_request" in bindings and "brake_request" in bindings:
        res = bindings["accel_request"]["sig"].factor
        if bindings["brake_request"]["sig"].factor != res:
            diags.error("GEN-063", display, "accel_request and brake_request need the same resolution")
        for key in ("jerk_increase_max_mps3", "jerk_decrease_max_mps3"):
            steps = cal[key] * cycle / 1000.0 / res
            if abs(steps - round(steps)) > 1e-6 or round(steps) < 1:
                diags.error("GEN-063", display, f"{key} * cycle must be a whole number (>= 1) of {res} m/s2 steps")
            jerk[key] = int(round(steps))
        jerk["resolution"] = res

    sat("modes", model["modes"].get("satisfies", []))
    sat("control", model["control"].get("satisfies", []))
    if not diags.ok() or tx_msg is None or tx_e2e is None or any(r["e2e"] is None for r in rx_list):
        return None

    snake = to_snake(comp["name"])
    variables = {
        "component": comp,
        "snake": snake,
        "ns": comp["namespace"],
        "inc": inputs.name,
        "messages": [messages[k] for k in sorted(messages)],
        "rx": rx_list,
        "tx": {"msg": tx_msg, "e2e": tx_e2e},
        "b": bindings,
        "cal": cal,
        "jerk": jerk,
        "cycle_ms": cycle,
        "modes": MODES,
        "default_gap": (len(gaps) + 1) // 2,
    }
    outputs = [
        ("types.hpp.j2", f"include/{inputs.name}/{snake}_types.hpp"),
        ("common/codec.hpp.j2", f"include/{inputs.name}/{snake}_codec.hpp"),
        ("common/codec.cpp.j2", f"src/{snake}_codec.cpp"),
        ("component.hpp.j2", f"include/{inputs.name}/{snake}.hpp"),
        ("component.cpp.j2", f"src/{snake}.cpp"),
        ("sil_harness.hpp.j2", f"sil/{snake}_sil.hpp"),
        ("sil_test.cpp.j2", f"sil/{snake}_sil_test.cpp"),
        ("CMakeLists.txt.j2", "CMakeLists.txt"),
    ]
    return Context(
        systems=[comp["name"]],
        satisfies=satisfies,
        resolve_ref=lambda kind, text: None,
        variables=variables,
        outputs=outputs,
        sil_suite=f"{comp['name']}SilTest",
    )
