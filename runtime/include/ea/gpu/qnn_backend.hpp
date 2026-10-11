// Qualcomm QNN backend (Snapdragon Ride, Hexagon NPU / HTP). Built only with -DEA_GPU_QNN=ON.
//
// STATUS: interface and lifecycle contract only. execute() returns kNotSupported until the
// QNN SDK integration is implemented and validated on target (tracked in docs/rules/gpu.md).
//
// Intended mapping (QNN SDK >= 2.x):
//   configure(): QnnBackend_create, QnnDevice_create, QnnContext_createFromBinary with the
//                context binary named in the model (SHA-256 verified like weights, GPU-006);
//                QnnGraph_retrieve; register all I/O buffers once with QnnMem_register  (GPU-001)
//   seal():      graph is already finalized offline; verify tensor shapes/dtypes against the model
//   execute():   QnnGraph_execute with a signal + timeout (QnnSignal_*), bounded     (GPU-003)
// Numerics: HTP runs quantized; the model's reference_tolerance must be chosen for that, and the
// cpu_ref comparison (GPU-007) is the acceptance gate on target.
#pragma once

#include "ea/gpu/backend.hpp"

namespace ea::gpu {

class QnnBackend final : public IComputeBackend {
public:
    struct Config {
        const char* backend_library;   // e.g. "libQnnHtp.so"
        const char* context_binary;    // offline-prepared graph
        const char* context_sha256;    // from the model
        std::int64_t execute_timeout_ns;
    };

    explicit QnnBackend(const Config& cfg) noexcept : cfg_(cfg) {}

    const char* name() const noexcept override { return "qnn"; }
    Result<void> configure(Span<const TensorDesc> tensors, Span<const OpDesc> ops) noexcept override;
    Result<void> upload(std::uint16_t tensor, ConstByteSpan bytes) noexcept override;
    Result<void> seal() noexcept override;
    Result<void> execute() noexcept override;
    Result<void> download(std::uint16_t tensor, ByteSpan bytes) noexcept override;
    std::uint32_t allocation_count() const noexcept override { return 0U; }

private:
    Config cfg_;
};

}  // namespace ea::gpu
