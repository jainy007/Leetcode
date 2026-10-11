// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   gpu_inference_stage (pattern: gpu_stage)
// Inputs:    sha256:4bb029985135b75b0028af58fbda92e707ba881e9dd8a033d0cc97df5435d2dc
// Regenerate: python3 -m tools.gen

#include "gpu_inference_stage/obstacle_score_monitor.hpp"

#include <cmath>

namespace adas::obstacle_score {

const char* to_string(MonitorVerdict v) noexcept {
    switch (v) {
        case MonitorVerdict::kOk:
            return "OK";
        case MonitorVerdict::kNoData:
            return "NO_DATA";
        case MonitorVerdict::kTimeout:
            return "TIMEOUT";
        case MonitorVerdict::kImplausible:
            return "IMPLAUSIBLE";
        case MonitorVerdict::kDeadlineMiss:
            return "DEADLINE_MISS";
    }
    return "UNKNOWN";
}

MonitorVerdict ObstacleScoreMonitor::check(const Output* latest) noexcept {
    if (latest == nullptr) {
        return MonitorVerdict::kNoData;
    }
    const ea::osal::Nanoseconds age = clock_.now() - latest->timestamp_ns;
    if (age < 0 || age > kTimeout) {
        return MonitorVerdict::kTimeout;
    }
    switch (latest->status) {
        case OutputStatus::kValid:
            break;
        case OutputStatus::kDeadlineMiss:
            return MonitorVerdict::kDeadlineMiss;
        case OutputStatus::kImplausible:
        case OutputStatus::kBackendError:
            return MonitorVerdict::kImplausible;
    }
    return plausible(latest->scores) ? MonitorVerdict::kOk : MonitorVerdict::kImplausible;
}

bool ObstacleScoreMonitor::plausible(const Scores& s) noexcept {
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
