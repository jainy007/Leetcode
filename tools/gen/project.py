"""Per-project orchestration: load -> validate -> check rules -> bind steps -> render."""
from __future__ import annotations

import pathlib
import re
from dataclasses import dataclass, field
from typing import Any, Callable

from tools.gen import GENERATOR_NAME, GENERATOR_VERSION, gherkin, loader, render, requirements, schema, steps
from tools.gen.errors import Diagnostics
from tools.gen.gherkin import Scenario
from tools.gen.render import REPO_ROOT


@dataclass
class Inputs:
    """Everything loaded for one project, shared with the pattern."""

    name: str
    pattern: str
    model: dict
    dir: pathlib.Path
    model_display: str
    requirements: dict[str, requirements.Requirement] = field(default_factory=dict)
    scenarios: list[Scenario] = field(default_factory=list)
    files: list[pathlib.Path] = field(default_factory=list)

    def display(self, path: pathlib.Path) -> str:
        return render.rel_display(path)

    def resolve_input(self, rel: str, diags: Diagnostics) -> pathlib.Path | None:
        """GEN-003: a referenced input must exist and stay inside the project directory."""
        path = (self.dir / rel).resolve()
        if self.dir.resolve() not in path.parents or not path.is_file():
            diags.error("GEN-003", self.model_display, f"input file '{rel}' does not exist inside {self.display(self.dir)}")
            return None
        self.files.append(path)
        return path


@dataclass
class Context:
    """What a pattern hands back after building its IR."""

    systems: list[str]  # names allowed as EARS subject
    satisfies: list[tuple[str, str]]  # (model element, requirement id)
    resolve_ref: Callable[[str, str], "str | None"]
    variables: dict[str, Any]
    outputs: list[tuple[str, str]]  # (template, output path relative to project output dir)
    sil_suite: str


def _check_finite(doc: Any, display: str, diags: Diagnostics, where: str = "") -> None:
    if isinstance(doc, float) and (doc != doc or doc in (float("inf"), float("-inf"))):
        diags.error("GEN-001", display, f"{where or '<root>'}: non-finite numbers are not allowed")
    elif isinstance(doc, dict):
        for k, v in doc.items():
            _check_finite(v, display, diags, f"{where}/{k}")
    elif isinstance(doc, list):
        for i, v in enumerate(doc):
            _check_finite(v, display, diags, f"{where}/{i}")


def load_document(path: pathlib.Path, schema_name: str, display: str, diags: Diagnostics) -> Any | None:
    doc = loader.load_yaml(path, diags, display)
    if doc is None:
        return None
    _check_finite(doc, display, diags)
    if not schema.validate(doc, schema_name, diags, display):
        return None
    return doc


def test_name(index: int, scenario: Scenario) -> str:
    base = re.sub(r"#\d+$", "", scenario.name)
    words = re.findall(r"[A-Za-z0-9]+", base)
    ident = "".join(w[:1].upper() + w[1:] for w in words)[:60]
    suffix = ""
    m = re.search(r"#(\d+)$", scenario.name)
    if m:
        suffix = f"_Ex{m.group(1)}"
    return f"S{index:02d}_{ident}{suffix}"


