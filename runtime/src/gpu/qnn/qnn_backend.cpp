// Placeholder implementation: validates the graph and refuses to run. See ea/gpu/qnn_backend.hpp.
#include "ea/gpu/qnn_backend.hpp"

namespace ea::gpu {

Result<void> QnnBackend::configure(Span<const TensorDesc> tensors, Span<const OpDesc> ops) noexcept {
    EA_TRY(validate_graph(tensors, ops));
    if (cfg_.backend_library == nullptr || cfg_.context_binary == nullptr || cfg_.context_sha256 == nullptr) {
        return ErrorCode::kInvalidArgument;
    }
    return ErrorCode::kNotSupported;
}

Result<void> QnnBackend::upload(std::uint16_t /*tensor*/, ConstByteSpan /*bytes*/) noexcept {
    return ErrorCode::kNotSupported;
}

Result<void> QnnBackend::seal() noexcept {
    return ErrorCode::kNotSupported;
}

Result<void> QnnBackend::execute() noexcept {
    return ErrorCode::kNotSupported;
}

Result<void> QnnBackend::download(std::uint16_t /*tensor*/, ByteSpan /*bytes*/) noexcept {
    return ErrorCode::kNotSupported;
}

}  // namespace ea::gpu
