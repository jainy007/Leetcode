// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   gpu_inference_stage (pattern: gpu_stage)
// Inputs:    sha256:fc3fc2669b7d7a8178527f959cb9d5c8e33a671595b25690ce99798b8499d060
// Regenerate: python3 -m tools.gen

#include "gpu_inference_stage/obstacle_score_stage.hpp"

#include <cmath>
#include <cstdio>

#include "ea/core/sha256.hpp"

namespace adas::obstacle_score {

namespace {

// Reads exactly dst.size() bytes from <dir>/<rel> and verifies the SHA-256 (GPU-006). INIT only.
ea::Result<void> load_verified(const char* dir, const char* rel, const char* sha256_hex, ea::ByteSpan dst) noexcept {
    char path[512];
    const int n = std::snprintf(path, sizeof(path), "%s/%s", dir, rel);
    if (n < 0 || static_cast<std::size_t>(n) >= sizeof(path)) {
        return ea::ErrorCode::kInvalidArgument;
    }
    std::FILE* f = std::fopen(path, "rb");
    if (f == nullptr) {
        return ea::ErrorCode::kIoError;
    }
    const std::size_t got = std::fread(dst.data(), 1U, dst.size(), f);
    const int extra = std::fgetc(f);
    (void)std::fclose(f);
    if (got != dst.size() || extra != EOF) {
        return ea::ErrorCode::kLengthMismatch;
    }
    ea::Sha256Digest expected{};
    if (!ea::parse_sha256_hex(sha256_hex, expected)) {
        return ea::ErrorCode::kInvalidArgument;
    }
    if (ea::Sha256::digest(ea::ConstByteSpan(dst)) != expected) {
        return ea::ErrorCode::kIntegrityError;
    }
    return ea::ok_result();
}

}  // namespace

const char* to_string(OutputStatus s) noexcept {
    switch (s) {
        case OutputStatus::kValid:
            return "VALID";
        case OutputStatus::kDeadlineMiss:
            return "DEADLINE_MISS";
        case OutputStatus::kImplausible:
            return "IMPLAUSIBLE";
        case OutputStatus::kBackendError:
            return "BACKEND_ERROR";
    }
    return "UNKNOWN";
}

const char* ObstacleScoreStage::weight_file(Weight w) noexcept {
    switch (w) {
        case Weight::kFc1W:
            return "weights/fc1_w.bin";
        case Weight::kFc1B:
            return "weights/fc1_b.bin";
    }
    return "";
}

const char* ObstacleScoreStage::weight_sha256(Weight w) noexcept {
    switch (w) {
        case Weight::kFc1W:
            return "2cf67aaf1a1f52ba8a92b5900b18efcb0ed176a4b93e2bf9bd1431dd5bf66d00";
        case Weight::kFc1B:
            return "a358de81303a615972df7e660e0649b000e20ae5d3f56b27550950a3f4b1eceb";
    }
    return "";
}

ea::Result<void> ObstacleScoreStage::init(const char* model_dir) noexcept {
    if (initialized_) {
        return ea::ErrorCode::kAlreadyInitialized;
    }
    if (model_dir == nullptr) {
        return ea::ErrorCode::kInvalidArgument;
    }
    // 1. Verify every artefact before touching the backend.
    EA_TRY(load_verified(model_dir, weight_file(Weight::kFc1W), weight_sha256(Weight::kFc1W),
                         ea::ByteSpan(fc1_w_.data(), fc1_w_.size())));
    EA_TRY(load_verified(model_dir, weight_file(Weight::kFc1B), weight_sha256(Weight::kFc1B),
                         ea::ByteSpan(fc1_b_.data(), fc1_b_.size())));
    // 2. Allocate all memory and load constants (INIT phase).
    EA_TRY(backend_.configure(ea::Span<const ea::gpu::TensorDesc>(kTensors), ea::Span<const ea::gpu::OpDesc>(kOps)));
    EA_TRY(backend_.upload(2U, ea::ConstByteSpan(fc1_w_.data(), fc1_w_.size())));
    EA_TRY(backend_.upload(3U, ea::ConstByteSpan(fc1_b_.data(), fc1_b_.size())));
    // 3. Freeze the graph; no allocation from here on.
    EA_TRY(backend_.seal());
    allocations_at_init_ = backend_.allocation_count();
    initialized_ = true;
    return ea::ok_result();
}

Output ObstacleScoreStage::run(const Input& input) noexcept {
    Output out{};
    sequence_ += 1U;
    out.sequence = sequence_;
    if (!initialized_) {
        out.timestamp_ns = clock_.now();
        out.status = OutputStatus::kBackendError;
        return out;
    }
    const ea::osal::Nanoseconds t0 = clock_.now();
    const bool ok =
        backend_.upload(kInputTensor, ea::ConstByteSpan(input.data(), input.size())).ok() && backend_.execute().ok() &&
        backend_.download(kOutputTensor, ea::ByteSpan(reinterpret_cast<std::uint8_t*>(out.scores.data()),  // NOLINT
                                                      sizeof(out.scores)))
            .ok();
    const ea::osal::Nanoseconds t1 = clock_.now();
    out.timestamp_ns = t1;
    if (!ok) {
        out.status = OutputStatus::kBackendError;
    } else if (t1 - t0 > kDeadline) {
        out.status = OutputStatus::kDeadlineMiss;  // GPU-009
    } else if (!plausible(out.scores)) {
        out.status = OutputStatus::kImplausible;  // GPU-008
    } else {
        out.status = OutputStatus::kValid;
    }
    return out;
}

bool ObstacleScoreStage::plausible(const Scores& s) noexcept {
    // finite  [REQ-OBS-005]
    for (const float v : s) {
        if (!std::isfinite(v)) {
            return false;
        }
    }
    // range [0.0, 1.0]  [REQ-OBS-005]
    for (const float v : s) {
        if (v < 0.0F || v > 1.0F) {
            return false;
        }
    }
    // sum == 1.0 +/- 0.001  [REQ-OBS-005]
    {
        float sum = 0.0F;
        for (const float v : s) {
            sum += v;
        }
        if (std::fabs(sum - 1.0F) > 0.001F) {
            return false;
        }
    }
    return true;
}

}  // namespace adas::obstacle_score
