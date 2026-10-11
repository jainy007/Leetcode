// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb (pattern: can_function)
// Inputs:    sha256:e2c276a23aa546cd2244f6d98d18ebe94c32ea6672f5f82001d56e55370b3be5
// Regenerate: python3 -m tools.gen

#include "aeb/aeb_controller_logic.hpp"

#include <cmath>

namespace adas::aeb {

void AebControllerLogic::step(const Inputs& in, Outputs& out) noexcept {
    if (!in.all_ok) {
        state_ = State::kFault;  // until every input is valid again
        warning_cycles_ = 0U;
    } else {
        if (state_ == State::kFault) {
            state_ = State::kStandby;
        }
        const double closing = -in.target_range_rate;  // > 0: approaching
        const bool relevant = in.target_valid > 0.5 && in.target_range <= kMaxTargetRangeM &&
                              std::fabs(in.target_lateral) <= kLaneHalfWidthM && closing > kMinClosingSpeedMps;
        const double ttc = relevant ? in.target_range / closing : 1.0e9;
        const bool speed_ok = in.ego_speed >= kMinSpeedKph && in.ego_speed <= kMaxSpeedKph;
        const bool kickdown = in.accel_pedal >= kKickdownPedalPct;
        const bool stopped = in.ego_speed <= kStandstillSpeedKph;
        switch (state_) {
            case State::kStandby:
                if (relevant && speed_ok && !kickdown && ttc <= kFcwTtcS) {
                    state_ = State::kWarning;
                    warning_cycles_ = 0U;
                }
                break;
            case State::kWarning:
                warning_cycles_ = (warning_cycles_ < kMinWarningCycles) ? warning_cycles_ + 1U : warning_cycles_;
                if (kickdown || !relevant || ttc > kWarningReleaseTtcS) {
                    state_ = State::kStandby;
                } else if (ttc <= kFullBrakeTtcS) {
                    state_ = State::kFullBrake;  // imminent: no minimum warning time
                } else if (ttc <= kPartialBrakeTtcS && warning_cycles_ >= kMinWarningCycles) {
                    state_ = State::kPartialBrake;
                } else {
                    // keep warning
                }
                break;
            case State::kPartialBrake:
                if (kickdown || !relevant || stopped) {
                    state_ = State::kStandby;
                } else if (ttc <= kFullBrakeTtcS) {
                    state_ = State::kFullBrake;
                } else {
                    // keep braking (the driver's brake pedal does not cancel)
                }
                break;
            case State::kFullBrake:
                if (kickdown || !relevant || stopped) {
                    state_ = State::kStandby;
                }
                break;
            case State::kFault:
                break;
        }
    }
    out.status = static_cast<double>(static_cast<std::uint8_t>(state_));
    out.fcw_warning = (state_ == State::kWarning || state_ == State::kPartialBrake || state_ == State::kFullBrake) ? 1.0 : 0.0;
    out.brake_request = (state_ == State::kPartialBrake) ? kPartialDecelMps2
                                                         : ((state_ == State::kFullBrake) ? kFullDecelMps2 : 0.0);
}

}  // namespace adas::aeb
