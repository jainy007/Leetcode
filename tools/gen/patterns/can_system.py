"""Pattern can_system: an ASIL D function decomposed into two can_function nodes on one bus.

  controller B(D) --command--> monitor B(D) --approval--> actuator ECU (assumption of use)

The system project generates no product code. It checks that the nodes fit together and that
the safety goals' fault tolerant time intervals are met by design, and it generates the system
SIL that runs both generated nodes against the plant with faults injected on their outputs:
  - GEN-065: per safety goal, every diagnostic mechanism's fault detection time (from the monitor
    model) plus the actuator's reaction time (AoU) fits into the FTTI
  - GEN-066: the nodes are valid projects, share identical message definitions, the monitor
    receives the controller's command with the same E2E data id, and both run at one cycle
  - GEN-067: the HARA rates every hazardous event with S, E and C; the ASIL follows from
    ISO 26262-3 table 4, and each safety goal carries the highest ASIL of its hazardous events
  - GEN-068: no minimal cut set of a safety goal's fault tree lies in elements that are not rated
    for the goal's ASIL (single points need the full ASIL, decomposed pairs the X(Y) rating)
  - GEN-069: each safety goal's FTTI does not exceed the FTTI derived from its hazardous events'
    situations by the kinematic models of tools/gen/safety.py"""
from __future__ import annotations

