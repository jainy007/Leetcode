// Deterministic CPU reference backend. Used for SIL and as the numeric oracle for the
// accelerator backends (rule GPU-007). Memory comes from a caller-provided static arena.
#pragma once

#include <array>

#include "ea/gpu/backend.hpp"

namespace ea::gpu {

class CpuRefBackend final : public IComputeBackend {
public:
    static constexpr std::size_t kMaxTensors = 32U;
    static constexpr std::size_t kAlignment = 64U;

    explicit CpuRefBackend(ByteSpan arena) noexcept : arena_(arena) {}

    const char* name() const noexcept override { return "cpu_ref"; }
    Result<void> configure(Span<const TensorDesc> tensors, Span<const OpDesc> ops) noexcept override;
    Result<void> upload(std::uint16_t tensor, ConstByteSpan bytes) noexcept override;
    Result<void> seal() noexcept override;
    Result<void> execute() noexcept override;
    Result<void> download(std::uint16_t tensor, ByteSpan bytes) noexcept override;
    std::uint32_t allocation_count() const noexcept override { return allocations_; }

private:
    float* f32(std::uint16_t t) noexcept;
    const std::uint8_t* u8(std::uint16_t t) const noexcept;
    std::size_t tensor_bytes(std::uint16_t t) const noexcept;

    ByteSpan arena_;
    Span<const TensorDesc> tensors_{};
    Span<const OpDesc> ops_{};
    std::array<std::size_t, kMaxTensors> offsets_{};
    std::array<bool, kMaxTensors> uploaded_{};
    std::uint32_t allocations_ = 0U;
    bool configured_ = false;
    bool sealed_ = false;
};

}  // namespace ea::gpu
