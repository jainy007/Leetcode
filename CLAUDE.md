# CLAUDE.md: rules for agents working in this repository

This repository is a **rule-based** toolchain for safety-related C++ on POSIX performance cores:

```
requirements (EARS) -> Gherkin -> model (YAML + DBC) -> [deterministic generator] -> C++ -> SIL -> trace report
```

Its outputs feed ISO 26262 ASIL-rated work products.

## Ownership
The repository owner authorised the agent to develop **everything end to end**: inputs, the
generator, templates, schemas, step catalogues, the runtime, the plant and the docs.

That authorisation does **not** relax one rule: **the generation step stays AI-free.**
- C++ under `generated/` is written only by `python3 -m tools.gen`. Never edit it by hand, as an
  agent or as a human. Change the inputs or the generator, then regenerate. CI fails on any diff
  (`python3 -m tools.gen --check`).
- The generator is deterministic: no randomness, no clock, no network, no AI calls. The same
  inputs always give byte-identical output (`tools/gen/tests/test_determinism.py`).

## Definition of done for any change
1. `python3 -m unittest discover -s tools/gen/tests -t .` passes. Every rule GEN-xxx has a
   negative test, and every rule is documented in `docs/rules/generator.md`.
2. `python3 -m tools.gen --check` passes (generated/ is current).
3. `ctest` passes for presets `linux-host`, `linux-host-asan` and `linux-host-tsan`.
4. `python3 -m tools.trace --results build/linux-host/test-results` reports every requirement PASSED.
5. **A new or changed feature gets a mutation check:** `python3 -m tools.mutate <project>`.
   - Plant defects in the model, the DBC and the templates; every calibration value needs a mutant.
   - Every mutant must be detected, or declared `equivalent` or `tolerated` with a reason.
   - Record the result in the feature's report under `docs/`.
6. **Structural coverage gate:** `python3 -m tools.coverage`, after running ctest on the
   `linux-host-coverage` preset with `LLVM_PROFILE_FILE` set. ASIL D-derived files need 100 %
   MC/DC or a justified exception; no file may fall below `coverage_baseline.json`.
7. **ASIL D functions are decomposed:** controller B(D) + independent monitor B(D) + system
   project with safety goals and FTTI (GEN-065). Never rate a function lower than its safety goals
   without saying so; ask if the ASIL is unclear.
8. Expected values in Gherkin come from the requirements, never from running the code.
9. Optimizations keep outputs bit-identical, proven by an equivalence test.

## Adding an ADAS feature
See `docs/extending.md`. In short:
```bash
python3 -m tools.scaffold feature <name>   # creates every file of a new CAN function feature
# fill in: requirements, DBC, model, Gherkin, logic template, feature module, plant observer
python3 -m tools.lint && python3 -m tools.gen && cmake --build --preset linux-host && ctest --preset linux-host
```

## Commands
```bash
cmake --preset linux-host && cmake --build --preset linux-host && ctest --preset linux-host
python3 -m tools.gen            # regenerate everything listed in ea_projects.yaml
python3 -m tools.gen --check    # fail if generated/ differs from what the generator produces
python3 -m tools.lint           # requirements / Gherkin / model consistency, writes nothing
python3 -m tools.trace --results build/linux-host/test-results
python3 -m unittest discover -s tools/gen/tests -t .
python3 -m tools.mutate <project>                  # mutation campaign (examples/<project>/mutations.yaml)
python3 -m tools.coverage                          # MC/DC gate (after ctest on linux-host-coverage)
```

## Coding rules
- C++17. Profiles: `asil` (no exceptions, no RTTI, no heap after init) and `qm`.
- Rule IDs: `docs/rules/cpu.md` (CPU-xxx), `docs/rules/gpu.md` (GPU-xxx),
  `docs/rules/multiprocess.md` (MP-xxx, IPC-xxx), `docs/rules/generator.md` (GEN-xxx).
- Errors are returned as `ea::Result<T>`; use `EA_TRY` to propagate.
- The plant (`sil/plant`) must stay independent of generated code: it uses its own hand-written
  signal layouts, so codec errors show up as test failures.
- Formatting: `.clang-format` (4 spaces, 120 columns).