from tools.gen import loader, safety
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
    analysis = _safety_analysis(inputs, model, ctrl, mon, display, diags)
    if analysis is None or not diags.ok():
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
        "safety": analysis, "inputs_hara": model["inputs"]["hara"], "inputs_fta": model["inputs"]["fta"],
    }
    snake = variables["snake"]
    outputs = [
        ("sil_harness.hpp.j2", f"sil/{snake}_sil.hpp"),
        ("sil_test.cpp.j2", f"sil/{snake}_sil_test.cpp"),
        ("CMakeLists.txt.j2", "CMakeLists.txt"),
        ("safety_analysis.md.j2", "safety_analysis.md"),
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


def _load(inputs, rel: str, schema_name: str, diags: Diagnostics):
    from tools.gen.project import load_document

    path = inputs.resolve_input(rel, diags)
    if path is None:
        return None
    return load_document(path, schema_name, inputs.display(path), diags)


def _safety_analysis(inputs, model: dict, ctrl: dict, mon: dict, display: str, diags: Diagnostics) -> dict | None:
    """HARA (GEN-067), FTTI derivation (GEN-069) and fault tree check (GEN-068)."""
    hara = _load(inputs, model["inputs"]["hara"], "hara.schema.json", diags)
    fta = _load(inputs, model["inputs"]["fta"], "fta.schema.json", diags)
    req_path = inputs.dir / model["inputs"]["requirements"]
    reqs = {r["id"]: r for r in (loader.load_yaml(req_path, diags, inputs.display(req_path)) or {}).get("requirements", [])}
    if hara is None or fta is None:
        return None
    goal_ids = [sg["requirement"] for sg in model["safety_goals"]]
    ftti_model = {sg["requirement"]: sg["ftti_ms"] for sg in model["safety_goals"]}
    hazards = {h["id"]: h for h in hara["hazards"]}

    # GEN-067: ASIL from S, E, C; hazardous events with an ASIL need a safety goal of that ASIL.
    events, by_goal = [], {g: [] for g in goal_ids}
    for he in hara["hazardous_events"]:
        level = safety.asil(he["severity"]["class"], he["exposure"]["class"], he["controllability"]["class"])
        where = f"{he['id']}"
        if he["hazard"] not in hazards:
            diags.error("GEN-067", display, f"{where}: unknown hazard '{he['hazard']}'")
        goal = he.get("safety_goal")
        if level != "QM" and goal is None:
            diags.error("GEN-067", display, f"{where}: ASIL {level} hazardous event without a safety goal")
        if goal is not None and goal not in by_goal:
            diags.error("GEN-067", display, f"{where}: safety goal {goal} is not a safety goal of the system model")
        if goal is not None and level == "QM":
            diags.error("GEN-067", display, f"{where}: rated QM but assigned to safety goal {goal}")
        entry = {"he": he, "hazard": hazards.get(he["hazard"]), "asil": level, "ftti": []}
        events.append(entry)
        if goal in by_goal:
            by_goal[goal].append(entry)
    for g, hes in by_goal.items():
        if not hes:
            diags.error("GEN-067", display, f"{g}: safety goal without a hazardous event in the HARA")
            continue
        top = max((e["asil"] for e in hes), key=safety.asil_rank)
        stated = reqs.get(g, {}).get("asil")
        if stated != top:
            diags.error("GEN-067", display, f"{g}: requirement ASIL {stated} != highest ASIL {top} of its hazardous events")

    # GEN-069: derived FTTI per situation; the model's FTTI must not exceed the smallest.
    ramp = model["actuator"]["ramp_out_mps3"]
    derived: dict[str, int] = {}
    for e in events:
        spec = e["he"].get("ftti")
        if spec is None:
            continue
        needed = safety.MODELS[spec["model"]][1]
        for i, sit in enumerate(spec["situations"]):
            params = {**hara.get("ftti_defaults", {}), **sit}
            missing = sorted(needed - set(params))
            if missing:
                diags.error("GEN-069", display, f"{e['he']['id']} situation {i + 1}: missing {', '.join(missing)}")
                continue
            if spec["model"] == "rear_end_by_follower" and params["ramp_out_mps3"] != ramp:
                diags.error("GEN-069", display, f"{e['he']['id']}: ramp-out {params['ramp_out_mps3']} m/s3 differs from "
                                                f"the actuator's {ramp} m/s3")
            ms = safety.derive_ftti_ms(spec["model"], params)
            if ms is None:
                diags.error("GEN-069", display, f"{e['he']['id']} situation {i + 1}: collision even without a fault; "
                                                f"not a valid reference situation")
                continue
            e["ftti"].append({"params": params, "ms": ms})
            g = e["he"].get("safety_goal")
            if g is not None:
                derived[g] = min(derived.get(g, ms), ms)
    for g in goal_ids:
        if g not in derived:
            diags.error("GEN-069", display, f"{g}: no FTTI derivation in the HARA")
        elif ftti_model[g] > derived[g]:
            diags.error("GEN-069", display, f"{g}: FTTI {ftti_model[g]} ms exceeds the derived FTTI of {derived[g]} ms")

    # GEN-068: fault trees.
    elements = fta["elements"]
    node_asil = {"controller": ctrl["v"]["component"]["asil"], "monitor": mon["v"]["component"]["asil"]}
    for name, el in elements.items():
        if "node" in el and el["asil"] != node_asil[el["node"]]:
            diags.error("GEN-068", display, f"element {name}: ASIL {el['asil']} != {el['node']} component ASIL "
                                            f"{node_asil[el['node']]}")
    for eid, ev in fta["events"].items():
        if ev["element"] not in elements:
            diags.error("GEN-068", display, f"event {eid}: unknown element '{ev['element']}'")
    trees = {t["safety_goal"]: t for t in fta["trees"]}
    fault_trees = []
    for g in goal_ids:
        t = trees.get(g)
        if t is None:
            diags.error("GEN-068", display, f"{g}: no fault tree")
            continue
        if t["gate"] not in fta["gates"]:
            diags.error("GEN-068", display, f"{g}: unknown gate '{t['gate']}'")
            continue
        try:
            cuts = safety.minimal_cut_sets(t["gate"], fta["events"], fta["gates"])
        except (KeyError, ValueError) as exc:
            diags.error("GEN-068", display, f"{g}: invalid fault tree ({exc})")
            continue
        goal_asil = reqs.get(g, {}).get("asil", "D")
        rows = []
        for cut in cuts:
            els = sorted({fta["events"][ev]["element"] for ev in cut if ev in fta["events"]})
            ratings = [elements[x]["asil"] for x in els if x in elements]
            full = any(safety.asil_rank(r) >= safety.asil_rank(goal_asil) for r in ratings)
            decomposed = len(els) >= 2 and all("(" in r and safety.asil_rank(r.split("(")[1].rstrip(")")) >=
                                               safety.asil_rank(goal_asil) for r in ratings)
            ok = full or decomposed
            if not ok:
                where = ", ".join(x + " (" + elements[x]["asil"] + ")" for x in els)
                diags.error("GEN-068", display, f"{g} (ASIL {goal_asil}): cut set {sorted(cut)} lies in {where}, "
                                                f"not rated for it")
            rows.append({"events": sorted(cut), "elements": els, "ratings": ratings,
                         "basis": "element rated for the goal" if full else "decomposition" if decomposed else "VIOLATION"})
        fault_trees.append({"goal": g, "asil": goal_asil, "top": t["top"], "cuts": rows})
    return {"hara": hara, "events": events, "derived": derived, "ftti_model": ftti_model, "fta": fta,
            "trees": fault_trees, "goal_asil": {g: reqs.get(g, {}).get("asil") for g in goal_ids}, "reqs": reqs}
