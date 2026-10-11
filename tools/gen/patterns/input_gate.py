"""Pattern input_gate: E2E-protected input, plausibility checks, debounced degradation state machine,
status output, periodic task glue and a SIL harness. Rules GEN-040..GEN-047."""
from __future__ import annotations

from collections import deque

from tools.gen import dbc, loader
from tools.gen.errors import Diagnostics
from tools.gen.patterns.codec import Message, build_message, to_camel, to_snake

CONDITIONS = ["valid", "e2e_error", "range_error", "missing"]
STATUS_SOURCES = ("state", "qualifier", "alive_counter", "e2e_error_count")


def build(inputs, diags: Diagnostics):
    from tools.gen.project import Context

    model = inputs.model
    display = inputs.model_display
    comp = model["component"]

    # Messages: from the model and from DBC files.
    raw_messages = [(m, display) for m in model["messages"]]
    for rel in model["inputs"].get("dbc", []):
        path = inputs.resolve_input(rel, diags)
        if path is not None:
            for m in dbc.parse(loader.read_text(path), inputs.display(path), diags):
                raw_messages.append((m, inputs.display(path)))
    messages: dict[str, Message] = {}
    for doc, where in raw_messages:
        if doc["name"] in messages:
            diags.error("GEN-030", where, f"duplicate message '{doc['name']}'")
            continue
        msg = build_message(doc, where, diags)
        if msg is not None:
            messages[msg.name] = msg

    gate = model["gate"]
    satisfies: list[tuple[str, str]] = []

    def sat(element: str, refs) -> None:
        satisfies.extend((element, r) for r in refs)

    for m in messages.values():
        sat(f"message:{m.name}", m.satisfies)
        for s in m.signals:
            sat(f"signal:{m.name}.{s.name}", s.satisfies)

    # GEN-040: input message
    inp = messages.get(gate["input"])
    if inp is None:
        diags.error("GEN-040", display, f"gate input '{gate['input']}' is not a declared message")
        return None
    if inp.e2e is None or inp.transport != "someip":
        diags.error("GEN-040", display, f"gate input '{inp.name}' must be a SOME/IP message with E2E protection")
        return None
    for s in inp.signals:
        if s.sil_default is None:
            diags.error("GEN-034", display, f"{inp.name}.{s.name}: input signals need a sil_default for SIL frames")

    # GEN-041: debounce counters
    debounce = []
    names = set()
    for d in gate["debounce"]:
        if d["name"] in names:
            diags.error("GEN-041", display, f"duplicate debounce counter '{d['name']}'")
            continue
        names.add(d["name"])
        debounce.append({"name": d["name"], "cname": to_camel(d["name"]), "counts": d["counts"],
                         "threshold": d["threshold"], "satisfies": d.get("satisfies", [])})
        sat(f"debounce:{d['name']}", d.get("satisfies", []))

    # State machine (GEN-042..GEN-045)
    sm = gate["state_machine"]
    states = sm["states"]
    sat("state_machine", sm.get("satisfies", []))
    if sm["initial"] not in states:
        diags.error("GEN-042", display, f"initial state '{sm['initial']}' is not declared")
    for key in ("latched", "valid_output_states"):
        for s in sm.get(key, []):
            if s not in states:
                diags.error("GEN-045", display, f"{key} names undeclared state '{s}'")
    latched = set(sm.get("latched", []))
    transitions = []
    seen_pairs = set()
    for t in sm["transitions"]:
        bad = [x for x in (t["from"], t["to"]) if x not in states]
        if bad:
            diags.error("GEN-042", display, f"transition {t['from']}->{t['to']} references undeclared state(s) {bad}")
            continue
        if t["on"] not in names:
            diags.error("GEN-042", display, f"transition {t['from']}->{t['to']} on unknown event '{t['on']}'")
            continue
        if (t["from"], t["on"]) in seen_pairs:
            diags.error("GEN-044", display, f"two transitions from {t['from']} on '{t['on']}' (nondeterministic)")
        seen_pairs.add((t["from"], t["on"]))
        if t["from"] in latched:
            diags.error("GEN-044", display, f"latched state {t['from']} must not have outgoing transitions")
        if t["from"] == t["to"]:
            diags.error("GEN-044", display, f"self transition on {t['from']} has no effect")
        transitions.append(t)
        sat(f"transition:{t['from']}->{t['to']}", t.get("satisfies", []))
    for s in states:
        if s not in latched and not any(t["from"] == s for t in transitions):
            diags.error("GEN-044", display, f"state {s} has no outgoing transition; declare it latched if intended")
    if sm["initial"] in states:
        reach, todo = {sm["initial"]}, deque([sm["initial"]])
        while todo:
            cur = todo.popleft()
            for t in transitions:
                if t["from"] == cur and t["to"] not in reach:
                    reach.add(t["to"])
                    todo.append(t["to"])
        for s in states:
            if s not in reach:
                diags.error("GEN-043", display, f"state {s} is unreachable from {sm['initial']}")

    # Status output (GEN-046)
    status = None
    so = gate.get("status_output")
    if so is not None:
        sat(f"status_output:{so['message']}", so.get("satisfies", []))
        smsg = messages.get(so["message"])
        if smsg is None or smsg.e2e is not None:
            diags.error("GEN-046", display, f"status message '{so['message']}' must be a declared message without E2E")
        else:
            status = _build_status(smsg, so["map"], inp, len(states), display, diags)

    # Task (GEN-047)
    task = model["task"]
    if task["wcet_budget_us"] >= task["period_ms"] * 1000:
        diags.error("GEN-047", display, f"WCET budget {task['wcet_budget_us']} us >= period {task['period_ms']} ms")
    sat(f"task:{task['name']}", task.get("satisfies", []))

    snake = to_snake(comp["name"])
    by_state = {s: [t for t in transitions if t["from"] == s] for s in states}
    variables = {
        "component": comp,
        "snake": snake,
        "ns": comp["namespace"],
        "inc": inputs.name,
        "messages": [messages[k] for k in sorted(messages)],
        "input": inp,
        "status": status,
        "conditions": CONDITIONS,
        "debounce": debounce,
        "states": states,
        "initial": sm["initial"],
        "latched": latched,
        "valid_output_states": sm["valid_output_states"],
        "transitions_by_state": by_state,
        "task": task,
    }
    outputs = [
        ("types.hpp.j2", f"include/{inputs.name}/{snake}_types.hpp"),
        ("component.hpp.j2", f"include/{inputs.name}/{snake}.hpp"),
        ("component.cpp.j2", f"src/{snake}.cpp"),
        ("task.hpp.j2", f"include/{inputs.name}/{snake}_task.hpp"),
        ("task.cpp.j2", f"src/{snake}_task.cpp"),
        ("codec.hpp.j2", f"include/{inputs.name}/{snake}_codec.hpp"),
        ("codec.cpp.j2", f"src/{snake}_codec.cpp"),
        ("sil_harness.hpp.j2", f"sil/{snake}_sil.hpp"),
        ("sil_test.cpp.j2", f"sil/{snake}_sil_test.cpp"),
        ("CMakeLists.txt.j2", "CMakeLists.txt"),
    ]

    state_set = set(states)
    input_signals = {s.name for s in inp.signals}
    status_signals = {} if status is None else {f["dbc_name"]: f["cname"] for f in status["fields"]}

    def resolve(kind: str, text: str) -> str | None:
        if kind == "state":
            return f"State::k{to_camel(text.lower())}" if text in state_set else None
        if kind == "signal":
            return f"Signal::k{to_camel(text)}" if text in input_signals else None
        if kind == "status_signal":
            return f"StatusSignal::k{status_signals[text]}" if text in status_signals else None
        return None

    return Context(
        systems=[comp["name"]],
        satisfies=satisfies,
        resolve_ref=resolve,
        variables=variables,
        outputs=outputs,
        sil_suite=f"{comp['name']}SilTest",
    )


