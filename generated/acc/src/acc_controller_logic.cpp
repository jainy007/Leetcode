// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc (pattern: can_function)
// Inputs:    sha256:e3f0b69ab4e6da8dcf4e68125db331147578831a646589377e6c96cffacf9544
// Regenerate: python3 -m tools.gen

#include "acc/acc_controller_logic.hpp"

#include <cmath>

#include "ea/core/rounding.hpp"

namespace adas::acc {

namespace {

double clamp(double v, double lo, double hi) noexcept {
    return (v < lo) ? lo : ((v > hi) ? hi : v);
}

bool on(double signal) noexcept {
    return signal > 0.5;
}

}  // namespace

void AccControllerLogic::update_mode(const Inputs& in) noexcept {
    if (!in.main_switch_ok) {
        if (state_ != State::kOff) {
            state_ = State::kFault;  // no valid driver input: FAULT (latched)
        }
        return;
    }
    // Rising edges of the buttons (newest valid driver input).
    const bool set_now = on(in.set_button);
    const bool resume_now = on(in.resume_button);
    const bool cancel_now = on(in.cancel_button);
    const bool up_now = on(in.speed_up_button);
    const bool down_now = on(in.speed_down_button);
    const bool set_edge = set_now && !prev_set_;
    const bool resume_edge = resume_now && !prev_resume_;
    const bool cancel_edge = cancel_now && !prev_cancel_;
    const bool up_edge = up_now && !prev_up_;
    const bool down_edge = down_now && !prev_down_;
    prev_set_ = set_now;
    prev_resume_ = resume_now;
    prev_cancel_ = cancel_now;
    prev_up_ = up_now;
    prev_down_ = down_now;
    gap_setting_ = static_cast<std::uint32_t>(ea::round_half_away(in.gap_setting));  // [1, 3] by decode

    if (!on(in.main_switch)) {
        state_ = State::kOff;
        set_speed_kph_ = 0.0;
        return;
    }
    if (state_ == State::kOff) {
        state_ = State::kStandby;
        return;
    }
    if (state_ == State::kFault) {
        return;  // latched until the main switch is turned off
    }
    if (!in.all_ok) {
        state_ = State::kFault;
        return;
    }
    const double v_kph = in.ego_speed;
    const bool brake = on(in.brake_pedal);
    const bool can_engage = !brake && v_kph >= kMinActivationSpeedKph;
    switch (state_) {
        case State::kStandby:
            if (set_edge && can_engage) {
                set_speed_kph_ = clamp(static_cast<double>(ea::round_half_away(v_kph)), kMinActivationSpeedKph, kMaxSetSpeedKph);
                state_ = State::kActive;
            } else if (resume_edge && can_engage && set_speed_kph_ > 0.0) {
                state_ = State::kActive;
            } else {
                // stay
            }
            break;
        case State::kActive:
        case State::kOverride:
            if (brake || cancel_edge) {
                state_ = State::kStandby;  // set speed stays stored
                break;
            }
            if (state_ == State::kActive && v_kph < kDisengageSpeedKph) {
                state_ = State::kStandby;
                break;
            }
            if (up_edge) {
                set_speed_kph_ = clamp(set_speed_kph_ + kSetSpeedStepKph, kMinActivationSpeedKph, kMaxSetSpeedKph);
            }
            if (down_edge) {
                set_speed_kph_ = clamp(set_speed_kph_ - kSetSpeedStepKph, kMinActivationSpeedKph, kMaxSetSpeedKph);
            }
            state_ = (in.accel_pedal > kOverridePedalPct) ? State::kOverride : State::kActive;
            break;
        case State::kOff:
        case State::kFault:
            break;
    }
}

void AccControllerLogic::control(const Inputs& in) noexcept {
    if (state_ != State::kActive) {
        accel_raw_ = 0;  // no request outside ACTIVE; jerk limiting restarts from 0
        return;
    }
    const double v = in.ego_speed / 3.6;
    double a = kSpeedGainPerS * (set_speed_kph_ / 3.6 - v);
    const bool in_path = on(in.target_valid) && in.target_range <= kMaxTargetRangeM &&
                         std::fabs(in.target_lateral) <= kLaneHalfWidthM;
    if (in_path) {
        const double desired = kStandstillDistanceM + kTimeGapsS[gap_setting_ - 1U] * v;
        const double a_gap = kGapGainPerS2 * (in.target_range - desired) + kRangeRateGainPerS * in.target_range_rate;
        a = (a_gap < a) ? a_gap : a;
    }
    a = clamp(a, -kDecelMaxMps2, kAccelMaxMps2);
    const std::int32_t target = static_cast<std::int32_t>(ea::round_half_away(a / kAccelResolution));
    const std::int32_t delta = target - accel_raw_;
    accel_raw_ += (delta > kJerkUpStep) ? kJerkUpStep : ((delta < -kJerkDownStep) ? -kJerkDownStep : delta);
}

void AccControllerLogic::step(const Inputs& in, Outputs& out) noexcept {
    update_mode(in);
    control(in);
    out.status = static_cast<double>(static_cast<std::uint8_t>(state_));
    out.accel_request = (accel_raw_ > 0) ? static_cast<double>(accel_raw_) * kAccelResolution : 0.0;
    out.brake_request = (accel_raw_ < 0) ? static_cast<double>(-accel_raw_) * kAccelResolution : 0.0;
    out.steering_request = 0.0;  // longitudinal function only
    out.set_speed = set_speed_kph_;
    out.time_gap_setting = static_cast<double>(gap_setting_);
}

}  // namespace adas::acc
