"""Pattern can_system: an ASIL D function decomposed into two can_function nodes on one bus.

  controller B(D) --command--> monitor B(D) --approval--> actuator ECU (assumption of use)

The system project generates no product code. It checks that the nodes fit together and that
the safety goals' fault tolerant time intervals are met by design, and it generates the system
SIL that runs both generated nodes against the plant with faults injected on their outputs:
  - GEN-065: per safety goal, every diagnostic mechanism's fault detection time (from the monitor
    model) plus the actuator's reaction time (AoU) fits into the FTTI
  - GEN-066: the nodes are valid projects, share identical message definitions, the monitor
    receives the controller's command with the same E2E data id, and both run at one cycle"""
from __future__ import annotations

from tools.gen import loader
from tools.gen.errors import Diagnostics
from tools.gen.features import load_features
from tools.gen.patterns.codec import to_snake
from tools.gen.render import REPO_ROOT, TEMPLATE_DIR, tool_files


def _signature(msg) -> tuple:
    return (msg.can_id, msg.payload_length,
            tuple(sorted((s.dbc_name or s.name, s.start_bit, s.length, s.big_endian, s.signed, s.factor, s.offset)
                         for s in msg.signals)))


def _node(role: str, project: str, display: str, diags: Diagnostics):
    from tools.gen import project as proj

    doc = loader.load_yaml(REPO_ROOT / "ea_projects.yaml", diags, "ea_projects.yaml")
    entry = next((p for p in (doc or {}).get("projects", []) if p["name"] == project), None)
    if entry is None:
        diags.error("GEN-066", display, f"{role}: unknown project '{project}' (not in ea_projects.yaml)")
        return None
    sub = Diagnostics()
    built = proj.build(project, entry["model"], sub)
    if built is None or not sub.ok():
        diags.error("GEN-066", display, f"{role}: project '{project}' is not valid on its own; run python3 -m tools.gen")
        return None
    inputs, ctx = built
    if inputs.pattern != "can_function":
        diags.error("GEN-066", display, f"{role}: project '{project}' must use pattern can_function")
        return None
    return {"project": project, "inputs": inputs, "ctx": ctx, "v": ctx.variables}


