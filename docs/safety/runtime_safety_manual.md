# Safety manual: ea runtime (assumptions of use)

The runtime (`runtime/`) is hand-written and reviewed. The generated code builds on it. An
integrator using it in an ASIL context must meet the following assumptions.

| ID | Assumption of use |
|---|---|
| AoU-01 | The platform provides POSIX threads, `CLOCK_MONOTONIC`, `clock_nanosleep` with `TIMER_ABSTIME`, `shm_open` and `mmap`. This holds for Linux (PREEMPT_RT recommended), QNX SDP 7.1/8.0, NVIDIA DRIVE OS and Snapdragon Ride (QNX). |
| AoU-02 | `lock_memory()` is called and succeeds before the cyclic phase of an ASIL process (CPU-005). The process has `CAP_IPC_LOCK` (Linux) or the matching QNX ability. |
| AoU-03 | Real-time priorities and CPU masks in the deployment model are applied and verified by the execution manager at startup. `PeriodicTask` ignores failures to set them so that it also works in SIL. |
| AoU-04 | `EA_ASSERT` failures abort the process. The execution manager must detect the termination and enter the safe state defined for the process (MP-008). |
| AoU-05 | Shared-memory channels are created by the producer process before the consumer opens them. Channel names are unique per system. File permissions restrict access to the processes involved (MP-002). |
| AoU-06 | The E2E P4 data IDs are unique across the vehicle network for every protected data element. |
| AoU-07 | The compiler and standard library used for ASIL builds are qualified or validated by the integrator. `cpu_ref` is built with `-ffp-contract=off`, so its numerics are reproducible. |
| AoU-08 | Accelerator backends (`cuda`, `qnn`) are QM unless the vendor supplies a safety manual for the driver stack. ASIL claims rely on the monitor (MP-001, GPU-008), not on the accelerator. |
| AoU-09 | `Seqlock` and `SpscRing` assume one writer (and one reader for the ring). The integrator enforces this through the deployment model. |
| AoU-10 | Health supervision results (`ea::sup`) are reported to a platform watchdog that is independent of the supervised process. |

## Verification evidence
- Unit tests: `tests/runtime/*` (GoogleTest), run under ASan/UBSan and TSan in CI.
- Coverage: the `linux-host-coverage` preset (clang source-based coverage, MC/DC with clang ≥ 18).
- Static analysis: `.clang-tidy` baseline. The customer's qualified MISRA checker is authoritative.
