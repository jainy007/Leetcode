"""Pattern can_function: a CAN-in / CAN-out vehicle function (ACC, AEB, ...).

The framework part (here) is shared by all features:
  - DBC import, rx/tx message selection, E2E P11 layout          (GEN-060, GEN-062)
  - binding of the feature's interface to DBC signals + ranges   (GEN-060, GEN-061)
  - calibration completeness / consistency                       (GEN-063)
  - receive timing and timeout supervision                       (GEN-064)
  - generated receive (E2E, decode, ages) -> Inputs, Outputs -> encode + protect
The feature part (tools/gen/features/<name>.py + templates/features/<name>/) supplies the
interface, calibration checks and the C++ logic that maps Inputs to Outputs."""
from __future__ import annotations

from tools.gen import dbc, loader
from tools.gen.errors import Diagnostics
from tools.gen.features import load_features
from tools.gen.patterns.codec import Message, Signal, build_message, codec_vectors, to_camel, to_snake
from tools.gen.render import REPO_ROOT, TEMPLATE_DIR

_EPS = 1e-9


def _e2e_layout(msg: Message, e2e: dict, where: str, display: str, diags: Diagnostics) -> dict | None:
    """GEN-062: CRC = one whole byte, counter = one nibble, both plain unsigned integers."""
    sigs = {s.dbc_name or s.name: s for s in msg.signals}
    crc, ctr = sigs.get(e2e["crc"]), sigs.get(e2e["counter"])
    if crc is None or ctr is None:
        diags.error("GEN-062", display, f"{where}: E2E signals '{e2e['crc']}'/'{e2e['counter']}' not in {msg.name}")
        return None
    ok = True
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
    return {"data_id": e2e["data_id"], "crc_byte": crc.start_bit // 8, "counter_bit": ctr.start_bit,
            "max_delta": e2e["max_delta_counter"], "crc_name": crc.name, "counter_name": ctr.name}


