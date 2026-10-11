// CUDA backend (NVIDIA DRIVE Orin/Thor, CUDA >= 11.4). Built only with -DEA_GPU_CUDA=ON.
// No CUDA headers here: handles are opaque so host code stays toolkit independent.
//
//   configure(): one stream (with priority), one event, device buffers for every tensor and
//                pinned staging buffers for graph inputs/outputs                    (GPU-001, GPU-010)
//   seal():      captures H2D copies + kernels + D2H copies into one CUDA Graph     (GPU-002)
//   execute():   cudaGraphLaunch + event poll bounded by sync_timeout              (GPU-003)
//   every CUDA call is checked                                                      (GPU-004)
#pragma once

#include <array>
#include <cstdint>

#include "ea/gpu/backend.hpp"

namespace ea::gpu {

class CudaBackend final : public IComputeBackend {
public:
    static constexpr std::size_t kMaxTensors = 32U;

    struct Config {
        int device;
        int stream_priority;       // cudaDeviceGetStreamPriorityRange: lower value = higher priority
        std::int64_t sync_timeout_ns;  // bound for execute()
    };

    explicit CudaBackend(const Config& cfg) noexcept : cfg_(cfg) {}
    ~CudaBackend() override;
    CudaBackend(const CudaBackend&) = delete;
    CudaBackend& operator=(const CudaBackend&) = delete;

    const char* name() const noexcept override { return "cuda"; }
    Result<void> configure(Span<const TensorDesc> tensors, Span<const OpDesc> ops) noexcept override;
    Result<void> upload(std::uint16_t tensor, ConstByteSpan bytes) noexcept override;
    Result<void> seal() noexcept override;
    Result<void> execute() noexcept override;
    Result<void> download(std::uint16_t tensor, ByteSpan bytes) noexcept override;
    std::uint32_t allocation_count() const noexcept override { return allocations_; }

private:
    void release() noexcept;
    std::size_t bytes(std::uint16_t t) const noexcept;

    Config cfg_;
    Span<const TensorDesc> tensors_{};
    Span<const OpDesc> ops_{};
    std::array<void*, kMaxTensors> device_{};
    std::array<void*, kMaxTensors> staging_{};  // pinned host memory, graph inputs/outputs only
    std::array<bool, kMaxTensors> is_input_{};
    std::array<bool, kMaxTensors> is_output_{};
    std::array<bool, kMaxTensors> uploaded_{};
    void* stream_ = nullptr;
    void* done_ = nullptr;
    void* graph_ = nullptr;
    void* exec_ = nullptr;
    std::uint32_t allocations_ = 0U;
    bool configured_ = false;
    bool sealed_ = false;
};

}  // namespace ea::gpu
