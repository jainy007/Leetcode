# Functional safety concept: ACC and AEB at ASIL D (decomposition)

Standard: ISO 26262:2018, parts 3, 4, 6 and 9. Status: **draft**. Every safety goal, ASIL and FTTI
below is an **assumption pending the vehicle-level HARA**. They are set so that the chain can be
shown end to end, not taken from a real item definition.

## 1. Scope and assumed hazard analysis
| Hazard | Situation (assumed) | Rating (assumed) | Safety goal |
|---|---|---|---|
| H-ACC-1 Unintended strong acceleration | Following or approaching traffic at speed | S3 E4 C3 → **ASIL D** | REQ-ACCS-001 |
| H-ACC-2 Unintended strong deceleration | Dense traffic behind the ego vehicle | S3 E4 C3 → **ASIL D** | REQ-ACCS-002 |
| H-ACC-3 Acceleration toward a close vehicle | Cut-in, slower vehicle ahead | S3 E4 C3 → **ASIL D** | REQ-ACCS-003 |
| H-ACC-4 Actuation on stale or invalid commands | Any ACC operation | S3 E4 C3 → **ASIL D** | REQ-ACCS-004 |
| H-AEB-1 Unjustified emergency braking | Free road at speed, traffic behind | S3 E4 C3 → **ASIL D** | REQ-AEBS-001 |
| H-AEB-2 Braking on stale or invalid commands | AEB intervention ending, traffic behind | S3 E4 C3 → **ASIL D** | REQ-AEBS-002 |
| H-AEB-3 Missed braking | AEB is a driver assistance on top of the driver's own braking | lower ASIL (not addressed here) | — |

Safe state of both functions: no request from the function. Acceleration is cut at once; a
braking request is ramped out (ACC 5 m/s³, AEB 10 m/s³), and a driver takeover is requested.

## 2. Architecture and ASIL decomposition (ISO 26262-9, clause 5)
```
 radar, ego, driver ──► AccController / AebController  B(D) ──command (E2E)──┐
        │                                                                    ▼
        └────────────► AccMonitor / AebMonitor         B(D) ──verdict (E2E)─►  actuator ECU (ASIL D, AoU)
                       (checks every command)                                 applies only approved commands
```
- **Controller B(D):** generated `can_function` node (`examples/acc`, `examples/aeb`). Implements
  the function and its own input supervision.
- **Monitor B(D):** a separate generated `can_function` node (`examples/acc_monitor`,
  `examples/aeb_monitor`). It checks each new command (new E2E counter) against a safety envelope
  derived from the safety goals, not from the control law. The first violation latches
  `SAFE_STATE` until the next power cycle. In `MONITORING` it grants a permit and echoes the E2E
  counter of the command it checked.
- **Actuator ECU (assumption of use, ASIL D):** see §5. Modelled in SIL by
  `sil/plant/.../monitored_actuator.hpp`.

A single fault in either element cannot violate a safety goal:
- **Controller fault:** the monitor rejects the command, and the actuator applies nothing that
  was not approved.
- **Monitor fault, false alarm:** availability loss only (safe state).
- **Monitor fault, missed detection:** the monitor never generates a command, so a hazard still
  needs a second, independent controller fault.

## 3. Safety goals, mechanisms and FTTI
FDTI is computed by the generator from the monitor model (rule GEN-065). FRTI is the actuator's
assumed reaction time of 100 ms (§5).

| Safety goal | FTTI | Mechanisms (monitor) | FDTI | FDTI + FRTI | System SIL asserts |
|---|---|---|---|---|---|
| REQ-ACCS-001 acceleration above 2.1 m/s² or above the jerk envelope | 200 ms | `request_limit`, `request_rate` | 20 ms | 120 ms | safe state within 100 ms of the fault; applied and actual acceleration never above 2.1 m/s² |
| REQ-ACCS-002 deceleration above 3.6 m/s² or above the jerk envelope | 200 ms | `request_limit`, `request_rate` | 20 ms | 120 ms | safe state within 100 ms; applied and actual deceleration never above 3.6 m/s² |
| REQ-ACCS-003 acceleration toward a vehicle with TTC < 2 s | 1000 ms | `unintended_accel` (0.5 s debounce), `accel_and_brake` | 520 ms | 620 ms | safe state within 900 ms (cut-in with acceleration kept up) |
| REQ-ACCS-004 stopped, frozen or non-ACTIVE commands | 200 ms | `command_timeout` (60 ms), `request_outside_active` | 60 ms | 160 ms | safe state within 100 ms; braking ramped out within 800 ms |
| REQ-AEBS-001 unjustified braking | 200 ms | `braking_justification`, `request_limit`, `request_outside_braking` | 20 ms | 120 ms | safe state within 100 ms; on a free road the applied deceleration stays 0 and the actual one ≤ 0.1 m/s²; during full braking a request above 8.1 m/s² is never applied |
| REQ-AEBS-002 stopped or frozen commands | 200 ms | `command_timeout` (60 ms) | 60 ms | 160 ms | safe state within 100 ms; braking ramped out within 900 ms |

