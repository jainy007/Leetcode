#include "ea/gpu/cpu_ref_backend.hpp"

#include <cmath>
#include <cstring>

namespace ea::gpu {

namespace {

constexpr std::size_t align_up(std::size_t v, std::size_t a) noexcept {
    return (v + a - 1U) / a * a;
}

}  // namespace

std::size_t CpuRefBackend::tensor_bytes(std::uint16_t t) const noexcept {
    return static_cast<std::size_t>(tensors_[t].elements) * dtype_size(tensors_[t].dtype);
}

float* CpuRefBackend::f32(std::uint16_t t) noexcept {
    return reinterpret_cast<float*>(arena_.data() + offsets_[t]);  // NOLINT: arena is 64-byte aligned
}

const std::uint8_t* CpuRefBackend::u8(std::uint16_t t) const noexcept {
    return arena_.data() + offsets_[t];
}

Result<void> CpuRefBackend::configure(Span<const TensorDesc> tensors, Span<const OpDesc> ops) noexcept {
    if (configured_ || sealed_) {
        return ErrorCode::kAlreadyInitialized;
    }
    if (tensors.size() > kMaxTensors) {
        return ErrorCode::kResourceExhausted;
    }
    EA_TRY(validate_graph(tensors, ops));
    if ((reinterpret_cast<std::uintptr_t>(arena_.data()) % kAlignment) != 0U) {  // NOLINT
        return ErrorCode::kInvalidArgument;
    }
    tensors_ = tensors;
    ops_ = ops;
    std::size_t offset = 0U;
    for (std::uint16_t i = 0U; i < tensors.size(); ++i) {
        offsets_[i] = offset;
        offset = align_up(offset + tensor_bytes(i), kAlignment);
        if (offset > arena_.size()) {
            return ErrorCode::kResourceExhausted;
        }
        ++allocations_;
    }
    std::memset(arena_.data(), 0, offset);
    configured_ = true;
    return ok_result();
}

Result<void> CpuRefBackend::upload(std::uint16_t tensor, ConstByteSpan bytes) noexcept {
    if (!configured_) {
        return ErrorCode::kNotInitialized;
    }
    if (tensor >= tensors_.size() || bytes.size() != tensor_bytes(tensor)) {
        return ErrorCode::kInvalidArgument;
    }
    if (tensors_[tensor].is_const && sealed_) {
        return ErrorCode::kAlreadyInitialized;  // weights are immutable after INIT
    }
    if (!tensors_[tensor].is_const && !sealed_) {
        return ErrorCode::kNotInitialized;  // inputs only during RUN
    }
    std::memcpy(arena_.data() + offsets_[tensor], bytes.data(), bytes.size());
    uploaded_[tensor] = true;
    return ok_result();
}

Result<void> CpuRefBackend::seal() noexcept {
    if (!configured_) {
        return ErrorCode::kNotInitialized;
    }
    if (sealed_) {
        return ErrorCode::kAlreadyInitialized;
    }
    for (std::uint16_t i = 0U; i < tensors_.size(); ++i) {
        if (tensors_[i].is_const && !uploaded_[i]) {
            return ErrorCode::kNotInitialized;  // every constant must be loaded before RUN
        }
    }
    sealed_ = true;
    return ok_result();
}

Result<void> CpuRefBackend::execute() noexcept {
    if (!sealed_) {
        return ErrorCode::kNotInitialized;
    }
    for (const OpDesc& op : ops_) {
        const std::uint32_t n = tensors_[op.output].elements;
        float* out = f32(op.output);
        switch (op.kind) {
            case OpKind::kNormalize: {
                const std::uint8_t* in = u8(op.input);
                for (std::uint32_t i = 0U; i < n; ++i) {
                    out[i] = static_cast<float>(in[i]) * op.scale + op.shift;
                }
                break;
            }
            case OpKind::kDense: {
                const float* in = f32(op.input);
                const float* w = f32(op.weights);
                const float* b = f32(op.bias);
                for (std::uint32_t j = 0U; j < op.out_features; ++j) {
                    float acc = b[j];
                    for (std::uint32_t i = 0U; i < op.in_features; ++i) {
                        acc += in[i] * w[i * op.out_features + j];
                    }
                    out[j] = acc;
                }
                break;
            }
            case OpKind::kRelu: {
                const float* in = f32(op.input);
                for (std::uint32_t i = 0U; i < n; ++i) {
                    out[i] = (in[i] > 0.0F) ? in[i] : 0.0F;
                }
                break;
            }
            case OpKind::kSoftmax: {
                const float* in = f32(op.input);
                float max = in[0];
                for (std::uint32_t i = 1U; i < n; ++i) {
                    max = (in[i] > max) ? in[i] : max;
                }
                float sum = 0.0F;
                for (std::uint32_t i = 0U; i < n; ++i) {
                    out[i] = std::exp(in[i] - max);
                    sum += out[i];
                }
                for (std::uint32_t i = 0U; i < n; ++i) {
                    out[i] /= sum;
                }
                break;
            }
        }
    }
    return ok_result();
}

Result<void> CpuRefBackend::download(std::uint16_t tensor, ByteSpan bytes) noexcept {
    if (!sealed_) {
        return ErrorCode::kNotInitialized;
    }
    if (tensor >= tensors_.size() || bytes.size() != tensor_bytes(tensor)) {
        return ErrorCode::kInvalidArgument;
    }
    std::memcpy(bytes.data(), arena_.data() + offsets_[tensor], bytes.size());
    return ok_result();
}

}  // namespace ea::gpu
