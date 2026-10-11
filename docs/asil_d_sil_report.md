# ASIL D by decomposition: monitor and system SIL report

ACC and AEB carry ASIL D safety goals (assumed, see `docs/safety/acc_aeb_safety_concept.md`). The
chain reaches them by decomposition: the existing controllers (rated B(D)), independent monitors
B(D), and an actuator ECU that applies only approved commands (assumption of use). This report
covers what the toolchain generated and verified for that decomposition.

## What was added
| Element | Inputs | Generated | Verified by |
|---|---|---|---|
| `acc_monitor` (B(D)) | 10 EARS requirements, DBC extract, model, 2 feature files | monitor node C++, codec, component, SIL | 30 component SIL tests, codec and robustness tests |
| `aeb_monitor` (B(D)) | 10 EARS requirements, DBC extract, model, 2 feature files | same | 28 component SIL tests, codec and robustness tests |
| `acc_system` (D) | 4 safety goals + allocation + availability, FTTI per goal | system SIL harness and tests | 14 system SIL tests with node-output faults |
| `aeb_system` (D) | 2 safety goals + allocation + availability, FTTI per goal | same | 14 system SIL tests with node-output faults |
| Rules | GEN-065 (FTTI budget from the monitor model), GEN-066 (node interface consistency) | — | negative tests in the qualification suite |

## How a monitor is tested on its own
A stimulus node from the plant (`CommandStub`) stands in for the controller. It sends
`AccCommand` / `AebCommand` frames with values set by the scenario, encoded and E2E-protected with
the plant's own tables. The actuator ECU model (`MonitoredActuator`) is the observer. It decodes
the monitor's verdict independently, applies only approved commands to the vehicle, and records
when the monitor and the actuator entered their safe states. Scenario values come from the
monitor requirements, for example:
- the limit plus tolerance, 2.1 m/s²;
- the rate limits, 3.5 / 6 m/s³;
- the TTC thresholds plus margin, 2.1 / 1.5 s;
- the debounce, 0.5 s;
- the timeout, 60 ms.

## How the system is tested
Both generated nodes step on one bus every cycle, controller first. Faults are injected on their
output frames after they leave the node:

| Fault | Represents | Step |
|---|---|---|
| wrong signal value, CRC recomputed | logic fault inside the controller | `the controller sends <Signal> = <v> with a valid CRC for <n> frames` |
| sender stops | crashed node, bus-off | `the command sender stops` |
| sender frozen (same frame repeated) | stuck task | `the command sender freezes` |
| payload bit flip after protection | bus corruption | `the next <n> command frames are corrupted` |
| monitor stops | monitor crash | `the monitor sender stops` |

Each safety-goal scenario asserts three things:
- the actuator entered its safe state within the goal's fault detection budget (FTTI minus the
  actuator reaction time, resolved from the system model);
- the applied and the actual vehicle acceleration or deceleration never crossed the goal's
  threshold;
- a driver takeover was requested.

The availability scenarios rerun representative ACC and AEB situations with the monitor in the
loop: following, cut-in, a lead vehicle braking, driver override, and the AEB performance cases.
They assert the same requirement values as the controller's own SIL, and also that the monitor
never intervened and every command was applied.

## Findings
1. **Name collision between a DBC message and the component.** The first monitor DBC called its
   output message `AccMonitor`, the same name as the component class. The generated C++ did not
   compile. GEN-060 now rejects a component named like a DBC message, before any code is generated.
2. **Plant bug in frozen-sender injection.** A freeze on an id that had no earlier fault
   specification replayed nothing, so the frames passed unchanged. The monitor SIL caught it (the
   command timeout was not reached). The plant now remembers the last frame of every node tx id.
3. **Mutation testing found weak monitor scenarios:**
   - a debounce counter that never reset survived, until a scenario with two short acceleration
     bursts was added;
   - a braking justification that was never reset survived, until a scenario with a second braking
     onset after the target pulled away was added.
4. **MC/DC found missing scenarios:**
   - warning release by lane loss and by kickdown;
   - the 150 m relevance limit;
   - full braking for a receding target;
   - braking while the radar is lost;
   - an ACC target just inside 200 m. This one also detected a mutant of the 200 m range that the
     earlier scenarios missed.
5. **MC/DC found infeasible conditions in the generated code:**
   - range checks on fields whose physical range fills the whole bit field;
   - a redundant post-rounding check in the encoder;
   - `last_counter_ >= 0` in the monitors, which always holds while MONITORING.

   The templates now emit only feasible conditions, with the same behaviour.

## Mutation campaigns
MUTATION_TABLE

## Structural coverage (generated product code, all ASIL D-derived files)
| Measure | Result |
|---|---|
| MC/DC | 100 % in every file, except 2 justified conditions (AEB standstill guard, needs a reversing target) |
| Branches | codecs 100 %; components 94 %; logic 95–100 % |
| Gate | `python3 -m tools.coverage`; see `reports/coverage.md` after a coverage run |

## Limits
- The safety goals, ASILs, FTTIs and the actuator reaction time are assumptions (no HARA).
- The actuator ECU is a SIL model of its assumptions of use, not an implementation.
- Both elements read the same radar object; the sensor's integrity is an assumption (AoU-5).
- Timing is simulated in 20 ms ticks; WCET and scheduling on the target are not measured.