The component SIL of each monitor asserts the timeout reaction more tightly: SAFE_STATE within 40 ms
of the first lost frame. A frame loss starting at t0 leaves the last valid frame at t0 − 20 ms. The 60 ms timeout therefore
expires 40 ms after the first lost frame.

**Static check, GEN-065.** For every mechanism of every goal, the generator checks
FDTI + FRTI ≤ FTTI before any code exists. Changing a monitor timeout or debounce re-checks every
goal that relies on it. Example: a debounce of 0.9 s gives 920 + 100 > 1000 ms, and the
generator rejects the model.

**Dynamic check, system SIL.** `examples/*_system` runs both generated nodes on one bus. Faults
are injected on the node outputs:
- a wrong value behind a valid CRC (a logic fault);
- a stopped sender;
- a frozen sender;
- corrupted frames;
- loss of the monitor.

For each, the system SIL measures the time from the fault to the actuator's safe state against
FTTI − FRTI, and checks that the hazard never materialized, using the plant's actual
acceleration and deceleration.

## 4. Independence and freedom from interference
| Aspect | Measure | Status |
|---|---|---|
| Separate elements | Separate generated components, separate processes or ECUs, separate DBC extracts | Done in model and SIL |
| Diverse checks | Monitor logic is written against the envelope (limits, rates, state consistency, an independent TTC evaluation), not against the control law | Done |
| Interface consistency | GEN-066: shared messages are identical in both DBC extracts, the E2E data id of the command matches, and both nodes run at one cycle | Done |
| Communication | E2E (Profile 11 style) on command and verdict; the verdict names the approved command by its counter | Done |
| Common sensor | Both elements use the same radar object. The radar must deliver ASIL D object data (AoU on the sensor, §5) | **Open: sensor AoU** |
| Common tool | Both elements come from the same generator. Mitigation: TCL2 qualification (1b + 1c for ASIL D), back-to-back codec tests, mutation campaigns, MC/DC gate | See `tool_qualification.md` |
| Common runtime | Both elements use the hand-written runtime (E2E, codec, CRC) | Covered by `runtime_safety_manual.md` |
| Execution | Separate processes with memory protection, or separate cores and ECUs; scheduling separation (cgroups or QNX partitions) | **Open: deployment** |

## 5. Assumptions of use (AoU)
| ID | Assumption | Placed on |
|---|---|---|
| AoU-1 | Applies a command only if the monitor's latest valid frame reports MONITORING, grants the permit and echoes exactly that command's E2E counter | Actuator ECU (ASIL D) |
| AoU-2 | Without a new applicable command, holds the last request for at most 60 ms, then ramps it out | Actuator ECU |
| AoU-3 | On monitor SAFE_STATE, or 60 ms without a valid monitor frame after MONITORING: cuts acceleration at once, ramps braking out (ACC 5 m/s³, AEB 10 m/s³), raises a takeover request | Actuator ECU |
| AoU-4 | Reacts within 100 ms of the verdict (FRTI) | Actuator ECU |
| AoU-5 | The radar object list (range, range rate, lateral offset, validity) is correct to ASIL D, or is cross-checked by a second sensor | Radar / fusion |
| AoU-6 | Ego speed and driver inputs are delivered with E2E protection at the stated periods | Vehicle network |

## 6. Verification evidence (generated or measured in CI)
| Evidence | Where |
|---|---|
| Requirements → scenarios → tests → results | `python3 -m tools.trace` (83/83 requirements PASSED) |
| Component SIL of each node, system SIL with fault injection | `generated/*/sil/*_sil_test.cpp` |
| Codec back-to-back tests, component robustness tests | `generated/*/sil/*_codec_test.cpp`, `*_component_test.cpp` |
| MC/DC: 100 % for every ASIL D-derived file, two justified conditions | `python3 -m tools.coverage`, `coverage_justifications.yaml` |
| Mutation campaigns, including every calibration value | `python3 -m tools.mutate <project>`; see the SIL reports |
| Static FTTI check | GEN-065 |
| Sanitizers | ASan/UBSan and TSan presets |

## 7. Open items (needed before a real ASIL D claim)
1. **HARA:** the vehicle-level HARA must confirm or replace the safety goals, ASILs and FTTIs above.
2. **Sensor:** sensor ASIL and second-path plausibility (AoU-5). Both elements depend on one radar.
3. **Actuator ECU:** implementation and verification of AoU-1 to AoU-4 (currently only a SIL model).
4. **Target platform:** WCET and timing analysis on the target (PREEMPT_RT, QNX) and
   freedom-from-interference evidence. These need the target hardware and the QNX SDP. The
   generated code has bounded loops, no heap and no exceptions, which helps, but nothing has
   been measured on a target yet.
5. **Tool qualification:** method 1b (assessment of the development process) for ASIL D is
   not done yet.
6. **E2E:** validation of the E2E profile against official AUTOSAR test vectors.
7. **Plant:** the plant is limited to longitudinal point masses, one target and an ideal sensor
   apart from quantization and the injected faults. HIL and vehicle tests stay necessary.
