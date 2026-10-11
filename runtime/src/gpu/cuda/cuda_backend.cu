// CUDA backend implementation. See ea/gpu/cuda_backend.hpp for the lifecycle contract.
// Kernels mirror cpu_ref operation order; multiplications/additions use __fmul_rn/__fadd_rn so the
// compiler cannot contract them into FMAs, keeping results within the model's tolerance (GPU-007).
#include <cuda_runtime.h>
#include <time.h>

#include <cstring>

#include "ea/gpu/cuda_backend.hpp"

namespace ea::gpu {

namespace {

ErrorCode map_error(cudaError_t e) noexcept {
    return (e == cudaErrorMemoryAllocation) ? ErrorCode::kResourceExhausted : ErrorCode::kOsError;
}

#define EA_CUDA_TRY(expr)                   \
    do {                                    \
        const cudaError_t ea_cuda_e_ = (expr); \
        if (ea_cuda_e_ != cudaSuccess) {    \
            return map_error(ea_cuda_e_);   \
        }                                   \
    } while (false)

std::int64_t now_ns() noexcept {
    timespec ts{};
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<std::int64_t>(ts.tv_sec) * 1000000000LL + ts.tv_nsec;
}

constexpr unsigned kBlock = 128U;

unsigned blocks_for(std::uint32_t n) noexcept {
    return (n + kBlock - 1U) / kBlock;
}

__global__ void normalize_kernel(const std::uint8_t* in, float* out, std::uint32_t n, float scale, float shift) {
    const std::uint32_t i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        out[i] = __fadd_rn(__fmul_rn(static_cast<float>(in[i]), scale), shift);
    }
}

__global__ void dense_kernel(const float* in, const float* w, const float* b, float* out, std::uint32_t in_f,
                             std::uint32_t out_f) {
    const std::uint32_t j = blockIdx.x * blockDim.x + threadIdx.x;
    if (j < out_f) {
        float acc = b[j];
        for (std::uint32_t i = 0U; i < in_f; ++i) {  // same order as cpu_ref
            acc = __fadd_rn(acc, __fmul_rn(in[i], w[i * out_f + j]));
        }
        out[j] = acc;
    }
}

__global__ void relu_kernel(const float* in, float* out, std::uint32_t n) {
    const std::uint32_t i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        out[i] = (in[i] > 0.0F) ? in[i] : 0.0F;
    }
}

// Classification heads are small; one thread keeps the reduction order identical to cpu_ref.
__global__ void softmax_kernel(const float* in, float* out, std::uint32_t n) {
    if (blockIdx.x == 0U && threadIdx.x == 0U) {
        float max = in[0];
        for (std::uint32_t i = 1U; i < n; ++i) {
            max = (in[i] > max) ? in[i] : max;
        }
        float sum = 0.0F;
        for (std::uint32_t i = 0U; i < n; ++i) {
            out[i] = expf(in[i] - max);
            sum = __fadd_rn(sum, out[i]);
        }
        for (std::uint32_t i = 0U; i < n; ++i) {
            out[i] = __fdiv_rn(out[i], sum);
        }
    }
}

}  // namespace

std::size_t CudaBackend::bytes(std::uint16_t t) const noexcept {
    return static_cast<std::size_t>(tensors_[t].elements) * dtype_size(tensors_[t].dtype);
}

CudaBackend::~CudaBackend() {
    release();
}

void CudaBackend::release() noexcept {
    if (exec_ != nullptr) {
        (void)cudaGraphExecDestroy(static_cast<cudaGraphExec_t>(exec_));
    }
    if (graph_ != nullptr) {
        (void)cudaGraphDestroy(static_cast<cudaGraph_t>(graph_));
    }
    for (std::size_t i = 0U; i < kMaxTensors; ++i) {
        if (device_[i] != nullptr) {
            (void)cudaFree(device_[i]);
        }
        if (staging_[i] != nullptr) {
            (void)cudaFreeHost(staging_[i]);
        }
        device_[i] = nullptr;
        staging_[i] = nullptr;
    }
    if (done_ != nullptr) {
        (void)cudaEventDestroy(static_cast<cudaEvent_t>(done_));
    }
    if (stream_ != nullptr) {
        (void)cudaStreamDestroy(static_cast<cudaStream_t>(stream_));
    }
    exec_ = graph_ = done_ = stream_ = nullptr;
}

Result<void> CudaBackend::configure(Span<const TensorDesc> tensors, Span<const OpDesc> ops) noexcept {
    if (configured_ || sealed_) {
        return ErrorCode::kAlreadyInitialized;
    }
    if (tensors.size() > kMaxTensors) {
        return ErrorCode::kResourceExhausted;
    }
    EA_TRY(validate_graph(tensors, ops));
    for (const OpDesc& op : ops) {
        if (op.kind == OpKind::kSoftmax && tensors[op.output].elements > 1024U) {
            return ErrorCode::kNotSupported;  // single-thread head kernel
        }
    }
    tensors_ = tensors;
    ops_ = ops;
    std::array<bool, kMaxTensors> written{};
    std::array<bool, kMaxTensors> read{};
    for (const OpDesc& op : ops) {
        written[op.output] = true;
        read[op.input] = true;
    }
    EA_CUDA_TRY(cudaSetDevice(cfg_.device));
    cudaStream_t stream = nullptr;
    EA_CUDA_TRY(cudaStreamCreateWithPriority(&stream, cudaStreamNonBlocking, cfg_.stream_priority));
    stream_ = stream;
    cudaEvent_t done = nullptr;
    EA_CUDA_TRY(cudaEventCreateWithFlags(&done, cudaEventDisableTiming));
    done_ = done;
    for (std::uint16_t i = 0U; i < tensors.size(); ++i) {
        EA_CUDA_TRY(cudaMalloc(&device_[i], bytes(i)));
        ++allocations_;
        is_input_[i] = !tensors[i].is_const && !written[i];
        is_output_[i] = written[i] && !read[i];
        if (is_input_[i] || is_output_[i]) {
            EA_CUDA_TRY(cudaHostAlloc(&staging_[i], bytes(i), cudaHostAllocDefault));
            ++allocations_;
        }
    }
    configured_ = true;
    return ok_result();
}

