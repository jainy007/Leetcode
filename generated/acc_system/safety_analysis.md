<!--
GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
Project:   acc_system (pattern: can_system)
Inputs:    sha256:23346b03d4046c6dcd8a3513aba427631a7d11a306ae3f61a7a6d60428b173b5
Regenerate: python3 -m tools.gen
-->
# Safety analysis of AccSystem (generated)

Inputs: `examples/acc_system/safety/hara.yaml` (HARA), `examples/acc_system/safety/fta.yaml`
(fault trees) and `examples/acc_system/model.yaml` (safety goals, FTTI, actuator). S, E and C are
engineering judgement with the rationale given in the HARA. Everything else on this page is computed
by the generator: ASIL (ISO 26262-3 table 4, GEN-067), FTTI (kinematic models, GEN-069), minimal
cut sets and the single-point check (GEN-068), and the FDTI budget (GEN-065).

## Assumptions
| ID | Assumption | Source |
|---|---|---|
| A-01 | A driver reacts to an unexpected event, including a takeover request, after 1.25 s. | M. Green, "How long does it take to stop?", Transportation Human Factors 2(3), 2000 (0.7 s expected, 1.25 s unexpected, 1.5 s surprise) |
| A-02 | Ordinary drivers brake at 6 m/s2 in an emergency, below the 8-9 m/s2 tyre limit on a dry road. | engineering estimate, consistent with emergency-braking field studies |
| A-03 | A following vehicle keeps a headway of 0.8 s. Shorter headways than the 2 s rule are common on motorways. | engineering estimate (conservative within observed motorway headways) |
| A-04 | A powertrain fault can accelerate the car by up to 4 m/s2 at 80-130 km/h; a brake fault can decelerate it by up to 10 m/s2. | passenger-car capability, engineering estimate |
| A-05 | In its safe state the actuator ECU cuts acceleration at once and ramps braking out at 20 m/s3 (AoU-3), within 100 ms of the monitor's verdict (AoU-4). | system model actuator.reaction_ms and actuator.ramp_out_mps3 |
| A-06 | ACC is an SAE level 1 function (ISO 15622) with authority limited to 2.0 m/s2 acceleration and 3.5 m/s2 deceleration; the driver supervises. | examples/acc/requirements.yaml |
| A-07 | Severity follows the impact speed of a car-to-car collision. S3 if more than 40 km/h is plausible, S2 for 20-40 km/h, S1 below. | ISO 26262-3 Annex B examples, engineering estimate |

## Hazards (HAZOP guide words)
| ID | Guide word | Function | Malfunction |
|---|---|---|---|
| H-ACC-01 | more | acceleration request | acceleration above the ACC envelope (more than 2.1 m/s2 or faster than the jerk limit) |
| H-ACC-02 | more | deceleration request | deceleration above the ACC envelope (more than 3.6 m/s2 or faster than the jerk limit) |
| H-ACC-03 | unintended | acceleration request | acceleration continues toward a close vehicle in the lane |
| H-ACC-04 | stuck | acceleration and deceleration requests | the command freezes or keeps being applied after the function stopped |
| H-ACC-05 | no | deceleration request | deceleration is lost while ACC is expected to brake, without a takeover request |

## Hazardous events and ASIL
| ID | Hazard | Situation | S | E | C | ASIL | Safety goal |
|---|---|---|---|---|---|---|---|
| HE-ACC-01 | H-ACC-01 | Motorway, following a vehicle at the shortest time gap (1.0 s) at 80-130 km/h. | S2 | E4 | C2 | **B** | REQ-ACCS-001 |
| HE-ACC-02 | H-ACC-02 | Motorway or main road at 50-130 km/h with a following vehicle at 0.8 s headway. | S3 | E4 | C3 | **D** | REQ-ACCS-002 |
| HE-ACC-03 | H-ACC-03 | Approaching a slower vehicle in the lane (closing at 20-50 km/h) at a time to collision of 3 s. | S3 | E3 | C2 | **B** | REQ-ACCS-003 |
| HE-ACC-04 | H-ACC-04 | A frozen braking command at the ACC limit (3.5 m/s2) with a following vehicle at 0.8 s headway, 50-130 km/h. | S3 | E4 | C1 | **B** | REQ-ACCS-004 |
| HE-ACC-05 | H-ACC-04 | A frozen acceleration command at the ACC limit (2.0 m/s2) while approaching a slower vehicle (closing 20-50 km/h) at TTC 3 s. | S3 | E3 | C2 | **B** | REQ-ACCS-004 |
| HE-ACC-06 | H-ACC-05 | Approaching a slower vehicle (closing 30-50 km/h) at TTC 3 s; the ACC braking is lost and the car coasts. | S3 | E3 | C1 | **A** | REQ-ACCS-007 |

