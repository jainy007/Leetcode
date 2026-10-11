// Accelerator backend abstraction for generated inference stages (rules GPU-001..GPU-008).
//
// Lifecycle (enforced by every backend):
//   configure()  - INIT: validate graph, allocate ALL device/host memory     (GPU-001)
//   upload()     - INIT: constant tensors (weights); RUN: input tensors only  (GPU-005)
//   seal()       - end of INIT: build the executable graph (CUDA Graph / QNN graph), no more allocation
//   execute()    - RUN: launch the sealed graph, synchronous with bounded wait (GPU-003)
//   download()   - RUN: read results
//
// Backends: cpu_ref (reference, SIL), cuda (NVIDIA DRIVE), qnn (Qualcomm Ride).
#pragma once

#include <cstddef>
#include <cstdint>

#include "ea/core/result.hpp"
#include "ea/core/span.hpp"

namespace ea::gpu {

enum class DType : std::uint8_t { kU8, kF32 };

constexpr std::size_t dtype_size(DType t) noexcept {
    return (t == DType::kU8) ? 1U : 4U;
}

struct TensorDesc {
    const char* name;
    DType dtype;
    std::uint32_t elements;
    bool is_const;  // weights/biases: uploaded once during INIT, immutable afterwards
};

enum class OpKind : std::uint8_t {
    kNormalize,  // out[i] = in[i] * scale + shift          (u8 -> f32)
    kDense,      // out[j] = sum_i in[i] * W[i][j] + b[j]   (f32, W row-major [in][out])
    kRelu,       // out[i] = max(in[i], 0)                   (f32)
    kSoftmax,    // out = exp(in - max) / sum                (f32)
};

inline constexpr std::uint16_t kNoTensor = 0xFFFFU;

struct OpDesc {
    OpKind kind;
    std::uint16_t input;
    std::uint16_t output;
    std::uint16_t weights;  // kDense only
    std::uint16_t bias;     // kDense only
    std::uint32_t in_features;
    std::uint32_t out_features;
    float scale;  // kNormalize only
    float shift;  // kNormalize only
};

// Validates tensor/op tables independent of the backend (shared by all implementations).
Result<void> validate_graph(Span<const TensorDesc> tensors, Span<const OpDesc> ops) noexcept;

class IComputeBackend {
public:
    virtual ~IComputeBackend() = default;

    virtual const char* name() const noexcept = 0;
    virtual Result<void> configure(Span<const TensorDesc> tensors, Span<const OpDesc> ops) noexcept = 0;
    virtual Result<void> upload(std::uint16_t tensor, ConstByteSpan bytes) noexcept = 0;
    virtual Result<void> seal() noexcept = 0;
    virtual Result<void> execute() noexcept = 0;
    virtual Result<void> download(std::uint16_t tensor, ByteSpan bytes) noexcept = 0;

    // Number of memory allocations performed so far (must not change after seal()).
    virtual std::uint32_t allocation_count() const noexcept = 0;

protected:
    IComputeBackend() = default;
    IComputeBackend(const IComputeBackend&) = default;
    IComputeBackend& operator=(const IComputeBackend&) = default;
};

}  // namespace ea::gpu
