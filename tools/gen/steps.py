"""Binding of Gherkin steps to the controlled step catalogue (GEN-021..GEN-023)."""
from __future__ import annotations

import pathlib
import re
from dataclasses import dataclass
from typing import Callable

import jinja2

from tools.gen import loader, schema
from tools.gen.errors import Diagnostics
from tools.gen.gherkin import Scenario, Step

STEPS_DIR = pathlib.Path(__file__).resolve().parents[2] / "sil" / "steps"

# Resolves a model reference (kind, text) to a C++ expression, or None if it does not exist.
RefResolver = Callable[[str, str], "str | None"]


@dataclass(frozen=True)
class CatalogueStep:
    id: str
    keywords: tuple[str, ...]
    regex: re.Pattern
    types: tuple[tuple[str, str], ...]
    cpp: str


@dataclass(frozen=True)
class BoundStep:
    step: Step
    catalogue_id: str
    cpp: str


@dataclass(frozen=True)
class BoundScenario:
    scenario: Scenario
    steps: tuple[BoundStep, ...]


def camel(text: str) -> str:
    """VALID -> Valid, DEADLINE_MISS -> DeadlineMiss, nearest_distance -> NearestDistance."""
    parts = re.split(r"[_\s]+", text)
    return "".join(p[:1].upper() + p[1:].lower() if p.isupper() else p[:1].upper() + p[1:] for p in parts if p)


_ENV = jinja2.Environment(undefined=jinja2.StrictUndefined, autoescape=False, keep_trailing_newline=False)
_ENV.filters["camel"] = camel


def load_catalogues(rel_paths: list[str], diags: Diagnostics) -> list[CatalogueStep]:
    """Loads one or more catalogues (repo-relative paths); step ids must be unique across them."""
    out: list[CatalogueStep] = []
    for rel in rel_paths:
        path = STEPS_DIR.parent.parent / rel
        doc = loader.load_yaml(path, diags, rel)
        if doc is None or not schema.validate(doc, "steps.schema.json", diags, rel):
            continue
        for entry in doc["steps"]:
            regex = re.compile(entry["pattern"])
            types = entry.get("types", {})
            groups = set(regex.groupindex)
            if groups != set(types):
                diags.error("GEN-002", rel, f"{entry['id']}: named groups {sorted(groups)} != types {sorted(types)}")
                continue
            out.append(CatalogueStep(entry["id"], tuple(entry["keywords"]), regex, tuple(sorted(types.items())), entry["cpp"]))
    ids = [s.id for s in out]
    if len(ids) != len(set(ids)):
        diags.error("GEN-002", ", ".join(rel_paths), "duplicate step ids")
    return out


def _number_literal(text: str) -> str:
    return text if ("." in text or "e" in text.lower()) else text + ".0"


def _render_param(kind: str, text: str, resolve: RefResolver) -> str | None:
    if kind == "int":
        return str(int(text))
    if kind == "number":
        return _number_literal(text)
    if kind == "number_or_nan":
        return "std::numeric_limits<float>::quiet_NaN()" if text == "NaN" else _number_literal(text) + "F"
    if kind.startswith("ref:"):
        return resolve(kind[4:], text)
    if kind.startswith("enum:"):
        allowed = [a.strip() for a in kind[len("enum:["):-1].split(",")]
        return text if text in allowed else None
    return None


def bind(scenarios: list[Scenario], catalogue: list[CatalogueStep], resolve: RefResolver,
         diags: Diagnostics) -> list[BoundScenario]:
    bound: list[BoundScenario] = []
    for sc in scenarios:
        steps: list[BoundStep] = []
        ok = True
        for st in sc.steps:
            matches = [(c, m) for c in catalogue for m in [c.regex.match(st.text)] if m]
            if len(matches) != 1:
                what = "no" if not matches else f"{len(matches)} ({', '.join(c.id for c, _ in matches)})"
                diags.error("GEN-021", sc.path, f"step '{st.text}' matches {what} catalogue entries", st.line)
                ok = False
                continue
            entry, m = matches[0]
            if st.kind not in entry.keywords:
                diags.error("GEN-022", sc.path,
                            f"step '{st.text}' ({entry.id}) is a {'/'.join(entry.keywords)} step, used as {st.kind}",
                            st.line)
                ok = False
                continue
            params = {}
            for name, kind in entry.types:
                rendered = _render_param(kind, m.group(name), resolve)
                if rendered is None:
                    diags.error("GEN-023", sc.path, f"'{m.group(name)}' is not a valid {kind} in '{st.text}'", st.line)
                    ok = False
                params[name] = rendered
            if ok:
                steps.append(BoundStep(st, entry.id, _ENV.from_string(entry.cpp).render(**params)))
        if ok:
            bound.append(BoundScenario(sc, tuple(steps)))
    return bound
