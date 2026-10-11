"""Deterministic template rendering and file stamping."""
from __future__ import annotations

import hashlib
import json
import math
import pathlib

import jinja2

from tools.gen import GENERATOR_NAME, GENERATOR_VERSION
from tools.gen.steps import camel

REPO_ROOT = pathlib.Path(__file__).resolve().parents[2]
TEMPLATE_DIR = REPO_ROOT / "templates"
GEN_DIR = pathlib.Path(__file__).resolve().parent


def cpp_double(value: float | int) -> str:
    v = float(value)
    if not math.isfinite(v):
        raise ValueError(f"non-finite value {value!r}")
    text = repr(v)
    return text if ("." in text or "e" in text) else text + ".0"


def cpp_float(value: float | int) -> str:
    return cpp_double(value) + "F"


def cpp_hex(value: int, width: int = 0) -> str:
    return f"0x{value:0{width}X}U" if width else f"0x{value:X}U"


def make_env() -> jinja2.Environment:
    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(str(TEMPLATE_DIR)),
        undefined=jinja2.StrictUndefined,
        autoescape=False,
        trim_blocks=True,
        lstrip_blocks=True,
        keep_trailing_newline=True,
    )
    env.filters["camel"] = camel
    env.filters["cpp_double"] = cpp_double
    env.filters["cpp_float"] = cpp_float
    env.filters["cpp_hex"] = cpp_hex
    return env


def tool_files(pattern: str) -> list[pathlib.Path]:
    """Files of the tool itself whose content determines the output for `pattern`."""
    files = sorted(f for f in GEN_DIR.rglob("*.py") if "tests" not in f.relative_to(GEN_DIR).parts)
    files += sorted((TEMPLATE_DIR / "common").rglob("*.j2"))
    files += sorted((TEMPLATE_DIR / pattern).rglob("*.j2"))
    files += [REPO_ROOT / "sil" / "steps" / f"{pattern}.yaml"]
    files += sorted((REPO_ROOT / "schemas").glob("*.schema.json"))
    return files


def rel_display(path: pathlib.Path) -> str:
    """Repository-relative POSIX path; inputs outside the repository (tests) keep their name only."""
    try:
        return path.resolve().relative_to(REPO_ROOT).as_posix()
    except ValueError:
        return "<external>/" + path.name


def inputs_digest(files: list[pathlib.Path]) -> str:
    h = hashlib.sha256()
    for f in sorted(set(files), key=rel_display):
        rel = rel_display(f)
        data = f.read_bytes()
        if f.suffix in (".py", ".j2", ".yaml", ".json", ".feature", ".dbc"):
            data = data.replace(b"\r\n", b"\n")
        h.update(rel.encode() + b"\0" + data + b"\0")
    return h.hexdigest()


def header_lines(pattern: str, project: str, digest: str) -> list[str]:
    return [
        "GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.",
        f"Generator: {GENERATOR_NAME} {GENERATOR_VERSION} (rule-based, deterministic; no AI in this path)",
        f"Project:   {project} (pattern: {pattern})",
        f"Inputs:    sha256:{digest}",
        "Regenerate: python3 -m tools.gen",
    ]


def stamp(path: str, body: str, header: list[str]) -> str:
    if path.endswith((".hpp", ".cpp")):
        prefix = "".join(f"// {line}\n" for line in header)
    elif path.endswith((".txt", ".cmake")):
        prefix = "".join(f"# {line}\n" for line in header)
    else:
        return body
    return prefix + "\n" + body


def render(env: jinja2.Environment, template: str, out_path: str, variables: dict, header: list[str]) -> str:
    body = env.get_template(template).render(**variables)
    if not body.endswith("\n"):
        body += "\n"
    return stamp(out_path, body, header)


def to_json(doc: dict) -> str:
    return json.dumps(doc, indent=2, sort_keys=True, ensure_ascii=True) + "\n"
