"""Mutation testing of a project: does the chain detect deliberate defects?

    python3 -m tools.mutate <project> [--build-dir build/linux-host] [--only NAME] [--report FILE]

Reads examples/<project>/mutations.yaml:
    mutations:
      - name: time gap 1.5 -> 1.6 s
        file: examples/acc/model.yaml              # repo-relative; inputs, templates or generator
        old: "[1.0, 1.5, 2.0]"                     # must occur exactly once
        new: "[1.0, 1.6, 2.0]"
        expect: detected                           # detected | equivalent | tolerated
        note: optional reason (required for equivalent and tolerated)

'equivalent': no observable behaviour can differ. 'tolerated': behaviour differs, but stays within
every requirement's tolerance (e.g. a tuning gain without a requirement of its own).

For each mutant: apply, regenerate (a generator rejection counts as detected), rebuild the
project's test targets (<project>_sil and, if generated, <project>_codec_test), run them, restore
every file. Exit code 1 if a mutant expected to be detected survives, or an 'equivalent' mutant
is detected (the list is then out of date).

Calibration gate (ASIL D): every calibration value of the project's model must be changed by at
least one mutant of model.yaml; otherwise the run fails before any mutant is applied.
"""
from __future__ import annotations

import argparse
import pathlib
import re
import subprocess
import sys

import yaml

from tools.gen.render import REPO_ROOT


def _run(cmd: list[str]) -> subprocess.CompletedProcess:
    return subprocess.run(cmd, cwd=REPO_ROOT, capture_output=True, text=True)


def _targets(project: str) -> list[str]:
    cmake = (REPO_ROOT / "generated" / project / "CMakeLists.txt").read_text(encoding="utf-8")
    return [f"{project}_sil"] + ([f"{project}_codec_test"] if f"{project}_codec_test" in cmake else [])


def missing_calibration_mutants(project: str, mutants: list[dict]) -> list[str]:
    model_rel = f"examples/{project}/model.yaml"
    model = yaml.safe_load((REPO_ROOT / model_rel).read_text(encoding="utf-8"))
    olds = [m["old"] for m in mutants if m["file"] == model_rel]
    return [k for k in sorted(model.get("calibration", {})) if not any(f"{k}: {{value:" in o for o in olds)]


def evaluate(project: str, build_dir: pathlib.Path, m: dict) -> tuple[str, str]:
    path = REPO_ROOT / m["file"]
    original = path.read_bytes()
    text = original.decode("utf-8")
    if text.count(m["old"]) != 1:
        return "ERROR", f"anchor occurs {text.count(m['old'])} times in {m['file']}"
    try:
        path.write_text(text.replace(m["old"], m["new"]), encoding="utf-8")
        gen = _run([sys.executable, "-m", "tools.gen"])
        if gen.returncode != 0:
            rules = sorted(set(re.findall(r"GEN-\d{3}", gen.stderr)))
            return "detected", f"generator ({', '.join(rules) or 'error'})"
        targets = _targets(project)
        build = _run(["cmake", "--build", str(build_dir)] + [a for t in targets for a in ("--target", t)])
        if build.returncode != 0:
            return "detected", "does not compile"
        for target in targets:
            exe = build_dir / "generated" / project / target
            run = _run([str(exe), "--gtest_brief=1"])
            failed = len(re.findall(r"^\[  FAILED  \] \S+ \(", run.stdout, re.M))
            total = re.search(r"\[==========\] (\d+) tests? from", run.stdout)
            if run.returncode != 0 or failed:
                kind = "SIL" if target.endswith("_sil") else "codec test"
                return "detected", f"{kind} ({failed} of {total.group(1) if total else '?'} tests failed)"
        return "survived", "all SIL tests passed"
    finally:
        path.write_bytes(original)


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(prog="python3 -m tools.mutate")
    ap.add_argument("project")
    ap.add_argument("--build-dir", type=pathlib.Path, default=REPO_ROOT / "build" / "linux-host")
    ap.add_argument("--only", help="run only the mutant with this name")
    ap.add_argument("--report", type=pathlib.Path, help="write a markdown table to this file")
    args = ap.parse_args(argv)

    spec = yaml.safe_load((REPO_ROOT / "examples" / args.project / "mutations.yaml").read_text(encoding="utf-8"))
    missing = missing_calibration_mutants(args.project, spec["mutations"])
    if missing:
        print(f"calibration values without a mutant: {', '.join(missing)}")
        return 1
    mutants = [m for m in spec["mutations"] if not args.only or m["name"] == args.only]
    rows, bad = [], 0
    try:
        for m in mutants:
            verdict, detail = evaluate(args.project, args.build_dir, m)
            expect = m.get("expect", "detected")
            ok = (verdict == "detected") == (expect == "detected") and verdict != "ERROR"
            ok = ok and (expect == "detected" or bool(m.get("note")))
            bad += 0 if ok else 1
            rows.append((m["name"], m["file"], expect, verdict, detail, m.get("note", "")))
            print(f"{'ok ' if ok else 'BAD'} {m['name']}: {verdict} - {detail}", flush=True)
    finally:
        _run([sys.executable, "-m", "tools.gen"])  # leave generated/ consistent with the inputs
        _run(["cmake", "--build", str(args.build_dir)] + [a for t in _targets(args.project) for a in ("--target", t)])
    det = sum(1 for r in rows if r[3] == "detected")
    eq = sum(1 for r in rows if r[2] == "equivalent")
    tol = sum(1 for r in rows if r[2] == "tolerated")
    print(f"{det} of {len(rows)} mutants detected; {eq} declared equivalent; {tol} declared tolerated; "
          f"{bad} unexpected result(s)")
    if args.report:
        lines = ["| Mutant | File | Expected | Result | Detail | Note |", "|---|---|---|---|---|---|"]
        lines += [f"| {r[0]} | `{r[1]}` | {r[2]} | {r[3]} | {r[4]} | {r[5]} |" for r in rows]
        args.report.write_text("\n".join(lines) + "\n", encoding="utf-8")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
