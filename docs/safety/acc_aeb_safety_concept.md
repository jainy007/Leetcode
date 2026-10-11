# Functional safety concept: ACC and AEB at ASIL D (decomposition)

Standard: ISO 26262:2018, parts 3, 4, 6 and 9. Status: **draft**.

The hazard analysis is part of the rule-based chain:
- **Inputs:** `examples/<f>_system/safety/hara.yaml` (hazards, situations, S/E/C with rationale)
  and `safety/fta.yaml` (fault trees).
- **Generated output:** the generator computes the ASIL, the FTTI and the fault-tree checks, and
  writes `generated/<f>_system/safety_analysis.md`.
- **Judgement vs. computation:** S, E and C are the project's engineering judgement; no public
  HARA with these parameters exists. Everything downstream of them is computed and checked
  (GEN-065, GEN-067 to GEN-069).

## 1. Hazard analysis and risk assessment (summary)
Hazards were found with HAZOP guide words (more, no, unintended, stuck) applied to the item
functions. The full tables are in the generated `safety_analysis.md`.

| Hazardous event | S | E | C | ASIL | Safety goal | Derived FTTI |
|---|---|---|---|---|---|---|
| HE-ACC-01 acceleration above the envelope while following at 1.0 s | S2 | E4 | C2 | B | REQ-ACCS-001 | 2200 ms |
| HE-ACC-02 deceleration above the envelope, follower at 0.8 s | S3 | E4 | C3 | **D** | REQ-ACCS-002 | 490 ms |
| HE-ACC-03 acceleration toward a slower vehicle at TTC 3 s | S3 | E3 | C2 | B | REQ-ACCS-003 | 370 ms |
| HE-ACC-04 frozen braking command (3.5 m/s²), follower at 0.8 s | S3 | E4 | C1 | B | REQ-ACCS-004 | no collision within 10 s |
| HE-ACC-05 frozen acceleration command (2.0 m/s²) toward a slower vehicle | S3 | E3 | C2 | B | REQ-ACCS-004 | 380 ms |
| HE-ACC-06 loss of ACC braking when approaching at TTC 3 s | S3 | E3 | C1 | A | REQ-ACCS-007 | 590 ms |
| HE-AEB-01 unjustified full braking, follower at 0.8 s | S3 | E4 | C3 | **D** | REQ-AEBS-001 | 780 ms |
| HE-AEB-02 unjustified partial braking (4 m/s²) | S2 | E4 | C2 | B | REQ-AEBS-001 | no collision within 10 s |
| HE-AEB-03 braking continues after the threat has gone | S3 | E2 | C3 | B | REQ-AEBS-002 | 780 ms |
| HE-AEB-04 no braking when a collision is imminent | S3 | E2 | C1 | QM | — | — |
| HE-AEB-05 false warning | S0 | E4 | C0 | QM | — | — |

**Result: both items are ASIL D.** The ASIL D comes from unintended braking, whether commanded by
ACC beyond its envelope or by AEB without justification. The other safety goals are ASIL A or B.
The decomposition B(D) + B(D) covers all of them.

**How the FTTI is derived (GEN-069):** point-mass kinematics of reference situations, using the
assumptions A-01 to A-07 in the HARA:
- driver and follower reaction 1.25 s (Green 2000, unexpected event);
- emergency braking 6 m/s² by ordinary drivers;
- follower headway 0.8 s;
- fault magnitudes of 4 m/s² (powertrain) and 10 m/s² (brakes);
- safe-state ramp-out of 20 m/s³ (AoU-3).

The FTTI is the longest time from the fault until the actuator enters its safe state for which
the collision is still avoided. The method follows the published kinematic and risk-threshold
derivations of FTTI; it does not use a fixed number from the literature, because none exists for
these hazards.

**What the derivation changed in the design:**
1. **Ramp-out:** the actuator's brake ramp-out was 5 m/s³ (ACC) and 10 m/s³ (AEB). At 5 m/s³, an
   unintended 10 m/s² braking hits a follower at 0.8 s headway even with instant detection
   (derived FTTI 0 ms). Both ramps are now 20 m/s³.
2. **ACC monitor check:** the monitor allowed acceleration toward a vehicle with TTC < 2 s for
   0.5 s. The derived FTTI for acceleration toward a slower vehicle is only 370 ms. The check now
   applies below TTC 3 s and needs no debounce: every new command must reduce the acceleration
   by at least 4 m/s³, so FDTI = 1 cycle. This cannot raise false alarms. The controller never
   requests acceleration from its gap law below TTC 3 s, because its range-rate gain divided by
   its gap gain is 3 s, and it ramps any remaining acceleration down at 5 m/s³.

Safe state of both functions: no request from the function. Acceleration is cut at once, braking
is ramped out at 20 m/s³, and a driver takeover is requested.

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
FDTI is computed by the generator from the monitor model (GEN-065). FRTI is the actuator's
assumed reaction time of 100 ms (AoU-4). The FTTI is derived from the HARA (GEN-069).