def build(inputs, diags: Diagnostics):
    from tools.gen.project import Context

    model = inputs.model
    display = inputs.model_display
    comp = model["component"]
    features = load_features()
    feature = features.get(model["feature"])
    if feature is None:
        diags.error("GEN-060", display, f"unknown feature '{model['feature']}' (known: {sorted(features)})")
        return None
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

    # Generated C++ declares one struct per message next to the component class.
    if comp["name"] in messages:
        diags.error("GEN-060", display, f"component name '{comp['name']}' collides with a DBC message name")

    # Calibration (GEN-063)
    raw_cal = model["calibration"]
    cal: dict = {}
    for key in sorted(set(raw_cal) | set(feature.CALIBRATION)):
        if key not in feature.CALIBRATION:
            diags.error("GEN-063", display, f"unknown calibration '{key}' for feature {feature.NAME}")
            continue
        if key not in raw_cal:
            diags.error("GEN-063", display, f"missing calibration '{key}'")
            continue
        value = raw_cal[key]["value"]
        if (feature.CALIBRATION[key] == "list") != isinstance(value, list):
            diags.error("GEN-063", display, f"calibration '{key}' has the wrong shape")
            continue
        cal[key] = value
        sat(f"calibration:{key}", raw_cal[key].get("satisfies", []))
    if set(cal) != set(feature.CALIBRATION):
        return None
    for why in feature.check(cal):
        diags.error("GEN-063", display, why)

    # rx messages and timing (GEN-060, GEN-064)
    cycle = model["cycle_ms"]
    rx_list, seen = [], set()
    for r in model["rx"]:
        where = f"rx {r['message']}"
        msg = messages.get(r["message"])
        if msg is None or r["message"] in seen:
            diags.error("GEN-060", display, f"{where}: not a (unique) DBC message")
            continue
        seen.add(r["message"])
        if r["period_ms"] % cycle or r["timeout_ms"] % cycle or r["timeout_ms"] < 2 * r["period_ms"]:
            diags.error("GEN-064", display,
                        f"{where}: period and timeout must be multiples of the {cycle} ms cycle, timeout >= 2 periods")
        # A frame arriving just before the timeout follows timeout/period - 1 lost frames, so its
        # counter jumps by timeout/period; a smaller max delta would reject it (late timeout).
        if r["e2e"]["max_delta_counter"] < r["timeout_ms"] // r["period_ms"]:
            diags.error("GEN-064", display,
                        f"{where}: max_delta_counter {r['e2e']['max_delta_counter']} < timeout/period "
                        f"{r['timeout_ms'] // r['period_ms']}; frames before the timeout would fail E2E")
        layout = _e2e_layout(msg, r["e2e"], where, display, diags)
        sat(f"rx:{msg.name}", r.get("satisfies", []))
        rx_list.append({"msg": msg, "var": f"rx_{msg.snake}_", "cname": msg.name, "snake": msg.snake,
                        "timeout_cycles": r["timeout_ms"] // cycle, "e2e": layout})

    # tx messages (GEN-060, GEN-062)
    tx_list = []
    for t in model["tx"]:
        msg = messages.get(t["message"])
        if msg is None or t["message"] in seen:
            diags.error("GEN-060", display, f"tx {t['message']}: not a DBC message distinct from the other rx/tx messages")
            continue
        seen.add(t["message"])
        layout = _e2e_layout(msg, t["e2e"], f"tx {msg.name}", display, diags)
        sat(f"tx:{msg.name}", t.get("satisfies", []))
        tx_list.append({"msg": msg, "cname": msg.name, "snake": msg.snake, "e2e": layout, "var": f"tx_{msg.snake}"})

    # Bindings (GEN-060 existence/direction, GEN-061 ranges, GEN-062 E2E signals)
    rx_by_name = {r["cname"]: r for r in rx_list}
    tx_by_name = {t["cname"]: t for t in tx_list}
    bindings: dict[str, dict] = {}
    for name in sorted(set(model["bindings"]) | set(feature.INTERFACE)):
        if name not in feature.INTERFACE:
            diags.error("GEN-060", display, f"'{name}' is not part of the {feature.NAME} interface")
            continue
        if name not in model["bindings"]:
            diags.error("GEN-060", display, f"interface signal '{name}' is not bound")
            continue
        direction, discrete, needed = feature.INTERFACE[name]
        msg_name, sig_name = model["bindings"][name].split(".", 1)
        side = rx_by_name if direction == "rx" else tx_by_name
        msg = messages.get(msg_name)
        sig: Signal | None = None
        if msg is not None:
            sig = next((s for s in msg.signals if (s.dbc_name or s.name) == sig_name), None)
        if sig is None or msg_name not in side:
            diags.error("GEN-060", display, f"'{name}' must bind to a signal of a {direction} message, not {msg_name}.{sig_name}")
            continue
        e2e = side[msg_name]["e2e"]
        # A function may read the counter of a received message (e.g. a monitor approving one
        # specific command), but never the CRC, and it never writes either.
        protected = () if e2e is None else ((e2e["crc_name"],) if direction == "rx" else (e2e["crc_name"], e2e["counter_name"]))
        if sig.name in protected:
            diags.error("GEN-062", display, f"'{name}' is bound to an E2E protection signal")
            continue
        lo, hi = needed(cal)
        if sig.min > lo + _EPS or sig.max < hi - _EPS:
            diags.error("GEN-061", display, f"'{name}' needs [{lo}, {hi}], {msg_name}.{sig_name} covers [{sig.min}, {sig.max}]")
        if discrete and (sig.factor != 1.0 or sig.offset != 0.0):
            diags.error("GEN-061", display, f"'{name}' is discrete and needs factor 1, offset 0")
        if direction == "tx" and not (sig.min <= 0.0 <= sig.max):
            diags.error("GEN-061", display, f"'{name}' must accept 0: frames before the first cycle carry zeros")
        bindings[name] = {"name": name, "dir": direction, "msg": msg, "sig": sig, "entry": side[msg_name]}
    for t in tx_list:
        if t["e2e"] is None:
            continue
        bound_here = {b["sig"].name for b in bindings.values() if b["msg"] is t["msg"]}
        for s in t["msg"].signals:
            if s.name not in bound_here and s.name not in (t["e2e"]["crc_name"], t["e2e"]["counter_name"]):
                diags.error("GEN-060", display, f"tx signal {t['cname']}.{s.dbc_name or s.name} is not bound")

    if not diags.ok() or any(e["e2e"] is None for e in rx_list + tx_list) or not tx_list:
        return None
    feature_vars, errors = feature.derive(cal, {k: b["sig"] for k, b in bindings.items()}, cycle)
    for why in errors:
        diags.error("GEN-063", display, why)
    sat("logic", model["logic"].get("satisfies", []))
    if not diags.ok():
        return None

    snake = to_snake(comp["name"])
    rx_inputs = [b for b in bindings.values() if b["dir"] == "rx"]
    tx_outputs = [b for b in bindings.values() if b["dir"] == "tx"]
    variables = {
        "component": comp, "snake": snake, "ns": comp["namespace"], "inc": inputs.name,
        "feature": feature.NAME, "observer_header": feature.OBSERVER[0], "observer_type": feature.OBSERVER[1],
        "stimulus": getattr(feature, "STIMULUS", None),
        "states": feature.STATES, "messages": [messages[k] for k in sorted(messages)],
        "rx": rx_list, "tx": tx_list, "inputs": sorted(rx_inputs, key=lambda b: b["name"]),
        "outputs": sorted(tx_outputs, key=lambda b: b["name"]), "b": bindings, "cal": cal, "cycle_ms": cycle,
        "codec_vectors": codec_vectors([messages[k] for k in sorted(messages)]),
        **feature_vars,
    }
    outputs = [
        ("types.hpp.j2", f"include/{inputs.name}/{snake}_types.hpp"),
        ("common/codec.hpp.j2", f"include/{inputs.name}/{snake}_codec.hpp"),
        ("common/codec.cpp.j2", f"src/{snake}_codec.cpp"),
        (f"features/{feature.NAME}/logic.hpp.j2", f"include/{inputs.name}/{snake}_logic.hpp"),
        (f"features/{feature.NAME}/logic.cpp.j2", f"src/{snake}_logic.cpp"),
        ("component.hpp.j2", f"include/{inputs.name}/{snake}.hpp"),
        ("component.cpp.j2", f"src/{snake}.cpp"),
        ("sil_harness.hpp.j2", f"sil/{snake}_sil.hpp"),
        ("sil_test.cpp.j2", f"sil/{snake}_sil_test.cpp"),
        ("common/codec_test.cpp.j2", f"sil/{snake}_codec_test.cpp"),
        ("component_test.cpp.j2", f"sil/{snake}_component_test.cpp"),
        ("CMakeLists.txt.j2", "CMakeLists.txt"),
    ]
    feature_files = sorted((TEMPLATE_DIR / "features" / feature.NAME).rglob("*.j2"))
    feature_files.append(REPO_ROOT / "sil" / "steps" / "features" / f"{feature.NAME}.yaml")
    extra_steps = list(getattr(feature, "EXTRA_STEPS", []))
    feature_files.extend(REPO_ROOT / rel for rel in extra_steps)
    return Context(
        systems=[comp["name"]],
        satisfies=satisfies,
        resolve_ref=lambda kind, text: None,
        variables=variables,
        outputs=outputs,
        sil_suite=f"{comp['name']}SilTest",
        step_catalogues=["sil/steps/can_function.yaml", *extra_steps, f"sil/steps/features/{feature.NAME}.yaml"],
        extra_tool_files=feature_files,
    )
