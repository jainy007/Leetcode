"""CLI: python3 -m tools.gen [--check | --dry-run] [--out-root DIR] [project ...]"""
from __future__ import annotations

import argparse
import pathlib
import sys

from tools.gen import GENERATOR_NAME, GENERATOR_VERSION, project, render
from tools.gen.errors import Diagnostics
from tools.gen.render import REPO_ROOT

PROJECTS_FILE = "ea_projects.yaml"
GENERATED_ROOT = "generated"


def generate_all(diags: Diagnostics, only: list[str] | None = None) -> dict[str, str]:
    doc = project.load_document(REPO_ROOT / PROJECTS_FILE, "projects.schema.json", PROJECTS_FILE, diags)
    if doc is None:
        return {}
    files: dict[str, str] = {}
    entries = [p for p in doc["projects"] if not only or p["name"] in only]
    for entry in entries:
        if not entry["output"].startswith(GENERATED_ROOT + "/"):
            diags.error("GEN-003", PROJECTS_FILE, f"output of {entry['name']} must be below {GENERATED_ROOT}/")
            continue
        files.update(project.generate(entry["name"], entry["model"], entry["output"], diags))
    if not only and diags.ok():
        files[f"{GENERATED_ROOT}/CMakeLists.txt"] = render.stamp(
            "CMakeLists.txt",
            "".join(f"add_subdirectory({p['output'][len(GENERATED_ROOT) + 1:]})\n" for p in doc["projects"]),
            render.header_lines("-", "all", render.inputs_digest([REPO_ROOT / PROJECTS_FILE])),
        )
    return files


def existing_files(root: pathlib.Path) -> set[str]:
    if not root.exists():
        return set()
    return {p.relative_to(root.parent).as_posix() for p in root.rglob("*") if p.is_file()}


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(prog="python3 -m tools.gen", description=f"{GENERATOR_NAME} {GENERATOR_VERSION}")
    mode = ap.add_mutually_exclusive_group()
    mode.add_argument("--check", action="store_true", help="fail if generated/ differs from the generator output")
    mode.add_argument("--dry-run", action="store_true", help="run all rules, write nothing")
    ap.add_argument("--out-root", type=pathlib.Path, default=REPO_ROOT,
                    help="directory that receives generated/ (default: repository root)")
    ap.add_argument("projects", nargs="*", help="limit to these project names (no stale-file pruning)")
    args = ap.parse_args(argv)

    diags = Diagnostics()
    files = generate_all(diags, args.projects or None)
    if not diags.ok():
        for f in diags.sorted():
            print(f.format(), file=sys.stderr)
        print(f"{len(diags.findings)} rule violation(s); nothing written.", file=sys.stderr)
        return 1
    if args.dry_run:
        print(f"OK: {len(files)} file(s) would be generated.")
        return 0

    out_root = args.out_root.resolve()
    gen_root = out_root / GENERATED_ROOT
    stale = set() if args.projects else existing_files(gen_root) - set(files)
    if args.check:
        problems = []
        for rel, content in sorted(files.items()):
            target = out_root / rel
            if not target.is_file():
                problems.append(f"missing: {rel}")
            elif target.read_bytes() != content.encode("utf-8"):
                problems.append(f"differs: {rel}")
        problems += [f"not produced by the generator: {rel}" for rel in sorted(stale)]
        if problems:
            print("\n".join(problems), file=sys.stderr)
            print("generated/ is out of date or was edited by hand. Run: python3 -m tools.gen", file=sys.stderr)
            return 1
        print(f"OK: {len(files)} generated file(s) up to date.")
        return 0

    for rel, content in sorted(files.items()):
        target = out_root / rel
        target.parent.mkdir(parents=True, exist_ok=True)
        if not target.is_file() or target.read_bytes() != content.encode("utf-8"):
            target.write_bytes(content.encode("utf-8"))
    for rel in sorted(stale):
        (out_root / rel).unlink()
    print(f"Generated {len(files)} file(s); removed {len(stale)} stale file(s).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
