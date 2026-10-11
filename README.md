# Embedded Agent: rule-based C++ generation for automotive POSIX platforms

Safety islands (MCU/RTOS) already have an accepted chain: **requirements → Gherkin → model-based
design → qualified code generator → SIL**. Performance cores have none. These are the POSIX
domains (Linux/PREEMPT_RT, QNX, NVIDIA DRIVE OS, Snapdragon Ride) that run C++ for ADAS/AD,
HMI and IVI, plus GPU/NPU code. This repository provides that chain for them.

```
requirements.yaml (EARS)        every requirement is covered by >= 1 scenario
      ▼
features/*.feature (Gherkin)    only steps from the controlled catalogue (sil/steps/*.yaml)
      ▼
model.yaml (+ DBC)              "MBD for C++": messages, E2E, state machines, tasks, GPU graphs
      ▼
python3 -m tools.gen            deterministic, rule-based, qualified (ISO 26262-8 §11). No AI here.
      ▼
generated/                      C++17 + SIL tests (GoogleTest) + trace.json
      ▼
runtime/ + sil/                 hand-written, reviewed once: OSAL, codecs, E2E, IPC, supervision, GPU backends
      ▼
ctest + python3 -m tools.trace  trace matrix: req → scenario → model element → code → test → result
```

An agent may develop the whole toolchain: inputs, generator, runtime and plant. The generation step
itself is AI-free. `generated/` is written only by the deterministic generator, and CI fails on any
hand edit. See [CLAUDE.md](CLAUDE.md). New ADAS features: [docs/extending.md](docs/extending.md).

## Layout
| Path | Content |
|---|---|
| `runtime/` | Hand-written ASIL runtime: `Result`, `Span`, `StaticVector`, CRC8/16/32-P4, SHA-256, DBC bit codec, SOME/IP, E2E P4, POSIX/QNX OSAL (periodic task, real-time setup, shared memory), SPSC ring, seqlock, shm channel, supervision, GPU backend API with `cpu_ref`, `cuda` and `qnn` backends (see Status) |
| `tools/gen/` | The generator (Python + Jinja2): loader → schema → semantic rules (GEN-xxx) → patterns → templates |
| `templates/`, `schemas/`, `sil/steps/` | Generator configuration (qualified with the tool) |
| `tools/lint/`, `tools/trace/` | Requirements, Gherkin and model consistency checks; trace report |
| `examples/` | Inputs: `adas_input_gate` (ASIL B interface gate), `gpu_inference_stage` (inference + ASIL monitor), `acc` (adaptive cruise control), `aeb` (FCW + emergency braking) |
| `tools/gen/features/`, `templates/features/` | Feature plug-ins of the `can_function` framework (acc, aeb); new ones via `python3 -m tools.scaffold feature <name>` |
| `tools/mutate/`, `tools/scaffold/` | Reproducible mutation testing per project; feature scaffolding |
| `sil/plant/` | Hand-written closed-loop plant: vehicles, radar, driver, CAN timing, fault injection; one observer per feature |
| `generated/` | Generator output (committed, never edited by hand) |
| `docs/rules/` | CPU, GPU, multiprocess/IPC, domain and generator rule sets with IDs |
| `docs/safety/` | Tool classification, qualification plan, safety manual for the runtime |
| `bench/`, `docs/performance.md` | Micro-benchmarks and optimization results (gate cycle 500 ns → 48 ns, all outputs bit-identical) |
| `legacy/` | Archived LeetCode practice code (Bazel); not built |

## Quick start
```bash
pip install -r tools/requirements.txt         # pinned: Jinja2, PyYAML, jsonschema
cmake --preset linux-host
cmake --build --preset linux-host
ctest --preset linux-host
python3 -m tools.gen --check                    # generated/ is up to date and was not hand-edited
python3 -m tools.trace --results build/linux-host/test-results
```

Target builds: `cmake --preset linux-aarch64-rt`, `--preset driveos` (CUDA backend) and
`--preset qnx-aarch64`. Each needs its vendor SDK.

## Status
| Area | State |
|---|---|
| Runtime (core, codecs, E2E P4, OSAL POSIX, IPC, supervision, `cpu_ref`) | Implemented. Unit-tested on the Linux host under ASan/UBSan and TSan. |
| Generator, 42 rules, qualification suite | Implemented. Every rule has at least one negative test; output is byte-deterministic. |
| Slice 1: `adas_input_gate` (ASIL B) | 9 requirements, 16 generated SIL tests, all passing. Trace matrix: 9/9 PASSED. |
| Slice 2: `gpu_inference_stage` + monitor | 8 requirements, 20 generated SIL tests plus a two-process integration test, all passing. Trace matrix: 8/8 PASSED. |
| Slice 3: `acc` (closed-loop ACC, ASIL B) | 22 requirements, 46 generated SIL tests against the plant, all passing. Mutation testing: 33/35 detected, 2 equivalent. See `docs/acc_sil_report.md`. |
| Slice 4: `aeb` (FCW + AEB, ASIL B) | 14 requirements, 29 generated SIL tests, all passing. Mutation testing: 24/26 detected, 2 equivalent. See `docs/aeb_sil_report.md`. |
| Extensibility | `can_function` framework + feature plug-ins; `python3 -m tools.scaffold` creates a feature that builds and passes its SIL immediately (checked in CI). See `docs/extending.md`. |
| `cuda` backend | Written (device memory allocated at init, CUDA Graph, bounded event wait). **Not compiled or run in CI yet**: needs a CUDA toolkit or DRIVE target. |
| `qnn` backend | Interface and lifecycle contract only; `execute()` returns `kNotSupported`. Needs the QNN SDK. |
| QNX | Toolchain file and QNX affinity implementation. **Not built yet**: needs the QNX SDP. |
| HMI telltale monitor and IVI patterns | Planned (Phase 5). Rules are in `docs/rules/domains.md`. |
| Coverage | Statement, branch and MC/DC reports via the `linux-host-coverage` preset. Generated codecs are not yet fully covered by the SIL scenarios. |
