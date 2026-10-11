#include "ea/gpu/backend.hpp"

namespace ea::gpu {

namespace {

bool is_f32(Span<const TensorDesc> t, std::uint16_t i) noexcept {
    return i < t.size() && t[i].dtype == DType::kF32;
}

}  // namespace

Result<void> validate_graph(Span<const TensorDesc> tensors, Span<const OpDesc> ops) noexcept {
    if (tensors.empty() || ops.empty() || tensors.size() >= kNoTensor) {
        return ErrorCode::kInvalidArgument;
    }
    for (const TensorDesc& t : tensors) {
        if (t.elements == 0U) {
            return ErrorCode::kInvalidArgument;
        }
    }
    for (const OpDesc& op : ops) {
        if (op.input >= tensors.size() || op.output >= tensors.size() || op.input == op.output) {
            return ErrorCode::kInvalidArgument;
        }
        const TensorDesc& in = tensors[op.input];
        const TensorDesc& out = tensors[op.output];
        if (out.is_const) {
            return ErrorCode::kInvalidArgument;  // ops never write constants
        }
        switch (op.kind) {
            case OpKind::kNormalize:
                if (in.dtype != DType::kU8 || out.dtype != DType::kF32 || in.elements != out.elements) {
                    return ErrorCode::kInvalidArgument;
                }
                break;
            case OpKind::kDense:
                if (!is_f32(tensors, op.input) || !is_f32(tensors, op.output) || !is_f32(tensors, op.weights) ||
                    !is_f32(tensors, op.bias) || in.elements != op.in_features || out.elements != op.out_features ||
                    tensors[op.weights].elements != op.in_features * op.out_features ||
                    tensors[op.bias].elements != op.out_features || !tensors[op.weights].is_const ||
                    !tensors[op.bias].is_const) {
                    return ErrorCode::kInvalidArgument;
                }
                break;
            case OpKind::kRelu:
            case OpKind::kSoftmax:
                if (!is_f32(tensors, op.input) || !is_f32(tensors, op.output) || in.elements != out.elements) {
                    return ErrorCode::kInvalidArgument;
                }
                break;
        }
    }
    return ok_result();
}

}  // namespace ea::gpu
