#include "ea/sil/fault_injecting_backend.hpp"

#include <cstring>

namespace ea::sil {

Result<void> FaultInjectingBackend::execute() noexcept {
    const Result<void> r = inner_.execute();
    clock_.advance(delay_);
    return r;
}

Result<void> FaultInjectingBackend::download(std::uint16_t tensor, ByteSpan bytes) noexcept {
    const Result<void> r = inner_.download(tensor, bytes);
    if (r.ok() && overwrite_ && tensor == output_tensor_ && (overwrite_index_ + 1U) * sizeof(float) <= bytes.size()) {
        std::memcpy(bytes.data() + overwrite_index_ * sizeof(float), &overwrite_value_, sizeof(float));
    }
    return r;
}

}  // namespace ea::sil