def build(name: str, model_rel: str, diags: Diagnostics) -> tuple[Inputs, Context] | None:
    from tools.gen.patterns import PATTERNS

    model_path = REPO_ROOT / model_rel  # an absolute model_rel (tests) is used as is
    display = render.rel_display(model_path)
    if not model_path.is_file():
        diags.error("GEN-003", "ea_projects.yaml", f"model '{model_rel}' does not exist")
        return None
    raw = loader.load_yaml(model_path, diags, display)
    if raw is None:
        return None
    pattern = raw.get("pattern") if isinstance(raw, dict) else None
    if pattern not in PATTERNS:
        diags.error("GEN-002", display, f"unknown pattern {pattern!r} (known: {sorted(PATTERNS)})")
        return None
    model = load_document(model_path, f"model_{pattern}.schema.json", display, diags)
    if model is None:
        return None
    inputs = Inputs(name=name, pattern=pattern, model=model, dir=model_path.parent, model_display=display)
    inputs.files.append(model_path)

    module = PATTERNS[pattern]
    ctx = module.build(inputs, diags)
    if ctx is None:
        return None

    req_path = inputs.resolve_input(model["inputs"]["requirements"], diags)
    if req_path is not None:
        doc = load_document(req_path, "requirements.schema.json", inputs.display(req_path), diags)
        if doc is not None:
            inputs.requirements = requirements.load(doc, model["component"]["name"], ctx.systems,
                                                    inputs.display(req_path), diags)
    for rel in model["inputs"]["features"]:
        fpath = inputs.resolve_input(rel, diags)
        if fpath is not None:
            feature = gherkin.parse(loader.read_text(fpath), inputs.display(fpath), diags)
            if feature is not None:
                inputs.scenarios.extend(feature.scenarios)

    requirements.check_scenarios(inputs.requirements, inputs.scenarios, diags)
    requirements.check_satisfies(inputs.requirements, ctx.satisfies, display, diags)
    return inputs, ctx


def generate(name: str, model_rel: str, output_rel: str, diags: Diagnostics) -> dict[str, str]:
    """Returns {repo-relative path: content}; empty with diagnostics on failure."""
    built = build(name, model_rel, diags)
    if built is None or not diags.ok():
        return {}
    inputs, ctx = built
    catalogue = steps.load_catalogue(inputs.pattern, diags)
    bound = steps.bind(inputs.scenarios, catalogue, ctx.resolve_ref, diags)
    if not diags.ok():
        return {}

    digest = render.inputs_digest(inputs.files + render.tool_files(inputs.pattern))
    header = render.header_lines(inputs.pattern, name, digest)
    tests = []
    for idx, b in enumerate(bound, start=1):
        sc = b.scenario
        tests.append({
            "name": test_name(idx, sc),
            "scenario": sc.name,
            "feature": sc.path,
            "line": sc.line,
            "requirements": requirements.tag_requirements(sc),
            "example": [list(kv) for kv in sc.example],
            "steps": [{"text": f"{s.step.keyword} {s.step.text}", "catalogue": s.catalogue_id, "cpp": s.cpp}
                      for s in b.steps],
        })

    env = render.make_env()
    variables = dict(ctx.variables)
    variables.update(project=name, tests=tests, sil_suite=ctx.sil_suite, repo_relative_project_dir=inputs.display(inputs.dir))
    files: dict[str, str] = {}
    for template, out in ctx.outputs:
        path = f"{output_rel}/{out}"
        source = template if template.startswith("common/") else f"{inputs.pattern}/{template}"
        files[path] = render.render(env, source, path, variables, header)

    satisfied_by: dict[str, list[str]] = {}
    for element, rid in ctx.satisfies:
        satisfied_by.setdefault(rid, []).append(element)
    trace = {
        "generator": f"{GENERATOR_NAME} {GENERATOR_VERSION}",
        "inputs_sha256": digest,
        "project": name,
        "pattern": inputs.pattern,
        "component": inputs.model["component"],
        "sil_suite": ctx.sil_suite,
        "requirements": [
            {
                "id": r.id,
                "asil": r.asil,
                "ears": r.ears,
                "text": r.text,
                "verification": list(r.verification),
                "satisfied_by": sorted(set(satisfied_by.get(r.id, []))),
                "tests": [t["name"] for t in tests if r.id in t["requirements"]],
            }
            for r in sorted(inputs.requirements.values(), key=lambda r: r.id)
        ],
        "tests": tests,
        "generated_files": sorted(files),
    }
    files[f"{output_rel}/trace.json"] = render.to_json(trace)
    return files
