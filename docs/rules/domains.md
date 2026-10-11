# Domain profiles: ADAS/AD, HMI/cluster, IVI

Each domain picks a compile profile, a set of mandatory patterns, and its verification depth.

## ADAS / AD (perception, fusion, planning, control interface)
- Typical integrity: QM up to ASIL B(D), reached through decomposition. Monitors are ASIL; complex
  functions (DNN perception, planning) are QM(x) with an ASIL monitor (MP-001).
- Patterns:
  - `input_gate` at every interface boundary (E2E, plausibility, degradation);
  - `gpu_stage` for inference;
  - a monitor in a separate process.
- Mandatory rules: all of CPU, GPU, MP and IPC.
- Verification:
  - Generated SIL from Gherkin;
  - MC/DC on ASIL units;
  - back-to-back testing of each GPU backend against `cpu_ref`;
  - deterministic replay of recorded input (SIL virtual clock).

## HMI / instrument cluster
- Typical integrity: QM rendering plus ASIL B telltales (warning lamps).
- Required split:
  - a QM renderer (Qt/QML, Kanzi, Unity...) draws everything;
  - an ASIL telltale monitor checks a CRC of the rendered telltale regions (read back from the
    display controller's safety channel or a framebuffer copy) against the commanded state, with
    a bounded signal-to-pixel latency.
- Pattern: `telltale_monitor` (Phase 5). It reuses `input_gate` for the commanded state and a
  CRC region table generated from the model.
- Rules:
  - CPU and MP rules for the monitor;
  - the renderer uses the `qm` profile;
  - the renderer can never inhibit the monitor's fault reaction (MP-001).

## IVI (infotainment)
- Typical integrity: QM.
- Profile `qm`: exceptions and heap are allowed in application code, but interfaces still come
  from the model (IPC-001) and codecs are still generated (IPC-003), so vehicle signals entering
  IVI are decoded by the same qualified codecs as in ADAS.
- Gateways from vehicle buses into IVI are `input_gate` instances, so malformed traffic stops at
  the boundary.
