# CPU-side rules (performance cores, POSIX)

Applies to: runtime code, generated code, and any code that implements generated interfaces.
Profile `asil` = all rules mandatory. Profile `qm` = rules marked **(asil)** are recommended only.
Each rule cross-references MISRA C++:2023 (M23) / AUTOSAR C++14 (A14) where one exists.

| ID | Rule | Rationale | Enforcement |
|---|---|---|---|
| CPU-001 **(asil)** | No exceptions. Errors are returned as `ea::Result<T>`. | Bounded control flow, no hidden paths. A14 A15-0-1 | `-fno-exceptions` in `ea::profile_asil` |
| CPU-002 **(asil)** | No RTTI (`dynamic_cast`, `typeid`). | Smaller binaries; designs must not need type discovery. | `-fno-rtti` |
| CPU-003 | Every return value is checked or explicitly discarded with `(void)`. | M23 0.1.2 | `[[nodiscard]]` on `Result`, `-Werror` |
| CPU-004 **(asil)** | No heap allocation after initialization. Use `StaticVector`, static arenas and pools. | No fragmentation and no allocator latency in the cyclic phase. A14 A18-5-5 | review; generated code has no `new`/`malloc` |
| CPU-005 **(asil)** | Lock memory (`lock_memory()`) and prefault stacks before the cyclic phase. | No page faults while running. | execution manager startup check |
| CPU-006 | Periodic work uses absolute-time sleeps (`PeriodicTask`). Overruns are counted and the schedule is re-anchored. | No drift and no catch-up bursts. | `ea::osal::PeriodicTask` |
| CPU-007 | Real-time threads have an explicit `SCHED_FIFO` priority and a CPU mask from the deployment model. Never rely on the scheduler defaults. | Predictable timing and isolation. | `TaskConfig` |
| CPU-008 | Timing and supervision use the monotonic clock only (`IClock`). Never `CLOCK_REALTIME` or `std::chrono::system_clock`. | Wall-clock time can jump. | review, clang-tidy banned-API list |
| CPU-009 | All loops are bounded by a compile-time constant or a validated input length. | WCET analysis. M23 6.8 | review; the generator emits bounded loops only |
| CPU-010 | Counters and debouncers saturate (`sat_inc`); unsigned wrap is used only for sequence counters defined by a protocol. | No silent wrap. | `ea/core/saturating.hpp` |
| CPU-011 | Data shared between threads uses either a lock-free SPSC queue, a seqlock, or a mutex with priority inheritance. No ad-hoc `volatile` flags. | Data-race freedom; TSan clean. | TSan CI job |
| CPU-012 | Data that is written by different threads sits on separate 64-byte cache lines (`alignas(kCacheLine)`). | No false sharing. | review |
| CPU-013 | Every conversion is explicit (`static_cast`) and every narrowing is checked. | M23 7.0 | `-Wconversion -Wsign-conversion -Werror` |
| CPU-014 | Integer widths are fixed (`std::uint32_t` etc.) on interfaces and in codecs. | Portability across aarch64/x86. | review |
| CPU-015 | No recursion in ASIL code paths. | Bounded stack. M23 8.2.10 | review; the generator never emits recursion |
| CPU-016 | NUMA and cluster awareness: memory used by a pinned task is allocated by that task's thread at init, so first-touch places it on the local node. | Avoids cross-node latency on big.LITTLE / multi-cluster SoCs. | review |