def _build_status(msg: Message, mapping: dict, inp: Message, n_states: int, display: str,
                  diags: Diagnostics) -> dict | None:
    fields = []
    ok = True
    by_dbc = {(s.dbc_name or s.name): s for s in msg.signals}
    for key in mapping:
        if key not in by_dbc:
            diags.error("GEN-046", display, f"status map names unknown signal '{key}' of {msg.name}")
            ok = False
    for key, sig in by_dbc.items():
        source = mapping.get(key)
        if source is None:
            diags.error("GEN-046", display, f"status signal {msg.name}.{key} is not mapped")
            ok = False
            continue
        field = {"dbc_name": key, "cname": to_camel(sig.name), "signal": sig, "source": source}
        if source == "state":
            if sig.raw_lo > 0 or sig.raw_hi < n_states - 1 or sig.factor != 1.0 or sig.offset != 0.0:
                diags.error("GEN-046", display, f"{key} cannot carry state codes 0..{n_states - 1}")
                ok = False
        elif source == "qualifier":
            if sig.raw_lo > 0 or sig.raw_hi < 1 or sig.factor != 1.0 or sig.offset != 0.0:
                diags.error("GEN-046", display, f"{key} cannot carry qualifier 0..1")
                ok = False
        elif source == "alive_counter":
            if sig.signed or sig.factor != 1.0 or sig.offset != 0.0 or sig.raw_lo != 0 or sig.raw_hi != sig.raw_type_max:
                diags.error("GEN-046", display, f"{key} must be an unsigned full-range counter (factor 1, offset 0)")
                ok = False
            field["modulo"] = sig.raw_type_max + 1
        elif source == "e2e_error_count":
            if sig.signed or sig.factor != 1.0 or sig.offset != 0.0 or sig.raw_lo != 0:
                diags.error("GEN-046", display, f"{key} must be an unsigned counter (factor 1, offset 0)")
                ok = False
            field["saturate"] = sig.raw_hi
        else:
            name = source[len("input."):]
            src = next((s for s in inp.signals if s.name == name), None)
            if src is None:
                diags.error("GEN-046", display, f"{key} maps unknown input signal '{name}'")
                ok = False
                continue
            if src.min < sig.min or src.max > sig.max:
                diags.error("GEN-046", display, f"{key} range [{sig.min}, {sig.max}] cannot hold {name} [{src.min}, {src.max}]")
                ok = False
            field["input_signal"] = name
        fields.append(field)
    return {"message": msg, "fields": fields} if ok else None
