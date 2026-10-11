"""Pattern gpu_stage: statically allocated inference stage on an IComputeBackend, integrity-checked
weights, deadline and plausibility checks, plus an independent ASIL monitor. Rules GEN-050..GEN-057."""
from __future__ import annotations

import hashlib
import math

from tools.gen.errors import Diagnostics
from tools.gen.patterns.codec import to_camel, to_snake

DTYPE_SIZE = {"u8": 1, "f32": 4}
ARENA_ALIGN = 64
ARENA_LIMIT = 16 * 1024 * 1024


def _align(v: int) -> int:
    return (v + ARENA_ALIGN - 1) // ARENA_ALIGN * ARENA_ALIGN


def build(inputs, diags: Diagnostics):
    from tools.gen.project import Context

    model = inputs.model
    display = inputs.model_display
    comp = model["component"]
    graph = model["graph"]
    mon = model["monitor"]
    satisfies: list[tuple[str, str]] = []

    def sat(element: str, refs) -> None:
        satisfies.extend((element, r) for r in refs)

    sat("graph", graph.get("satisfies", []))
    sat(f"monitor:{mon['name']}", mon.get("satisfies", []))

    # GEN-050 / GEN-051 / GEN-052: tensors
    tensors = []
    index: dict[str, int] = {}
    for t in graph["tensors"]:
        if t["name"] in index:
            diags.error("GEN-050", display, f"duplicate tensor '{t['name']}'")
            continue
        elements = math.prod(t["shape"])
        entry = {
            "name": t["name"], "cname": to_camel(t["name"]), "dtype": t["dtype"], "shape": t["shape"],
            "elements": elements, "bytes": elements * DTYPE_SIZE[t["dtype"]], "role": t["role"],
            "index": len(tensors), "file": t.get("file"), "sha256": t.get("sha256"),
        }
        is_weight = t["role"] == "weight"
        if is_weight != ("file" in t) or is_weight != ("sha256" in t):
            diags.error("GEN-051", display, f"tensor '{t['name']}': file and sha256 are required for weights only")
        elif is_weight:
            path = inputs.resolve_input(t["file"], diags)
            if path is not None:
                data = path.read_bytes()
                if hashlib.sha256(data).hexdigest() != t["sha256"]:
                    diags.error("GEN-052", display, f"tensor '{t['name']}': {t['file']} does not match its sha256")
                if len(data) != entry["bytes"]:
                    diags.error("GEN-052", display,
                                f"tensor '{t['name']}': {t['file']} has {len(data)} bytes, shape needs {entry['bytes']}")
        sat(f"tensor:{t['name']}", t.get("satisfies", []))
        index[t["name"]] = len(tensors)
        tensors.append(entry)
    ins = [t for t in tensors if t["role"] == "input"]
    outs = [t for t in tensors if t["role"] == "output"]
    if len(ins) != 1 or len(outs) != 1:
        diags.error("GEN-050", display, f"need exactly one input and one output tensor (have {len(ins)} / {len(outs)})")
        return None
    inp, out = ins[0], outs[0]
    if inp["dtype"] != "u8" or out["dtype"] != "f32":
        diags.error("GEN-053", display, "the input tensor must be u8 and the output tensor f32")

    # GEN-053: dataflow
    nodes = []
    defined = {t["name"] for t in tensors if t["role"] in ("input", "weight")}
    written: set[str] = set()
    names: set[str] = set()
    for n in graph["nodes"]:
        where = f"node '{n['name']}'"
        if n["name"] in names:
            diags.error("GEN-053", display, f"duplicate {where}")
        names.add(n["name"])
        refs = [n["input"], n["output"]] + [n[k] for k in ("weights", "bias") if k in n]
        missing = [r for r in refs if r not in index]
        if missing:
            diags.error("GEN-053", display, f"{where} references unknown tensor(s) {missing}")
            continue
        src, dst = tensors[index[n["input"]]], tensors[index[n["output"]]]
        if n["input"] not in defined:
            diags.error("GEN-053", display, f"{where} reads '{n['input']}' before it is written")
        if dst["role"] in ("input", "weight") or n["output"] in written:
            diags.error("GEN-053", display, f"{where} writes '{n['output']}' which is not a fresh intermediate/output")
        written.add(n["output"])
        defined.add(n["output"])
        op = {"name": n["name"], "op": n["op"], "input": src["index"], "output": dst["index"],
              "weights": None, "bias": None, "in_features": 0, "out_features": 0, "scale": 0.0, "shift": 0.0}
        extra = set(n) - {"name", "op", "input", "output"}
        if n["op"] == "normalize":
            if extra != {"scale", "shift"}:
                diags.error("GEN-053", display, f"{where}: normalize needs exactly scale and shift")
            elif src["dtype"] != "u8" or dst["dtype"] != "f32" or src["elements"] != dst["elements"]:
                diags.error("GEN-053", display, f"{where}: normalize maps u8[n] -> f32[n]")
            else:
                op.update(scale=float(n["scale"]), shift=float(n["shift"]))
        elif n["op"] == "dense":
            if extra != {"weights", "bias"}:
                diags.error("GEN-053", display, f"{where}: dense needs exactly weights and bias")
            else:
                w, b = tensors[index[n["weights"]]], tensors[index[n["bias"]]]
                ok = (src["dtype"] == dst["dtype"] == w["dtype"] == b["dtype"] == "f32"
                      and w["role"] == b["role"] == "weight"
                      and w["shape"] == [src["elements"], dst["elements"]] and b["elements"] == dst["elements"])
                if not ok:
                    diags.error("GEN-053", display, f"{where}: dense needs f32 weights [in, out] and bias [out]")
                op.update(weights=w["index"], bias=b["index"], in_features=src["elements"], out_features=dst["elements"])
        else:
            if extra:
                diags.error("GEN-053", display, f"{where}: {n['op']} takes no parameters")
            elif not (src["dtype"] == dst["dtype"] == "f32" and src["elements"] == dst["elements"]):
                diags.error("GEN-053", display, f"{where}: {n['op']} maps f32[n] -> f32[n]")
        nodes.append(op)
    for t in tensors:
        if t["role"] in ("intermediate", "output") and t["name"] not in written:
            diags.error("GEN-053", display, f"tensor '{t['name']}' is never written")

    # GEN-054 classes
    classes = graph["classes"]
    if len(classes) != out["elements"]:
        diags.error("GEN-054", display, f"{len(classes)} classes but output '{out['name']}' has {out['elements']} elements")

    # GEN-055 plausibility
    checks = []
    for i, c in enumerate(graph["plausibility"]):
        where = f"plausibility[{i}] ({c['check']})"
        if c["tensor"] != out["name"]:
            diags.error("GEN-055", display, f"{where} must check the output tensor '{out['name']}'")
        params = set(c) - {"check", "tensor", "satisfies"}
        need = {"finite": set(), "range": {"min", "max"}, "sum": {"value", "tolerance"}}[c["check"]]
        if params != need:
            diags.error("GEN-055", display, f"{where} needs exactly {sorted(need) or 'no parameters'}")
            continue
        if c["check"] == "range" and c["min"] > c["max"]:
            diags.error("GEN-055", display, f"{where}: min > max")
        checks.append(c)
        sat(f"plausibility:{c['check']}", c.get("satisfies", []))
    if not checks or checks[0]["check"] != "finite":
        diags.error("GEN-055", display, "the first plausibility check must be 'finite' (NaN defeats range checks)")

    # GEN-056 arena
    arena = sum(_align(t["bytes"]) for t in tensors)
    if arena > ARENA_LIMIT:
        diags.error("GEN-056", display, f"graph needs {arena} bytes, limit is {ARENA_LIMIT}")

    # GEN-057 monitor timing
    if mon["timeout_ms"] < graph["deadline_ms"]:
        diags.error("GEN-057", display, f"monitor timeout {mon['timeout_ms']} ms < stage deadline {graph['deadline_ms']} ms")

    snake = to_snake(comp["name"])
    weights = [t for t in tensors if t["role"] == "weight"]
    variables = {
        "component": comp,
        "snake": snake,
        "ns": comp["namespace"],
        "inc": inputs.name,
        "tensors": tensors,
        "nodes": nodes,
        "input": inp,
        "output": out,
        "weights": weights,
        "classes": [{"name": c, "cname": to_camel(c), "index": i} for i, c in enumerate(classes)],
        "checks": checks,
        "deadline_ms": graph["deadline_ms"],
        "reference_tolerance": graph["reference_tolerance"],
        "backends": graph["backends"],
        "arena_bytes": arena,
        "monitor": mon,
        "monitor_snake": to_snake(mon["name"]),
    }
    outputs = [
        ("types.hpp.j2", f"include/{inputs.name}/{snake}_types.hpp"),
        ("stage.hpp.j2", f"include/{inputs.name}/{snake}.hpp"),
        ("stage.cpp.j2", f"src/{snake}.cpp"),
        ("monitor.hpp.j2", f"include/{inputs.name}/{variables['monitor_snake']}.hpp"),
        ("monitor.cpp.j2", f"src/{variables['monitor_snake']}.cpp"),
        ("sil_harness.hpp.j2", f"sil/{snake}_sil.hpp"),
        ("sil_test.cpp.j2", f"sil/{snake}_sil_test.cpp"),
        ("CMakeLists.txt.j2", "CMakeLists.txt"),
    ]
    class_names = {c: i for i, c in enumerate(classes)}
    weight_names = {t["name"]: t["cname"] for t in weights}

    def resolve(kind: str, text: str) -> str | None:
        if kind == "class":
            return f"kClass{to_camel(text)}" if text in class_names else None
        if kind == "weight":
            return f"Weight::k{weight_names[text]}" if text in weight_names else None
        return None

    return Context(
        systems=[comp["name"], mon["name"]],
        satisfies=satisfies,
        resolve_ref=resolve,
        variables=variables,
        outputs=outputs,
        sil_suite=f"{comp['name']}SilTest",
    )
