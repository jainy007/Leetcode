// Decorator around a compute backend that injects timing and data faults for SIL scenarios.
#pragma once

#include <cstddef>
#include <cstdint>

#include "ea/gpu/backend.hpp"
#include "ea/sil/virtual_clock.hpp"

namespace ea::sil {

class FaultInjectingBackend final : public gpu::IComputeBackend {
public:
    FaultInjectingBackend(gpu::IComputeBackend& inner, VirtualClock& clock, std::uint16_t output_tensor) noexcept
        : inner_(inner), clock_(clock), output_tensor_(output_tensor) {}

    // Every execute() advances the virtual clock by this amount.
    void set_execution_delay_ms(int ms) noexcept { delay_ = static_cast<osal::Nanoseconds>(ms) * osal::kNsPerMs; }
    // Every download of the output tensor has element `index` replaced by `value` (f32 tensors).
    void overwrite_output(std::size_t index, float value) noexcept {
        overwrite_ = true;
        overwrite_index_ = index;
        overwrite_value_ = value;
    }

    const char* name() const noexcept override { return "fault_injecting"; }
    Result<void> configure(Span<const gpu::TensorDesc> tensors, Span<const gpu::OpDesc> ops) noexcept override {
        return inner_.configure(tensors, ops);
    }
    Result<void> upload(std::uint16_t tensor, ConstByteSpan bytes) noexcept override {
        return inner_.upload(tensor, bytes);
    }
    Result<void> seal() noexcept override { return inner_.seal(); }
    Result<void> execute() noexcept override;
    Result<void> download(std::uint16_t tensor, ByteSpan bytes) noexcept override;
    std::uint32_t allocation_count() const noexcept override { return inner_.allocation_count(); }

private:
    gpu::IComputeBackend& inner_;
    VirtualClock& clock_;
    std::uint16_t output_tensor_;
    osal::Nanoseconds delay_ = 0;
    bool overwrite_ = false;
    std::size_t overwrite_index_ = 0U;
    float overwrite_value_ = 0.0F;
};

}  // namespace ea::sil
