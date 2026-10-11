// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   gpu_inference_stage (pattern: gpu_stage)
// Inputs:    sha256:4bb029985135b75b0028af58fbda92e707ba881e9dd8a033d0cc97df5435d2dc
// Regenerate: python3 -m tools.gen

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

// Types shared by ObstacleScoreStage (producer, ASIL QM) and
// ObstacleScoreMonitor (monitor, ASIL B). Everything here is trivially copyable and
// address-free so it can cross the shared-memory channel (MP-005).

namespace adas::obstacle_score {

inline constexpr std::size_t kInputElements = 64U;
inline constexpr std::size_t kNumClasses = 4U;
inline constexpr std::size_t kClassCar = 0U;  // car
inline constexpr std::size_t kClassPedestrian = 1U;  // pedestrian
inline constexpr std::size_t kClassCyclist = 2U;  // cyclist
inline constexpr std::size_t kClassBackground = 3U;  // background

using Input = std::array<std::uint8_t, kInputElements>;
using Scores = std::array<float, kNumClasses>;

enum class OutputStatus : std::uint8_t {
    kValid,
    kDeadlineMiss,
    kImplausible,
    kBackendError,
};
const char* to_string(OutputStatus s) noexcept;

struct Output {
    std::uint64_t sequence;
    std::int64_t timestamp_ns;  // monotonic clock at the end of processing
    Scores scores;
    OutputStatus status;
};

enum class MonitorVerdict : std::uint8_t {
    kOk,
    kNoData,
    kTimeout,
    kImplausible,
    kDeadlineMiss,
};
const char* to_string(MonitorVerdict v) noexcept;

enum class Weight : std::uint8_t {
    kFc1W,
    kFc1B,
};

}  // namespace adas::obstacle_score
