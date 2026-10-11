"""Requirement checks: uniqueness, EARS syntax, coverage and references (GEN-010..GEN-016)."""
from __future__ import annotations

import re
from dataclasses import dataclass

from tools.gen.errors import Diagnostics
from tools.gen.gherkin import Scenario

# EARS templates (Mavin et al.). The system name is the component name.
_EARS = {
    "ubiquitous": r"^The {sys} shall .+",
    "event": r"^When .+, the {sys} shall .+",
    "state": r"^While .+, the {sys} shall .+",
    "unwanted": r"^If .+, then the {sys} shall .+",
    "optional": r"^Where .+, the {sys} shall .+",
}


@dataclass(frozen=True)
class Requirement:
    id: str
    asil: str
    ears: str
    text: str
    verification: tuple[str, ...]


def load(doc: dict, component: str, systems: list[str], path: str, diags: Diagnostics) -> dict[str, Requirement]:
    """`systems` are the names a requirement sentence may use as its subject (component and its parts)."""
    if doc["component"] != component:
        diags.error("GEN-012", path, f"requirements are for '{doc['component']}', model component is '{component}'")
    reqs: dict[str, Requirement] = {}
    for r in doc["requirements"]:
        text = " ".join(r["text"].split())
        if r["id"] in reqs:
            diags.error("GEN-010", path, f"duplicate requirement id {r['id']}")
            continue
        system = "(?:" + "|".join(re.escape(n) for n in systems) + ")"
        if not re.match(_EARS[r["ears"]].format(sys=system), text):
            diags.error("GEN-011", path, f"{r['id']}: text does not follow the EARS '{r['ears']}' template")
        reqs[r["id"]] = Requirement(r["id"], r["asil"], r["ears"], text, tuple(r["verification"]))
    return reqs


def tag_requirements(sc: Scenario) -> list[str]:
    return [t[1:] for t in sc.tags if t.startswith("@REQ-")]


def check_scenarios(reqs: dict[str, Requirement], scenarios: list[Scenario], diags: Diagnostics) -> None:
    covered: set[str] = set()
    names: dict[str, Scenario] = {}
    for sc in scenarios:
        if sc.name in names:
            diags.error("GEN-024", sc.path, f"duplicate scenario name '{sc.name}'", sc.line)
        names[sc.name] = sc
        tagged = tag_requirements(sc)
        if not tagged:
            diags.error("GEN-014", sc.path, f"scenario '{sc.name}' has no @REQ-... tag", sc.line)
        for rid in tagged:
            if rid not in reqs:
                diags.error("GEN-014", sc.path, f"scenario '{sc.name}' references unknown {rid}", sc.line)
            covered.add(rid)
    for rid, req in sorted(reqs.items()):
        if "sil" in req.verification and rid not in covered:
            diags.error("GEN-013", "requirements", f"{rid} is verified by SIL but no scenario covers it")


def check_satisfies(reqs: dict[str, Requirement], satisfies: list[tuple[str, str]], path: str,
                    diags: Diagnostics) -> None:
    """`satisfies` is a list of (model element, requirement id)."""
    satisfied = set()
    for element, rid in satisfies:
        if rid not in reqs:
            diags.error("GEN-015", path, f"{element} satisfies unknown requirement {rid}")
        satisfied.add(rid)
    for rid in sorted(set(reqs) - satisfied):
        diags.error("GEN-016", path, f"{rid} is not satisfied by any model element")
