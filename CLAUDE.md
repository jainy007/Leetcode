# CLAUDE.md: rules for AI agents working in this repository

This repository is a **rule-based** toolchain for safety-related C++ on POSIX performance cores:

```
requirements (EARS) -> Gherkin -> model (YAML) -> [deterministic generator] -> C++ -> SIL tests -> trace report
```

Its outputs feed ISO 26262 ASIL-rated work products. **No AI may take part in the generation
leg.** The arrow marked `[deterministic generator]` is a qualified tool. The same inputs must
always give byte-identical outputs.

## Where an agent may and may not write

| Path | Agent may write? | Notes |
|---|---|---|
| `examples/*/requirements.yaml`, `examples/*/features/*.feature`, `examples/*/model.yaml`, `examples/*/interfaces/*` | **Yes, as a proposal** | The PR must carry the `ai-proposed` label and must be approved by the CODEOWNER of that example. |
| `docs/` (except `docs/safety/`) | Yes | |
| `generated/` | **Never** | Only `python3 -m tools.gen` writes here. CI regenerates and fails on any diff. |
| `tools/gen/`, `templates/`, `schemas/`, `sil/steps/` | **Never** | These are the qualified tool and its configuration. Humans only, two reviews, and the qualification suite must pass. |
| `runtime/` | No | Hand-written, reviewed safety code. An agent may suggest a change in a review comment. |
| `docs/safety/` | No | Safety case artefacts. |
| `legacy/` | No | Archived LeetCode practice code. Do not build or modify it. |

If a task seems to need a change in a "never" path, stop. Explain the needed change to the
human instead of making it.

## What an agent is good for here
- Drafting requirements in EARS form (`When ..., the <system> shall ...`). `python3 -m tools.lint` checks them.
- Drafting Gherkin scenarios that use **only** steps from the step catalogue (`sil/steps/*.yaml`).
- Drafting or extending a model, then running `python3 -m tools.gen --dry-run` to see the generator's
  rule violations, and fixing the model (never the generator) until it passes.
- Explaining generator errors, SIL failures and gaps in the trace report.
- Reviewing hand-written code against `docs/rules/*.md`, citing rule IDs.

## Commands
```bash
cmake --preset linux-host && cmake --build --preset linux-host && ctest --preset linux-host
python3 -m tools.gen            # regenerate everything listed in ea_projects.yaml
python3 -m tools.gen --check    # fail if generated/ differs from what the generator produces
python3 -m tools.lint           # requirements / Gherkin / model consistency
python3 -m tools.trace --results build/linux-host/test-results   # trace matrix and coverage of requirements
python3 -m unittest discover -s tools/gen/tests -t .             # generator qualification suite
```

## Coding rules (for hand-written code you review)
- C++17. Profiles: `asil` (no exceptions, no RTTI, no heap after init) and `qm`.
- Rule IDs: `docs/rules/cpu.md` (CPU-xxx), `docs/rules/gpu.md` (GPU-xxx),
  `docs/rules/multiprocess.md` (MP-xxx, IPC-xxx), `docs/rules/generator.md` (GEN-xxx).
- Errors are returned as `ea::Result<T>`; use `EA_TRY` to propagate.
- Formatting: `.clang-format` (4 spaces, 120 columns).
