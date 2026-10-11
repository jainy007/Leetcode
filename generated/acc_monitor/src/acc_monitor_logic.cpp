// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc_monitor (pattern: can_function)
// Inputs:    sha256:7aee4feecefbc18e2ad7476e4d93efffab091cab221dd8b1b9699ac5d31b6524
// Regenerate: python3 -m tools.gen

#include "acc_monitor/acc_monitor_logic.hpp"

#include <cmath>

#include "ea/core/saturating.hpp"

namespace adas::acc_monitor {

namespace {
constexpr double kEps = 1.0e-9;
}  // namespace

void AccMonitorLogic::violation(Reason r) noexcept {
    state_ = State::kSafeState;  // latched until the next power cycle
    reason_ = r;
}

void AccMonitorLogic::check_command(const Inputs& in) noexcept {
    const bool active = std::fabs(in.cmd_status - kActiveStatusCode) < 0.5;
    const double accel_step = in.cmd_accel - prev_accel_;
    const double brake_step = in.cmd_brake - prev_brake_;
    const double cycles = static_cast<double>(cycles_since_command_);
    if (in.cmd_accel > kAccelMaxMps2 + kRequestToleranceMps2 || in.cmd_brake > kDecelMaxMps2 + kRequestToleranceMps2) {
        violation(Reason::kRequestLimit);
    } else if (in.cmd_accel > 0.0 && in.cmd_brake > 0.0) {
        violation(Reason::kAccelAndBrake);
    } else if (!active && (in.cmd_accel > 0.0 || in.cmd_brake > 0.0)) {
        violation(Reason::kRequestOutsideActive);
    } else if (accel_step > kAccelRatePerCycle * cycles + kEps || brake_step > kBrakeRatePerCycle * cycles + kEps) {
        violation(Reason::kRequestRate);
    } else if (in.cmd_accel > 0.0 && endangered(in) && accel_step > -kAccelDecayPerCycle * cycles + kEps) {
        violation(Reason::kUnintendedAccel);  // acceleration toward a close target must decay at once
    } else {
        last_counter_ = in.cmd_counter;
    }
    prev_accel_ = in.cmd_accel;
    prev_brake_ = in.cmd_brake;
    cycles_since_command_ = 0U;
}

bool AccMonitorLogic::endangered(const Inputs& in) const noexcept {
    // An in-lane target closer than kMinTtcForAccelS, or a radar that cannot show a free path.
    const double closing = -in.target_range_rate;
    const bool in_path = in.target_valid > 0.5 && in.target_range <= kMaxTargetRangeM &&
                         std::fabs(in.target_lateral) <= kLaneHalfWidthM;
    return !in.target_valid_ok || (in_path && closing > 0.0 && in.target_range < kMinTtcForAccelS * closing);
}

void AccMonitorLogic::step(const Inputs& in, Outputs& out) noexcept {
    const bool command_new = in.cmd_status_ok && in.cmd_counter != last_counter_;
    if (state_ == State::kInit && command_new) {
        state_ = State::kMonitoring;  // the first command is checked like every other
    }
    if (state_ == State::kMonitoring) {
        cycles_since_command_ = ea::sat_inc(cycles_since_command_);  // CPU-010
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

}  // namespace adas::acc_monitor