def build(inputs, diags: Diagnostics):
    from tools.gen.project import Context

    model = inputs.model
    display = inputs.model_display
    comp = model["component"]
    ctrl = _node("controller", model["nodes"]["controller"], display, diags)
    mon = _node("monitor", model["nodes"]["monitor"], display, diags)
    if ctrl is None or mon is None:
        return None
    features = load_features()
    mon_feature = features[mon["v"]["feature"]]
    ctrl_feature = features[ctrl["v"]["feature"]]
    satisfies: list[tuple[str, str]] = []

    # GEN-066: one bus, one cycle, one definition per message, matching E2E on the command.
    if ctrl["v"]["cycle_ms"] != mon["v"]["cycle_ms"]:
        diags.error("GEN-066", display, f"controller cycle {ctrl['v']['cycle_ms']} ms != monitor cycle {mon['v']['cycle_ms']} ms")
    c_msgs = {m.name: m for m in ctrl["v"]["messages"]}
    m_msgs = {m.name: m for m in mon["v"]["messages"]}
    for name in sorted(set(c_msgs) & set(m_msgs)):
        if _signature(c_msgs[name]) != _signature(m_msgs[name]):
            diags.error("GEN-066", display, f"message {name} is defined differently in the controller and monitor DBC")
    for cid in sorted(set(m.can_id for m in c_msgs.values()) & set(m.can_id for m in m_msgs.values())):
        names = {m.name for m in c_msgs.values() if m.can_id == cid} | {m.name for m in m_msgs.values() if m.can_id == cid}
        if len(names) > 1:
            diags.error("GEN-066", display, f"CAN id {cid} carries different messages: {sorted(names)}")
    command = next((t for t in ctrl["v"]["tx"] if any(r["cname"] == t["cname"] for r in mon["v"]["rx"])), None)
    if command is None:
        diags.error("GEN-066", display, "the monitor receives none of the controller's tx messages")
        return None
    mon_rx = next(r for r in mon["v"]["rx"] if r["cname"] == command["cname"])
    if mon_rx["e2e"]["data_id"] != command["e2e"]["data_id"]:
        diags.error("GEN-066", display, f"E2E data id of {command['cname']}: controller {command['e2e']['data_id']:#06x}, "
                                        f"monitor {mon_rx['e2e']['data_id']:#06x}")
    if not diags.ok():
        return None

    # GEN-065: FDTI(mechanism) + actuator reaction <= FTTI for every safety goal.
    mechanisms = getattr(mon_feature, "MECHANISMS", {})
    bindings = mon["v"]["b"]
    cycle = mon["v"]["cycle_ms"]

    def timeout_of(iface: str) -> float:
        entry = bindings[iface]["entry"]
        return float(entry["timeout_cycles"] * cycle)

    reaction = model["actuator"]["reaction_ms"]
    goals: dict[str, dict] = {}
    for sg in model["safety_goals"]:
        rid = sg["requirement"]
        if rid in goals:
            diags.error("GEN-065", display, f"{rid}: listed twice")
            continue
        worst = 0.0
        for mech in sg["mechanisms"]:
            fn = mechanisms.get(mech)
            if fn is None:
                diags.error("GEN-065", display, f"{rid}: monitor feature '{mon_feature.NAME}' has no mechanism '{mech}' "
                                                f"(known: {sorted(mechanisms)})")
                continue
            fdti = float(fn(mon["v"]["cal"], timeout_of, cycle))
            worst = max(worst, fdti)
            if fdti + reaction > sg["ftti_ms"] + 1e-9:
                diags.error("GEN-065", display, f"{rid}: mechanism '{mech}' detects in {fdti:g} ms; with the actuator "
                                                f"reaction of {reaction} ms this exceeds the FTTI of {sg['ftti_ms']} ms")
        goals[rid] = {"id": rid, "ftti_ms": sg["ftti_ms"], "mechanisms": sg["mechanisms"], "worst_fdti_ms": worst,
                      "budget_ms": sg["ftti_ms"] - reaction}
        satisfies.append((f"safety_goal:{rid}", rid))
    satisfies.extend(("allocation", r) for r in model.get("allocation", {}).get("satisfies", []))
    satisfies.extend(("actuator", r) for r in model["actuator"].get("satisfies", []))
    for role, f in (("controller", ctrl_feature), ("monitor", mon_feature)):
        if getattr(f, "PLANT_TABLES", None) is None:
            diags.error("GEN-066", display, f"{role} feature '{f.NAME}' declares no PLANT_TABLES for fault injection")
    if not diags.ok():
        return None

    def resolve_ref(kind: str, text: str) -> str | None:
        if kind == "safety_goal" and text in goals:
            return f"{goals[text]['budget_ms']}.0"
        return None

    def node_vars(n, feature):
        v = n["v"]
        return {"project": n["project"], "inc": v["inc"], "snake": v["snake"], "ns": v["ns"], "cls": v["component"]["name"],
                "asil": v["component"]["asil"], "feature": v["feature"], "observer_header": feature.OBSERVER[0],
                "observer_type": feature.OBSERVER[1], "tables": getattr(feature, "PLANT_TABLES", None)}

    cmd_e2e = command["e2e"]
    mon_tx = mon["v"]["tx"][0]
    variables = {
        "component": comp, "snake": to_snake(comp["name"]), "ns": comp["namespace"], "inc": inputs.name,
        "controller": node_vars(ctrl, ctrl_feature), "monitor": node_vars(mon, mon_feature),
        "feature": ctrl["v"]["feature"],
        "command": {"name": command["cname"], "can_id": command["msg"].can_id, "e2e": cmd_e2e,
                    "length": command["msg"].payload_length},
        "monitor_msg": {"name": mon_tx["cname"], "can_id": mon_tx["msg"].can_id, "e2e": mon_tx["e2e"],
                        "length": mon_tx["msg"].payload_length},
        "goals": [goals[k] for k in sorted(goals)], "reaction_ms": reaction, "cycle_ms": cycle,
    }
    snake = variables["snake"]
    outputs = [
        ("sil_harness.hpp.j2", f"sil/{snake}_sil.hpp"),
        ("sil_test.cpp.j2", f"sil/{snake}_sil_test.cpp"),
        ("CMakeLists.txt.j2", "CMakeLists.txt"),
    ]
    # Node inputs and their tool files are inputs of this project (hash in every header).
    inputs.files.extend(ctrl["inputs"].files + mon["inputs"].files)
    node_tools = tool_files("can_function") + ctrl["ctx"].extra_tool_files + mon["ctx"].extra_tool_files
    node_tools += sorted((TEMPLATE_DIR / "features" / ctrl["v"]["feature"]).rglob("harness.hpp.j2"))
    catalogues = ["sil/steps/can_function.yaml", "sil/steps/monitored_actuator.yaml", "sil/steps/can_system.yaml",
                  f"sil/steps/features/{ctrl['v']['feature']}.yaml", f"sil/steps/features/{mon['v']['feature']}.yaml"]
    return Context(
        systems=[comp["name"]],
        satisfies=satisfies,
        resolve_ref=resolve_ref,
        variables=variables,
        outputs=outputs,
        sil_suite=f"{comp['name']}SilTest",
        step_catalogues=list(dict.fromkeys(catalogues)),
        extra_tool_files=sorted(set(node_tools)),
    )