### Rationale
- **HE-ACC-01**: S2: The driver brakes after noticing the acceleration, so a collision with the lead vehicle happens at a moderate speed difference (20-40 km/h). E4: Following on motorways is the main use of ACC and takes far more than 10 % of its operating time. C2: The supervising driver perceives an unexpected acceleration at once and overrides it with the brake pedal; 90 % or more of drivers avoid harm.
- **HE-ACC-02**: S3: A rear-end collision by the following car can exceed 40 km/h after full braking from 130 km/h. E4: Being followed at a short headway is a common motorway situation (more than 10 % of the driving time). C3: The follower is at risk. With 0.8 s headway and a 1.25 s reaction time, it cannot avoid an unexpected 10 m/s2 braking. The ego driver cannot cancel a braking fault in time.
- **HE-ACC-03**: S3: A collision at a closing speed of up to 50 km/h is plausible if the acceleration continues. E3: Approaching within 3 s of a slower vehicle while ACC is active occurs in 1-10 % of the operating time (cut-ins, traffic jams). C2: The supervising driver sees the closing vehicle and brakes; the takeover request supports this. The margin is small, so not C1.
- **HE-ACC-04**: S3: A rear-end collision at more than 40 km/h is plausible if the follower does not react. E4: ACC braking while being followed closely is a common situation. C1: The follower brakes harder (6 m/s2) than the frozen 3.5 m/s2 and avoids the collision after its reaction time. The kinematic FTTI confirms there is no collision for any duration.
- **HE-ACC-05**: S3: A collision at a closing speed of up to 50 km/h is plausible. E3: As HE-ACC-03; the frozen command must coincide with an approach. C2: As HE-ACC-03: the driver supervises and brakes.
- **HE-ACC-06**: S3: If the driver notices too late, a collision at a closing speed of up to 50 km/h is plausible. E3: As HE-ACC-03: an approach within 3 s while ACC is active. C1: ACC braking is limited to 3.5 m/s2, and the supervising driver expects to brake in such situations. With a takeover request, nearly all drivers brake in time.

## Safety goals, FTTI and detection budget
FTTI derived = the longest fault duration (fault → actuator safe state) for which every reference
situation still avoids the collision; the model FTTI may be smaller (conservative). Budget check:
FDTI of every mechanism + actuator reaction (100 ms) ≤ model FTTI.

| Safety goal | ASIL | Derived FTTI | Model FTTI | Mechanisms | Worst FDTI | FDTI + reaction |
|---|---|---|---|---|---|---|
| REQ-ACCS-001 | B | 2200 ms | 2200 ms | request_limit, request_rate | 20 ms | 120 ms |
| REQ-ACCS-002 | D | 490 ms | 490 ms | request_limit, request_rate | 20 ms | 120 ms |
| REQ-ACCS-003 | B | 370 ms | 370 ms | unintended_accel, accel_and_brake | 20 ms | 120 ms |
| REQ-ACCS-004 | B | 380 ms | 380 ms | command_timeout, request_outside_active | 60 ms | 160 ms |
| REQ-ACCS-007 | A | 590 ms | 590 ms | command_timeout | 60 ms | 160 ms |

### FTTI derivation per reference situation
**HE-ACC-01** (approach_with_accel):

| Situation | Derived FTTI |
|---|---|
| closing_kph=0, driver_decel_mps2=6.0, fault_accel_mps2=4.0, follower_decel_mps2=6.0, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=80, time_gap_s=1.0, ttc_s=0 | 2200 ms |
| closing_kph=0, driver_decel_mps2=6.0, fault_accel_mps2=4.0, follower_decel_mps2=6.0, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=100, time_gap_s=1.0, ttc_s=0 | 2470 ms |
| closing_kph=0, driver_decel_mps2=6.0, fault_accel_mps2=4.0, follower_decel_mps2=6.0, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=130, time_gap_s=1.0, ttc_s=0 | 2830 ms |

**HE-ACC-02** (rear_end_by_follower):

| Situation | Derived FTTI |
|---|---|
| driver_decel_mps2=6.0, fault_decel_mps2=10.0, follower_decel_mps2=6.0, headway_s=0.8, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=50 | 490 ms |
| driver_decel_mps2=6.0, fault_decel_mps2=10.0, follower_decel_mps2=6.0, headway_s=0.8, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=80 | 850 ms |
| driver_decel_mps2=6.0, fault_decel_mps2=10.0, follower_decel_mps2=6.0, headway_s=0.8, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=130 | 1370 ms |

**HE-ACC-03** (approach_with_accel):

