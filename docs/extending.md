# Extending the toolchain with new ADAS features

Most ADAS functions on a performance core read CAN messages (objects, ego state, driver input),
run a state machine and a controller, and write CAN messages (requests, warnings, status). The
`can_function` pattern generates everything such functions share. A new feature only supplies
what is specific to it.

```
              shared (framework)                         per feature (plug-in)
┌──────────────────────────────────────────┐   ┌──────────────────────────────────────────────┐
│ tools/gen/patterns/can_function.py        │   │ tools/gen/features/<f>.py                     │
│   DBC import, rx/tx, E2E P11 layout,      │   │   STATES, INTERFACE, CALIBRATION,             │
│   bindings + ranges, timing, calibration  │   │   check(), derive(), OBSERVER                 │
│   rules GEN-060..GEN-064                  │   │ templates/features/<f>/logic.{hpp,cpp}.j2     │
│ templates/can_function/*                  │   │   Inputs -> Outputs, pure C++ logic           │
│   receive -> Inputs, Outputs -> send,     │   │   (optional) harness.hpp.j2: driver sequences │
│   SIL harness, CMake                      │   │ sil/steps/features/<f>.yaml                   │
│ sil/steps/can_function.yaml               │   │   feature observations                        │
│   traffic, driver, faults, vehicle checks │   │ sil/plant/.../<f>_can.hpp, <f>_observer.*     │
│ sil/plant (Plant)                         │   │   independent decoding of the output message  │
│   vehicles, radar, driver, bus, faults    │   │ examples/<f>/                                 │
│                                           │   │   requirements, DBC, model, Gherkin, mutations│
└──────────────────────────────────────────┘   └──────────────────────────────────────────────┘
```

Shipped features: `acc` (adaptive cruise control) and `aeb` (forward collision warning plus
emergency braking). Both use the same vehicle bus and the same plant.

## Workflow
1. **Scaffold.** This creates a feature that already builds and passes its own SIL:
   ```bash
   python3 -m tools.scaffold feature lka --can-id 816
   python3 -m tools.gen && cmake --build --preset linux-host && ctest --preset linux-host -R Lka
   ```
2. **Requirements first.** Edit `examples/<f>/requirements.yaml` in EARS form. Each requirement
   has an ID, an ASIL and a verification method. Run `python3 -m tools.lint` while you write.
3. **Interface.** Extend `examples/<f>/interfaces/<f>.dbc` with the output signals. In
   `tools/gen/features/<f>.py`, list every logical signal in `INTERFACE`, with the physical range
   it needs (a function of the calibration). Rule GEN-061 then proves at generation time that the
   DBC can carry it.
4. **Calibration.** Declare keys in `CALIBRATION` and consistency rules in `check()`. `derive()`
   converts values into the forms the template needs (for example whole cycles or resolution
   steps) and rejects values that can't be represented exactly.
5. **Logic.** Write `templates/features/<f>/logic.{hpp,cpp}.j2`: a class `<Component>Logic` with
   `step(const Inputs&, Outputs&)`.
   - `Inputs` holds one double per rx interface signal, `<signal>_ok` for the health of the
     message carrying it, and `all_ok`.
   - `Outputs` holds one double per tx interface signal.
   - State code 0 is the state before the first cycle.
   - The rules in `docs/rules/cpu.md` apply: no heap, no exceptions, bounded loops.
6. **Plant observer.** In `sil/plant/include/ea/plant/<f>_can.hpp`, write the output layout
   **by hand** from the DBC. Never derive it from generated code: this independence is what
   catches codec errors. In `<f>_observer.*`:
   - decode the output frames;
   - keep the metrics the requirements talk about;
   - return the longitudinal request, if the feature actuates.
7. **Steps and scenarios.**
   - Add feature observations to `sil/steps/features/<f>.yaml`.
   - Reuse `sil/steps/can_function.yaml` for traffic, the driver, faults and vehicle checks.
   - Write Gherkin whose expected values come from the requirements, with measurement tolerances
     stated and justified.
   - New plant capabilities (a new actor, a lateral model, a new sensor message) go into `Plant`
     and `can_function.yaml`, so every feature gains them.
8. **Mutation check.** List deliberate defects in `examples/<f>/mutations.yaml`: model values,
   DBC layout and logic-template edits. Then run `python3 -m tools.mutate <f>`.
   - Every non-equivalent mutant must be detected.
   - A survivor means a scenario is too weak: strengthen it.
   - Declare a mutant `equivalent` only with a written reason.
9. **Report.** Write `docs/<f>_sil_report.md` (see `docs/acc_sil_report.md`,
   `docs/aeb_sil_report.md`). Then complete the definition of done in `CLAUDE.md`.

## When the framework is not enough
- **A new transport or sensor type** (lane markings from a camera, several radar objects, Ethernet
  or SOME/IP): extend the plant and `can_function.yaml`, and if needed the framework templates.
  Keep the framework feature-agnostic: feature names never appear in `templates/can_function/`.
- **A different architecture** (GPU stage, interface gate): add a pattern next to `input_gate`
  and `gpu_stage`, with its own schema, templates, step catalogue and rules.
- **New rules:** register each one in `tools/gen/rules.py` and document it in
  `docs/rules/generator.md`. Add a negative case to `tools/gen/tests/test_rules.py`.
  `test_rule_coverage` enforces all three.

## Ideas for next features on the same bus
| Feature | Extra inputs | Plant extension |
|---|---|---|
| LKA / LDW (lane keeping) | lane marking message (camera) | lateral dynamics, lane geometry |
| BSD (blind spot) | rear-corner radar objects | adjacent-lane actors |
| TSR-based speed limiter | traffic sign message | sign events on the route |
| Stop & go ACC | standstill handling | low-speed lead behaviour, standstill hold |
