// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb_monitor (pattern: can_function)
// Inputs:    sha256:230536ebb5fd8cf2e5c6eff700cb3596edf001524e2100e20ccfddcdcbbad55e
// Regenerate: python3 -m tools.gen

#include "aeb_monitor/aeb_monitor_logic.hpp"

#include <cmath>

namespace adas::aeb_monitor {

void AebMonitorLogic::violation(Reason r) noexcept {
    state_ = State::kSafeState;  // latched until the next power cycle
    reason_ = r;
}

void AebMonitorLogic::check_command(const Inputs& in) noexcept {
    const bool partial = std::fabs(in.cmd_status - kPartialStatusCode) < 0.5;
    const bool full = std::fabs(in.cmd_status - kFullStatusCode) < 0.5;
    const double closing = -in.target_range_rate;
    const bool in_lane = in.target_valid_ok && in.target_valid > 0.5 && in.target_range <= kMaxTargetRangeM &&
                         std::fabs(in.target_lateral) <= kLaneHalfWidthM + kLaneMarginM;
    const bool approaching = in_lane && closing > 0.0;
    const bool full_request = in.cmd_brake > kPartialDecelMps2 + kRequestToleranceMps2;
    if (in.cmd_brake > kFullDecelMps2 + kRequestToleranceMps2 || (partial && full_request)) {
        violation(Reason::kRequestLimit);
    } else if (in.cmd_brake > 0.0 && !partial && !full) {
        violation(Reason::kRequestOutsideBraking);
    } else if (in.cmd_brake <= 0.0) {
        braking_justified_ = false;
        full_braking_justified_ = false;
    } else if (!in_lane) {
        violation(Reason::kUnjustifiedBraking);  // braking continued without an in-lane target
    } else {
        // Onset: TTC within the threshold plus margin; afterwards the braking stays justified
        // while the target stays valid and in lane (TTC grows as braking succeeds).
        if (!braking_justified_) {
            braking_justified_ = approaching && in.target_range <= (kPartialBrakeTtcS + kTtcMarginS) * closing;
        }
        if (full_request && !full_braking_justified_) {
            full_braking_justified_ = approaching && in.target_range <= (kFullBrakeTtcS + kTtcMarginS) * closing;
        }
        if (!braking_justified_) {
            violation(Reason::kUnjustifiedBraking);
        } else if (full_request && !full_braking_justified_) {
            violation(Reason::kUnjustifiedFullBraking);
        } else {
            // justified
        }
    }
    if (state_ == State::kMonitoring) {
        last_counter_ = in.cmd_counter;
    }
}

void AebMonitorLogic::step(const Inputs& in, Outputs& out) noexcept {
    const bool command_new = in.cmd_status_ok && in.cmd_counter != last_counter_;
    if (state_ == State::kInit && command_new) {
        state_ = State::kMonitoring;  // the first command is checked like every other
    }
    if (state_ == State::kMonitoring) {
        if (!in.cmd_status_ok) {
            violation(Reason::kCommandTimeout);
        } else if (command_new) {
            check_command(in);
        } else {
            // a lost frame within the timeout: nothing new to approve
        }
    }
    const bool monitoring = state_ == State::kMonitoring;
    out.status = static_cast<double>(static_cast<std::uint8_t>(state_));
    out.permit = monitoring ? 1.0 : 0.0;
    // MONITORING is entered only through a checked command, so last_counter_ is valid here.
    out.approved_counter = monitoring ? last_counter_ : 0.0;
    out.fault_reason = static_cast<double>(static_cast<std::uint8_t>(reason_));
}

}  // namespace adas::aeb_monitor