Result<void> CudaBackend::upload(std::uint16_t tensor, ConstByteSpan data) noexcept {
    if (!configured_) {
        return ErrorCode::kNotInitialized;
    }
    if (tensor >= tensors_.size() || data.size() != bytes(tensor)) {
        return ErrorCode::kInvalidArgument;
    }
    if (tensors_[tensor].is_const) {
        if (sealed_) {
            return ErrorCode::kAlreadyInitialized;  // GPU-005
        }
        EA_CUDA_TRY(cudaMemcpy(device_[tensor], data.data(), data.size(), cudaMemcpyHostToDevice));
        uploaded_[tensor] = true;
        return ok_result();
    }
    if (!sealed_) {
        return ErrorCode::kNotInitialized;
    }
    if (!is_input_[tensor]) {
        return ErrorCode::kNotSupported;
    }
    std::memcpy(staging_[tensor], data.data(), data.size());  // the graph copies staging -> device
    return ok_result();
}

Result<void> CudaBackend::seal() noexcept {
    if (!configured_) {
        return ErrorCode::kNotInitialized;
    }
    if (sealed_) {
        return ErrorCode::kAlreadyInitialized;
    }
    for (std::uint16_t i = 0U; i < tensors_.size(); ++i) {
        if (tensors_[i].is_const && !uploaded_[i]) {
            return ErrorCode::kNotInitialized;
        }
    }
    cudaStream_t s = static_cast<cudaStream_t>(stream_);
    EA_CUDA_TRY(cudaStreamBeginCapture(s, cudaStreamCaptureModeThreadLocal));
    for (std::uint16_t i = 0U; i < tensors_.size(); ++i) {
        if (is_input_[i]) {
            EA_CUDA_TRY(cudaMemcpyAsync(device_[i], staging_[i], bytes(i), cudaMemcpyHostToDevice, s));
        }
    }
    for (const OpDesc& op : ops_) {
        const std::uint32_t n = tensors_[op.output].elements;
        float* out = static_cast<float*>(device_[op.output]);
        switch (op.kind) {
            case OpKind::kNormalize:
                normalize_kernel<<<blocks_for(n), kBlock, 0, s>>>(static_cast<const std::uint8_t*>(device_[op.input]),
                                                                  out, n, op.scale, op.shift);
                break;
            case OpKind::kDense:
                dense_kernel<<<blocks_for(op.out_features), kBlock, 0, s>>>(
                    static_cast<const float*>(device_[op.input]), static_cast<const float*>(device_[op.weights]),
                    static_cast<const float*>(device_[op.bias]), out, op.in_features, op.out_features);
                break;
            case OpKind::kRelu:
                relu_kernel<<<blocks_for(n), kBlock, 0, s>>>(static_cast<const float*>(device_[op.input]), out, n);
                break;
            case OpKind::kSoftmax:
                softmax_kernel<<<1, 1, 0, s>>>(static_cast<const float*>(device_[op.input]), out, n);
                break;
        }
        EA_CUDA_TRY(cudaGetLastError());
    }
    for (std::uint16_t i = 0U; i < tensors_.size(); ++i) {
        if (is_output_[i]) {
            EA_CUDA_TRY(cudaMemcpyAsync(staging_[i], device_[i], bytes(i), cudaMemcpyDeviceToHost, s));
        }
    }
    cudaGraph_t graph = nullptr;
    EA_CUDA_TRY(cudaStreamEndCapture(s, &graph));
    graph_ = graph;
    cudaGraphExec_t exec = nullptr;
    EA_CUDA_TRY(cudaGraphInstantiateWithFlags(&exec, graph, 0ULL));
    exec_ = exec;
    sealed_ = true;
    return ok_result();
}

Result<void> CudaBackend::execute() noexcept {
    if (!sealed_) {
        return ErrorCode::kNotInitialized;
    }
    cudaStream_t s = static_cast<cudaStream_t>(stream_);
    cudaEvent_t done = static_cast<cudaEvent_t>(done_);
    EA_CUDA_TRY(cudaGraphLaunch(static_cast<cudaGraphExec_t>(exec_), s));
    EA_CUDA_TRY(cudaEventRecord(done, s));
    const std::int64_t deadline = now_ns() + cfg_.sync_timeout_ns;
    for (;;) {  // GPU-003: bounded wait, never an unbounded synchronize
        const cudaError_t q = cudaEventQuery(done);
        if (q == cudaSuccess) {
            return ok_result();
        }
        if (q != cudaErrorNotReady) {
            return map_error(q);
        }
        if (now_ns() > deadline) {
            return ErrorCode::kTimeout;
        }
    }
}

Result<void> CudaBackend::download(std::uint16_t tensor, ByteSpan data) noexcept {
    if (!sealed_) {
        return ErrorCode::kNotInitialized;
    }
    if (tensor >= tensors_.size() || data.size() != bytes(tensor)) {
        return ErrorCode::kInvalidArgument;
    }
    if (!is_output_[tensor]) {
        return ErrorCode::kNotSupported;  // only graph outputs are copied back
    }
    std::memcpy(data.data(), staging_[tensor], data.size());
    return ok_result();
}

}  // namespace ea::gpu