| Safety goal | ASIL | FTTI | Mechanisms (monitor) | FDTI | FDTI + FRTI | System SIL asserts |
|---|---|---|---|---|---|---|
| REQ-ACCS-001 acceleration above 2.1 m/s² or the jerk envelope | B | 2200 ms | `request_limit`, `request_rate` | 20 ms | 120 ms | safe state within the budget; applied and actual acceleration never above 2.1 m/s² |
| REQ-ACCS-002 deceleration above 3.6 m/s² or the jerk envelope | D | 490 ms | `request_limit`, `request_rate` | 20 ms | 120 ms | safe state within the budget; applied and actual deceleration never above 3.6 m/s² |
| REQ-ACCS-003 non-decaying acceleration toward a vehicle with TTC < 3 s | B | 370 ms | `unintended_accel`, `accel_and_brake` | 20 ms | 120 ms | safe state within the budget (cut-in with acceleration kept up) |
| REQ-ACCS-004 stopped, frozen or non-ACTIVE commands | B | 380 ms | `command_timeout` (60 ms), `request_outside_active` | 60 ms | 160 ms | safe state within the budget; braking ramped out within 240 ms |
| REQ-ACCS-007 loss of ACC braking without a takeover request | A | 590 ms | `command_timeout` | 60 ms | 160 ms | takeover requested after a stopped or frozen controller (scenario tagged REQ-ACCS-007) |
| REQ-AEBS-001 unjustified braking | D | 780 ms | `braking_justification`, `request_limit`, `request_outside_braking` | 20 ms | 120 ms | safe state within the budget; on a free road the applied deceleration stays 0 |
| REQ-AEBS-002 stopped or frozen commands | B | 780 ms | `command_timeout` (60 ms) | 60 ms | 160 ms | safe state within the budget; braking ramped out within 460 ms |

The budget in each system SIL scenario is FTTI − FRTI, resolved from the system model. The
component SIL of each monitor asserts the timeout reaction more tightly: SAFE_STATE within 40 ms
of the first lost frame. A frame loss starting at t0 leaves the last valid frame at t0 − 20 ms, so
the 60 ms timeout expires 40 ms after the first lost frame.

## 3a. Fault tree analysis (GEN-068)
Each safety goal has a fault tree over the elements controller, monitor, actuator, radar and
bus. The generator computes the minimal cut sets and checks each one:
- **Cut set inside one element:** that element must itself be rated for the goal. This applies to
  the actuator (D), the bus with E2E (D), and the radar (ACC: B, AEB: D).
- **Cut set over several elements:** one of them must be rated for the goal, or all of them must
  carry the goal's ASIL by decomposition.

Result: no single point in the controller or the monitor can violate an ASIL D goal. The ASIL D
braking goals are reached only by the {controller, monitor} pair (B(D) + B(D)), or by an element
that is itself ASIL D. The fault trees also make two needs explicit:
- the radar (AoU-5) must be ASIL D for AEB, because a ghost object justifies braking in both
  nodes;
- the radar must be ASIL B for ACC, because of REQ-ACCS-003.

Quantitative analysis (PMHF, SPFM, LFM) needs hardware failure rates and stays open.

**Static check, GEN-065.** For every mechanism of every goal, the generator checks
FDTI + FRTI ≤ FTTI before any code exists. Changing a monitor timeout re-checks every goal that
relies on it. Example: a command timeout of 300 ms gives 300 + 100 > 380 ms for REQ-ACCS-004, and
the generator rejects the model. GEN-069 checks the other side: the FTTI may not exceed what the
HARA's reference situations tolerate.

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
| AoU-3 | On monitor SAFE_STATE, or 60 ms without a valid monitor frame after MONITORING: cuts acceleration at once, ramps braking out at 20 m/s³, raises a takeover request | Actuator ECU |
| AoU-4 | Reacts within 100 ms of the verdict (FRTI) | Actuator ECU |
| AoU-5 | The radar object list (range, range rate, lateral offset, validity) is correct to ASIL D for AEB and ASIL B for ACC (from the fault trees), or is cross-checked by a second sensor | Radar / fusion |
| AoU-6 | Ego speed and driver inputs are delivered with E2E protection at the stated periods | Vehicle network |

## 6. Verification evidence (generated or measured in CI)
| Evidence | Where |
|---|---|
| HARA, FTTI derivation, fault trees | `generated/*_system/safety_analysis.md` (GEN-067 to GEN-069) |
| Requirements → scenarios → tests → results | `python3 -m tools.trace` (84/84 requirements PASSED) |
| Component SIL of each node, system SIL with fault injection | `generated/*/sil/*_sil_test.cpp` |
| Codec back-to-back tests, component robustness tests | `generated/*/sil/*_codec_test.cpp`, `*_component_test.cpp` |
| MC/DC: 100 % for every ASIL D-derived file, two justified conditions | `python3 -m tools.coverage`, `coverage_justifications.yaml` |
| Mutation campaigns, including every calibration value | `python3 -m tools.mutate <project>`; see the SIL reports |
| Static FTTI check | GEN-065 |
| Sanitizers | ASan/UBSan and TSan presets |

## 7. Open items (needed before a real ASIL D claim)
1. **HARA review:** the vehicle-level item definition and an independent review must confirm the
   S/E/C ratings and the assumptions A-01 to A-07, especially the follower headway and the
   reaction times that drive the derived FTTI. Some FTTIs are tight (370 ms and 380 ms for ACC).
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
