// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   gpu_inference_stage (pattern: gpu_stage)
// Inputs:    sha256:2b2cc1de9869f153865de1c79359dc198287716630efcaa55192fce6f3ffc6ed
// Regenerate: python3 -m tools.gen

#pragma once

#include <array>
#include <cstdint>

#include "ea/core/result.hpp"
#include "ea/gpu/backend.hpp"
#include "ea/osal/clock.hpp"
#include "gpu_inference_stage/obstacle_score_stage_types.hpp"

namespace adas::obstacle_score {

// ObstacleScoreStage: fixed inference graph on an accelerator backend (cpu_ref, cuda, qnn).
//   init(): verify weight files (GPU-006), allocate everything (GPU-001), seal the graph (GPU-002)
//   run():  upload input, execute, download scores, check deadline (GPU-009) and plausibility (GPU-008)
class ObstacleScoreStage {
public:
    // Static memory needed by the cpu_ref backend (64-byte aligned tensor slots).
    static constexpr std::size_t kArenaBytes = 1600U;
    static constexpr ea::osal::Nanoseconds kDeadline = 33 * ea::osal::kNsPerMs;
    // Accelerator backends must match cpu_ref within this tolerance (GPU-007).
    static constexpr float kReferenceTolerance = 0.0001F;
    static constexpr std::uint16_t kInputTensor = 0U;
    static constexpr std::uint16_t kOutputTensor = 6U;

    static constexpr ea::gpu::TensorDesc kTensors[] = {
        {"image_patch", ea::gpu::DType::kU8, 64U, false},  // 0: input [1, 64]
        {"normalized", ea::gpu::DType::kF32, 64U, false},  // 1: intermediate [1, 64]
        {"fc1_w", ea::gpu::DType::kF32, 256U, true},  // 2: weight [64, 4]
        {"fc1_b", ea::gpu::DType::kF32, 4U, true},  // 3: weight [4]
        {"logits", ea::gpu::DType::kF32, 4U, false},  // 4: intermediate [1, 4]
        {"activated", ea::gpu::DType::kF32, 4U, false},  // 5: intermediate [1, 4]
        {"scores", ea::gpu::DType::kF32, 4U, false},  // 6: output [1, 4]
    };
    static constexpr ea::gpu::OpDesc kOps[] = {
        {ea::gpu::OpKind::kNormalize, 0U, 1U, ea::gpu::kNoTensor, ea::gpu::kNoTensor, 0U, 0U, 0.00392156862745098F, 0.0F},  // preprocess
        {ea::gpu::OpKind::kDense, 1U, 4U, 2U, 3U, 64U, 4U, 0.0F, 0.0F},  // fc1
        {ea::gpu::OpKind::kRelu, 4U, 5U, ea::gpu::kNoTensor, ea::gpu::kNoTensor, 0U, 0U, 0.0F, 0.0F},  // relu1
        {ea::gpu::OpKind::kSoftmax, 5U, 6U, ea::gpu::kNoTensor, ea::gpu::kNoTensor, 0U, 0U, 0.0F, 0.0F},  // head
    };

    // Weight file (relative to the model directory) and its SHA-256 from the model.
    static const char* weight_file(Weight w) noexcept;
    static const char* weight_sha256(Weight w) noexcept;

    ObstacleScoreStage(ea::gpu::IComputeBackend& backend, const ea::osal::IClock& clock) noexcept
        : backend_(backend), clock_(clock) {}

    ea::Result<void> init(const char* model_dir) noexcept;
    Output run(const Input& input) noexcept;

    bool initialized() const noexcept { return initialized_; }
    std::uint32_t allocations_at_init() const noexcept { return allocations_at_init_; }

    static bool plausible(const Scores& s) noexcept;

private:
    ea::gpu::IComputeBackend& backend_;
    const ea::osal::IClock& clock_;
    bool initialized_ = false;
    std::uint64_t sequence_ = 0U;
    std::uint32_t allocations_at_init_ = 0U;
    alignas(64) std::array<std::uint8_t, 1024U> fc1_w_{};  // staging, verified before upload
    alignas(64) std::array<std::uint8_t, 16U> fc1_b_{};  // staging, verified before upload
};

}  // namespace adas::obstacle_score