| Situation | Derived FTTI |
|---|---|
| closing_kph=20, driver_decel_mps2=6.0, fault_accel_mps2=2.1, follower_decel_mps2=6.0, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=100, time_gap_s=0, ttc_s=3.0 | 640 ms |
| closing_kph=30, driver_decel_mps2=6.0, fault_accel_mps2=2.1, follower_decel_mps2=6.0, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=100, time_gap_s=0, ttc_s=3.0 | 590 ms |
| closing_kph=50, driver_decel_mps2=6.0, fault_accel_mps2=2.1, follower_decel_mps2=6.0, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=100, time_gap_s=0, ttc_s=3.0 | 370 ms |

**HE-ACC-04** (rear_end_by_follower):

| Situation | Derived FTTI |
|---|---|
| driver_decel_mps2=6.0, fault_decel_mps2=3.5, follower_decel_mps2=6.0, headway_s=0.8, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=50 | 10000 ms |
| driver_decel_mps2=6.0, fault_decel_mps2=3.5, follower_decel_mps2=6.0, headway_s=0.8, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=130 | 10000 ms |

**HE-ACC-05** (approach_with_accel):

| Situation | Derived FTTI |
|---|---|
| closing_kph=30, driver_decel_mps2=6.0, fault_accel_mps2=2.0, follower_decel_mps2=6.0, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=100, time_gap_s=0, ttc_s=3.0 | 610 ms |
| closing_kph=50, driver_decel_mps2=6.0, fault_accel_mps2=2.0, follower_decel_mps2=6.0, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=100, time_gap_s=0, ttc_s=3.0 | 380 ms |

**HE-ACC-06** (approach_with_accel):

| Situation | Derived FTTI |
|---|---|
| closing_kph=30, driver_decel_mps2=6.0, fault_accel_mps2=0.0, follower_decel_mps2=6.0, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=100, time_gap_s=0, ttc_s=3.0 | 1050 ms |
| closing_kph=50, driver_decel_mps2=6.0, fault_accel_mps2=0.0, follower_decel_mps2=6.0, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=100, time_gap_s=0, ttc_s=3.0 | 590 ms |

## Fault trees: minimal cut sets
### REQ-ACCS-001 (ASIL B): acceleration above the ACC envelope is applied
| Cut set | Elements | Ratings | Basis |
|---|---|---|---|
| A-SPURIOUS | actuator | D | element rated for the goal |
| B-UNDETECTED | bus | D | element rated for the goal |
| A-UNGATED AND C-ACCEL-HIGH | actuator, controller | D, B(D) | element rated for the goal |
| C-ACCEL-HIGH AND M-APPROVE-LIMIT | controller, monitor | B(D), B(D) | element rated for the goal |

### REQ-ACCS-002 (ASIL D): deceleration above the ACC envelope is applied
| Cut set | Elements | Ratings | Basis |
|---|---|---|---|
| A-SPURIOUS | actuator | D | element rated for the goal |
| B-UNDETECTED | bus | D | element rated for the goal |
| A-UNGATED AND C-DECEL-HIGH | actuator, controller | D, B(D) | element rated for the goal |
| C-DECEL-HIGH AND M-APPROVE-LIMIT | controller, monitor | B(D), B(D) | decomposition |

### REQ-ACCS-003 (ASIL B): acceleration toward a close vehicle continues
| Cut set | Elements | Ratings | Basis |
|---|---|---|---|
| A-SPURIOUS | actuator | D | element rated for the goal |
| B-UNDETECTED | bus | D | element rated for the goal |
| R-MISSED | radar | B | element rated for the goal |
| A-UNGATED AND C-ACCEL-CLOSE | actuator, controller | D, B(D) | element rated for the goal |
| C-ACCEL-CLOSE AND M-APPROVE-CLOSE | controller, monitor | B(D), B(D) | element rated for the goal |

### REQ-ACCS-004 (ASIL B): a frozen or invalid command keeps being applied
| Cut set | Elements | Ratings | Basis |
|---|---|---|---|
| A-SPURIOUS | actuator | D | element rated for the goal |
| B-UNDETECTED | bus | D | element rated for the goal |
| A-HOLD AND C-FROZEN | actuator, controller | D, B(D) | element rated for the goal |
| A-UNGATED AND C-FROZEN | actuator, controller | D, B(D) | element rated for the goal |

### REQ-ACCS-007 (ASIL A): ACC braking is lost without a takeover request
| Cut set | Elements | Ratings | Basis |
|---|---|---|---|
| C-BRAKE-LOST | controller | B(D) | element rated for the goal |
| A-NO-TOR AND C-FROZEN | actuator, controller | D, B(D) | element rated for the goal |

