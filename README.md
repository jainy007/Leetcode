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

AI agents may *propose* requirements, scenarios and models, and review results. They may never
write generated code, the generator, or its templates. See [CLAUDE.md](CLAUDE.md); CI enforces this.

## Layout
| Path | Content |
|---|---|
| `runtime/` | Hand-written ASIL runtime: `Result`, `Span`, `StaticVector`, CRC8/16/32-P4, SHA-256, DBC bit codec, SOME/IP, E2E P4, POSIX/QNX OSAL (periodic task, real-time setup, shared memory), SPSC ring, seqlock, shm channel, supervision, GPU backend API with `cpu_ref`, `cuda` and `qnn` backends |
| `tools/gen/` | The generator (Python + Jinja2): loader → schema → semantic rules (GEN-xxx) → patterns → templates |
| `templates/`, `schemas/`, `sil/steps/` | Generator configuration (qualified with the tool) |
| `tools/lint/`, `tools/trace/` | Requirements, Gherkin and model consistency checks; trace report |
| `examples/` | Inputs: `adas_input_gate` (ASIL B interface gate), `gpu_inference_stage` (inference + ASIL monitor) |
| `generated/` | Generator output (committed, never edited by hand) |
| `docs/rules/` | CPU, GPU, multiprocess/IPC, domain and generator rule sets with IDs |
| `docs/safety/` | Tool classification, qualification plan, safety manual for the runtime |
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
