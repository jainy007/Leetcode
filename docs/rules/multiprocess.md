# Multiprocessing, IPC and supervision rules

| ID | Rule | Rationale | Enforcement |
|---|---|---|---|
| MP-001 | Software of different ASIL levels (or ASIL and QM) runs in **separate processes**. A lower-integrity process can only reach a higher one through a typed channel. | Freedom from interference (ISO 26262-6 Annex D): memory. | deployment model; `ShmPublisher` / `ShmSubscriber` |
| MP-002 | Shared memory is created by exactly one owner with `O_EXCL`, has a fixed size, and is validated by magic and payload size on open. | No ambiguity about ownership; size mismatches are detected. | `ea::osal::SharedMemory`, `ShmSubscriber::open` |
| MP-003 | Streaming data uses a wait-free SPSC ring. The producer never blocks on the consumer. | A slow or dead consumer cannot stall the producer. | `ea::ipc::SpscRing` |
| MP-004 | "Latest value" data uses a seqlock with bounded read retries. | The reader cannot block the writer; reads are bounded. | `ea::ipc::Seqlock` |
| MP-005 | Data in shared memory is trivially copyable and address-free: no pointers, no virtual functions, lock-free atomics only. | Valid in every address space. | `static_assert`s in the IPC templates |
| MP-006 | Every safety-relevant process reports alive, deadline and logical supervision. Supervision failures latch and are escalated to the platform watchdog. | Timing and execution FFI. | `ea::sup::*` |
| MP-007 | Resource budgets per process: CPU (cgroups v2 `cpu.max` on Linux, adaptive partitions on QNX), memory (`memory.max`), and file descriptors. | Timing FFI under load. | deployment manifest (Phase 5) |
| MP-008 | Startup order, restart policy and the safe state for each process are declared in the deployment model. A crashed ASIL process is never silently restarted without a state check. | Controlled degradation. | execution manager (Phase 5) |
| MP-009 | Inter-ECU and inter-SoC data carries E2E protection: counter, data ID, CRC, and a timeout per receiver. | Communication FFI (ISO 26262-6 Annex D). | `ea::e2e::P4*`; the input_gate pattern |

## IPC and codec rules

| ID | Rule |
|---|---|
| IPC-001 | Interfaces are defined in the model (or imported from DBC / ARXML), never in hand-written structs. |
| IPC-002 | Every received signal carries a physical range in the model. Values outside it are a fault (`kOutOfRange`) and are never clamped silently. |
| IPC-003 | Codecs are endian-explicit and bounds-checked, and never type-pun (`reinterpret_cast` of a frame into a struct is forbidden). |
| IPC-004 | SOME/IP headers are validated: service, method, interface version, message type and length. |
| IPC-005 | Physical-to-raw conversion rounds to nearest and is range-checked before it is packed. |
