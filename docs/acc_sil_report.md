# ACC closed-loop SIL: tool validation report

**Goal:** test the *toolchain*, not ACC. Starting from a set of requirements, we show three things:
- the rule-based generator produces C++ that meets the requirements in closed loop;
- the tests can see defects;
- the generator rejects inconsistent inputs before any code exists.

## Chain
```
examples/acc/requirements.yaml   22 EARS requirements (ISO 15622 style, values are project choices)
examples/acc/interfaces/acc.dbc  4 classic-CAN messages: RadarObject, EgoState, DriverInput -> AccCommand
examples/acc/model.yaml          pattern acc_controller: timing, E2E, bindings, calibration
examples/acc/features/*.feature  modes, control, faults; expected numbers derived from the requirements
        │  python3 -m tools.gen   (rules GEN-001..GEN-064, no AI)
        ▼
generated/acc/                   codec, controller, SIL harness, 46 SIL tests, trace.json
        │
sil/plant/                       hand-written plant: ego + lead vehicle, radar, driver, CAN timing,
                                 fault injection (lost / corrupted / paused frames), metrics
```

**The test oracle is independent of the generator.** The plant encodes and decodes frames from
its own hand-written copy of the DBC (`sil/plant/include/ea/plant/acc_can.hpp`). It checks the
controller's E2E protection itself. Every observation, such as mode, requests, distance or
speed, comes from what the plant sees on the bus and in its own vehicle state. Expected values
come from the requirements, for example a following distance of `5 m + T_gap · v`.

## Results
| Item | Result |
|---|---|
| ACC requirements | 22 / 22 PASSED in the trace matrix (39 / 39 across all projects) |
| Generated SIL tests | 46, all passing; 127 tests in the whole repository on host, ASan/UBSan and TSan |
| Generator qualification suite | passes; 15 new negative cases for GEN-060..GEN-064 |

## Findings made by the chain
1. **Inconsistent inputs found by closed-loop SIL.** REQ-ACC-021 says "no valid frame for
   120 ms gives FAULT", which allows 2 lost radar frames. But the model configured E2E with
   `max_delta_counter: 2`. The first frame after 2 lost frames therefore failed the sequence check
   and counted as not received, so the controller went to FAULT too early. Fixes:
   - **Model:** `max_delta_counter: 3`.
   - **Generator:** rule GEN-064 now rejects this class of inconsistency before code generation.
2. **Weak scenarios found by mutation testing**, all strengthened:
   - the hand-over speed was not checked on both sides of 25 km/h;
   - the 200 m target range was tested with a vehicle that would not have caused braking anyway;
   - OVERRIDE plus brake was never tested;
   - a held Set button was never tested;
   - the exact timeout boundary was hidden by the E2E sequence check. A "paused sender" fault
     (no frames sent, counter not advanced) now tests it.
3. **A test-infrastructure precision issue.** Converting between km/h and m/s gives values like
   120.00000000000001. Comparisons on plant measurements now use a 1e-6 km/h tolerance.

## Mutation testing
Each mutant is one deliberate defect. After each, the code was regenerated, rebuilt and SIL rerun.

### Defects in the inputs (model / DBC): 16 of 16 detected
| Mutant | Detected by |
|---|---|
| time gap 1.5 → 1.6 s | SIL (4 tests) |
| deceleration limit 3.5 → 4.5 m/s² | SIL |
| acceleration limit 2.0 → 2.5 m/s² | SIL |
| jerk increase limit 2.5 → 3.0 m/s³ | SIL |
| disengage speed 25 → 20 km/h and 25 → 28 km/h | SIL (after strengthening) |
| lane half width 1.8 → 2.0 m | SIL |
| override threshold 5 → 3 % | SIL |
| set speed step 5 → 10 km/h | SIL |
| standstill distance 5 → 7 m | SIL |
| wrong E2E data id | SIL (all tests) |
| radar timeout 120 → 160 ms | generator GEN-064 |
| E2E max delta 3 → 2 | generator GEN-064 |
| AccelRequest factor 0.01 → 0.02 | generator GEN-063 |
| ObjRange moved by one bit | generator GEN-033 / GEN-060 |
| SetSpeed overlapping another signal | generator GEN-032 |
| brake request bound to the accel signal | generator GEN-060 |

### Defects in the generator's templates: 15 of 18 detected, 3 equivalent
Detected:
- gap control `min` → `max`;
- acceleration and brake swapped;
- Set handled as a level instead of an edge;
- input timeout off by one;
- FAULT not latched;
- jerk limit removed;
- jerk limits swapped;
- lane check removed;
- 200 m range check removed;
- range-rate sign flipped;
- brake does not end OVERRIDE;
- set speed not cleared on OFF;
- Resume without a stored set speed;
- Speed Down adding instead of subtracting;
- steering request not 0.

Equivalent (no requirement-level observation can tell them apart):
- **Driver-input timeout off by one:** this check duplicates the general input-health check
  (redundant code, worth removing in a later cleanup).
- **Set speed not rounded internally:** the transmitted `SetSpeed` has 1 km/h resolution, and
  REQ-ACC-010 allows 1 km/h of speed error.
- **Disengage check also in OVERRIDE:** practically unreachable, because OVERRIDE starts at
  25 km/h or more while the driver is accelerating.

## Limits of this evidence
- **Plant model:** longitudinal point masses, first-order actuator lag, no road load, no grade.
  The radar is ideal apart from signal quantization and the injected faults. There is one target
  object.
- **Function scope:** no stop-and-go (by requirement).
- **E2E:** modelled on AUTOSAR Profile 11, but not validated against official AUTOSAR test vectors.
- **Exception to `CLAUDE.md`:** at the repository owner's request, an AI wrote the new pattern
  (`tools/gen/patterns/acc_controller.py`, `templates/acc_controller/`), the step catalogue
  (`sil/steps/acc_controller.yaml`) and the E2E runtime (`runtime/*/e2e/p11*`). These paths are
  human-only, so they need the two-person review before the pattern counts as qualified.
