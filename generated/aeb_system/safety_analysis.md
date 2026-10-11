<!--
GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
Project:   aeb_system (pattern: can_system)
Inputs:    sha256:3c38374823eec23e9813f37d7d20dee2e10abbfc8a620b8f99e7ed929b104323
Regenerate: python3 -m tools.gen
-->
# Safety analysis of AebSystem (generated)

Inputs: `examples/aeb_system/safety/hara.yaml` (HARA), `examples/aeb_system/safety/fta.yaml`
(fault trees) and `examples/aeb_system/model.yaml` (safety goals, FTTI, actuator). S, E and C are
engineering judgement with the rationale given in the HARA. Everything else on this page is computed
by the generator: ASIL (ISO 26262-3 table 4, GEN-067), FTTI (kinematic models, GEN-069), minimal
cut sets and the single-point check (GEN-068), and the FDTI budget (GEN-065).

## Assumptions
| ID | Assumption | Source |
|---|---|---|
| A-01 | A driver reacts to an unexpected event, including a takeover request or brake lights ahead, after 1.25 s. | M. Green, "How long does it take to stop?", Transportation Human Factors 2(3), 2000 |
| A-02 | Ordinary drivers brake at 6 m/s2 in an emergency. | engineering estimate |
| A-03 | A following vehicle keeps a headway of 0.8 s. | engineering estimate (conservative within observed motorway headways) |
| A-04 | AEB requests at most 8 m/s2 (full braking) or 4 m/s2 (partial braking), as in REQ-AEB-002/003. | examples/aeb/requirements.yaml |
| A-05 | In its safe state the actuator ECU ramps braking out at 20 m/s3 (AoU-3), within 100 ms of the monitor's verdict (AoU-4). | system model actuator.reaction_ms and actuator.ramp_out_mps3 |
| A-06 | AEB supplements an attentive driver; the driver remains responsible for avoiding collisions (UN R152 context). | UN Regulation No. 152 |
| A-07 | Severity follows the impact speed of a car-to-car collision. S3 if more than 40 km/h is plausible, S2 for 20-40 km/h, S1 below. | ISO 26262-3 Annex B examples, engineering estimate |

## Hazards (HAZOP guide words)
| ID | Guide word | Function | Malfunction |
|---|---|---|---|
| H-AEB-01 | unintended | emergency braking | full braking without a collision threat |
| H-AEB-02 | unintended | emergency braking | partial braking without a collision threat |
| H-AEB-03 | stuck | emergency braking | braking continues after the threat has gone or the command freezes |
| H-AEB-04 | no | emergency braking | no braking when a collision is imminent |
| H-AEB-05 | unintended | forward collision warning | warning without a collision threat |

## Hazardous events and ASIL
| ID | Hazard | Situation | S | E | C | ASIL | Safety goal |
|---|---|---|---|---|---|---|---|
| HE-AEB-01 | H-AEB-01 | Free road at 50-130 km/h with a following vehicle at 0.8 s headway. | S3 | E4 | C3 | **D** | REQ-AEBS-001 |
| HE-AEB-02 | H-AEB-02 | Free road at 50-130 km/h with a following vehicle at 0.8 s headway. | S2 | E4 | C2 | **B** | REQ-AEBS-001 |
| HE-AEB-03 | H-AEB-03 | After an AEB intervention, the obstacle has left the lane; a following vehicle at 0.8 s headway, 50-80 km/h. | S3 | E2 | C3 | **B** | REQ-AEBS-002 |
| HE-AEB-04 | H-AEB-04 | A stationary or slower vehicle ahead and the driver does not react. | S3 | E2 | C1 | **QM** | — |
| HE-AEB-05 | H-AEB-05 | Any driving situation; the warning sounds without a threat. | S0 | E4 | C0 | **QM** | — |

