// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   gpu_inference_stage (pattern: gpu_stage)
// Inputs:    sha256:7c6e31adbf2ccb7de4b9815cf4bbcfea3cc85b12d6095fab5d2d18ae8a06a647
// Regenerate: python3 -m tools.gen

#pragma once

#include "ea/osal/clock.hpp"
#include "gpu_inference_stage/obstacle_score_stage_types.hpp"

namespace adas::obstacle_score {

// ObstacleScoreMonitor (ASIL B): runs in its own process (MP-001) and sees the
// producer's outputs only through the shared-memory channel /ea_obstacle_score.
// It does not link the producer; the plausibility checks are generated a second time here.
class ObstacleScoreMonitor {
public:
    static constexpr ea::osal::Nanoseconds kTimeout = 66 * ea::osal::kNsPerMs;
    static constexpr const char* kChannelName = "/ea_obstacle_score";

    explicit ObstacleScoreMonitor(const ea::osal::IClock& clock) noexcept : clock_(clock) {}

    // `latest` is the newest output read from the channel, nullptr while nothing was published.
    MonitorVerdict check(const Output* latest) noexcept;

    static bool plausible(const Scores& s) noexcept;

private:
    const ea::osal::IClock& clock_;
};

}  // namespace adas::obstacle_score
