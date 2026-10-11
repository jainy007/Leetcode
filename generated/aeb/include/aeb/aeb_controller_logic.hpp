// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb (pattern: can_function)
// Inputs:    sha256:7985079d35cf79be077da24af2c4f9d16c2c565e0ce9d01d499b7c22d7f7c8d2
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstdint>

#include "aeb/aeb_controller_types.hpp"

namespace adas::aeb {

// Feature 'aeb' logic: forward collision warning and two-stage emergency braking based on the
// time to collision (TTC = range / closing speed) of the relevant radar target.
class AebControllerLogic {
public:
    static constexpr double kMinSpeedKph = 10.0;
    static constexpr double kMaxSpeedKph = 150.0;
    static constexpr double kMaxTargetRangeM = 150.0;
    static constexpr double kLaneHalfWidthM = 1.8;
    static constexpr double kMinClosingSpeedMps = 0.5;
    static constexpr double kFcwTtcS = 2.8;
    static constexpr double kWarningReleaseTtcS = 3.3;
    static constexpr double kPartialBrakeTtcS = 1.8;
    static constexpr double kFullBrakeTtcS = 1.2;
    static constexpr std::uint32_t kMinWarningCycles = 40U;
    static constexpr double kPartialDecelMps2 = 4.0;
    static constexpr double kFullDecelMps2 = 8.0;
    static constexpr double kKickdownPedalPct = 80.0;
    static constexpr double kStandstillSpeedKph = 1.0;

    void step(const Inputs& in, Outputs& out) noexcept;
    State state() const noexcept { return state_; }

private:
    State state_ = State::kStandby;
    std::uint32_t warning_cycles_ = 0U;
};

}  // namespace adas::aeb
