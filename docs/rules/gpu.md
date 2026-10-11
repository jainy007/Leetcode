# GPU / NPU rules (CUDA, TensorRT, Qualcomm QNN)

Applies to every inference or compute stage, whatever its backend. The generated `gpu_stage`
pattern implements these rules by construction. Hand-written kernels must follow them too.

| ID | Rule | Rationale | How the pattern satisfies it |
|---|---|---|---|
| GPU-001 | Allocate all device, pinned-host and workspace memory during INIT. `cudaMalloc*`, `cudaHostAlloc` and `QnnMem_register` are forbidden in RUN. | Allocation latency is unbounded and can fail. | `configure()` allocates; `allocation_count()` is checked in SIL to stay constant after `seal()` |
| GPU-002 | The work per frame is a pre-recorded graph (CUDA Graph or finalized QNN graph). Kernels are not launched ad hoc in RUN. | Launch overhead that does not vary, and a fixed op order. | `seal()` captures the graph |
| GPU-003 | Every wait is bounded. No unbounded `cudaStreamSynchronize` in ASIL monitoring paths. Poll an event against a deadline. | A hung accelerator must not hang the CPU task. | the backend `execute()` contract and the stage deadline |
| GPU-004 | Every accelerator API return code is checked and mapped to `ea::ErrorCode`. | No silent failure (sticky CUDA errors). | `EA_CUDA_TRY` / `EA_QNN_TRY` macros |
| GPU-005 | Weights and engines are immutable after INIT. Only input tensors are written in RUN. | Integrity of the model under test. | `upload()` rejects constant tensors after `seal()` |
| GPU-006 | Model artefacts (weights, engines, context binaries) are verified against the SHA-256 recorded in the model before use. | The model that was validated is the model that runs. | generated `init()` hashes each file |
| GPU-007 | Each backend is compared with the deterministic CPU reference (`cpu_ref`) within a tolerance declared in the model. `cpu_ref` builds with `-ffp-contract=off`. | Numeric correctness oracle. | golden scenarios in SIL; the same scenarios run as HIL on target |
| GPU-008 | Outputs are checked for plausibility (finite values, value ranges, invariants such as softmax summing to 1) before they leave the stage. An independent monitor repeats the checks in a separate process. | Detects silent corruption; FFI (QM producer, ASIL monitor). | generated `check_plausibility()` plus the monitor |
| GPU-009 | Deadline: measure the stage's runtime with the monotonic clock and mark a result over its deadline `DEADLINE_MISS`. Never forward such a result as valid. | Timing is part of correctness for ADAS. | generated `run()` |
| GPU-010 | One CUDA context and stream set per process, created at INIT, with priorities from the deployment model. No default stream (`cudaStreamPerThread` off). | Isolation and predictable scheduling. | cuda backend |
| GPU-011 | No dynamic shapes in ASIL pipelines. Tensor shapes are fixed in the model. | Bounded memory and timing. | schema: shapes are integer lists |