### Rationale
- **HE-AEB-01**: S3: A rear-end collision by the following car can exceed 40 km/h after unexpected full braking. E4: Driving with a following vehicle at a short headway takes more than 10 % of the driving time; AEB is active whenever the car moves. C3: With 0.8 s headway and a 1.25 s reaction, the follower cannot avoid an unexpected 8 m/s2 braking; the ego driver has no means to cancel it in time.
- **HE-AEB-02**: S2: The follower brakes harder (6 m/s2) than the partial 4 m/s2, so the impact speed of any collision stays moderate. E4: As HE-AEB-01: a following vehicle at short headway. C2: Most followers avoid the collision once they react. The kinematic FTTI shows no collision for any duration at 0.8 s headway.
- **HE-AEB-03**: S3: Full braking that continues needlessly can lead to a rear-end collision above 40 km/h. E2: Needs an AEB intervention first, and AEB interventions are rare (well below 1 % of the driving time). C3: As HE-AEB-01: the follower cannot avoid it.
- **HE-AEB-04**: S3: A collision with the vehicle ahead at more than 40 km/h is plausible. E2: Situations where AEB must intervene because the driver does not are rare. C1: AEB supplements an attentive driver (A-06). Losing it leaves the driver in the situation of a car without AEB, which is controllable in general.
- **HE-AEB-05**: S0: A warning alone causes no injury. E4: AEB is active whenever the car moves. C0: A false warning does not affect vehicle motion.

## Safety goals, FTTI and detection budget
FTTI derived = the longest fault duration (fault → actuator safe state) for which every reference
situation still avoids the collision; the model FTTI may be smaller (conservative). Budget check:
FDTI of every mechanism + actuator reaction (100 ms) ≤ model FTTI.

| Safety goal | ASIL | Derived FTTI | Model FTTI | Mechanisms | Worst FDTI | FDTI + reaction |
|---|---|---|---|---|---|---|
| REQ-AEBS-001 | D | 780 ms | 780 ms | braking_justification, request_limit, request_outside_braking | 20 ms | 120 ms |
| REQ-AEBS-002 | B | 780 ms | 780 ms | command_timeout | 60 ms | 160 ms |

### FTTI derivation per reference situation
**HE-AEB-01** (rear_end_by_follower):

| Situation | Derived FTTI |
|---|---|
| driver_decel_mps2=6.0, fault_decel_mps2=8.0, follower_decel_mps2=6.0, headway_s=0.8, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=50 | 780 ms |
| driver_decel_mps2=6.0, fault_decel_mps2=8.0, follower_decel_mps2=6.0, headway_s=0.8, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=80 | 1280 ms |
| driver_decel_mps2=6.0, fault_decel_mps2=8.0, follower_decel_mps2=6.0, headway_s=0.8, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=130 | 2020 ms |

**HE-AEB-02** (rear_end_by_follower):

| Situation | Derived FTTI |
|---|---|
| driver_decel_mps2=6.0, fault_decel_mps2=4.0, follower_decel_mps2=6.0, headway_s=0.8, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=50 | 10000 ms |
| driver_decel_mps2=6.0, fault_decel_mps2=4.0, follower_decel_mps2=6.0, headway_s=0.8, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=130 | 10000 ms |

**HE-AEB-03** (rear_end_by_follower):

| Situation | Derived FTTI |
|---|---|
| driver_decel_mps2=6.0, fault_decel_mps2=8.0, follower_decel_mps2=6.0, headway_s=0.8, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=50 | 780 ms |
| driver_decel_mps2=6.0, fault_decel_mps2=8.0, follower_decel_mps2=6.0, headway_s=0.8, ramp_out_mps3=20.0, reaction_s=1.25, speed_kph=80 | 1280 ms |

## Fault trees: minimal cut sets
### REQ-AEBS-001 (ASIL D): unjustified automatic braking is applied
| Cut set | Elements | Ratings | Basis |
|---|---|---|---|
| A-SPURIOUS | actuator | D | element rated for the goal |
| B-UNDETECTED | bus | D | element rated for the goal |
| R-GHOST | radar | D | element rated for the goal |
| A-UNGATED AND C-BRAKE-UNJUST | actuator, controller | D, B(D) | element rated for the goal |
| C-BRAKE-UNJUST AND M-APPROVE-UNJUST | controller, monitor | B(D), B(D) | decomposition |

### REQ-AEBS-002 (ASIL B): braking continues after the threat or on a frozen command
| Cut set | Elements | Ratings | Basis |
|---|---|---|---|
| A-SPURIOUS | actuator | D | element rated for the goal |
| B-UNDETECTED | bus | D | element rated for the goal |
| A-HOLD AND C-BRAKE-STUCK | actuator, controller | D, B(D) | element rated for the goal |
| A-UNGATED AND C-BRAKE-STUCK | actuator, controller | D, B(D) | element rated for the goal |
| C-BRAKE-STUCK AND M-APPROVE-UNJUST | controller, monitor | B(D), B(D) | element rated for the goal |

