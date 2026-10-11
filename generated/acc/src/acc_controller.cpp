// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc (pattern: acc_controller)
// Inputs:    sha256:cce2b25a21d5b0053b9588be55a9c9be1a00be09e870c4dedb9dbbf5638be09e
// Regenerate: python3 -m tools.gen

#include "acc/acc_controller.hpp"

#include <cmath>
#include <cstring>

#include "ea/core/assert.hpp"
#include "ea/core/rounding.hpp"
#include "ea/core/saturating.hpp"

namespace adas::acc {

namespace {
constexpr ea::e2e::P11Config kRadarObjectE2e{0x0200U, 0U, 8U, 8U, 3U};
constexpr std::uint16_t kRadarObjectTimeoutCycles = 6U;
constexpr ea::e2e::P11Config kEgoStateE2e{0x0210U, 0U, 8U, 8U, 3U};
constexpr std::uint16_t kEgoStateTimeoutCycles = 3U;
constexpr ea::e2e::P11Config kDriverInputE2e{0x0220U, 0U, 8U, 8U, 3U};
constexpr std::uint16_t kDriverInputTimeoutCycles = 3U;
constexpr ea::e2e::P11Config kAccCommandE2e{0x0300U, 0U, 8U, 8U, 2U};

double clamp(double v, double lo, double hi) noexcept {
    return (v < lo) ? lo : ((v > hi) ? hi : v);
}

bool on(double signal) noexcept {
    return signal > 0.5;
}

}  // namespace

const char* to_string(Mode m) noexcept {
    switch (m) {
        case Mode::kOff:
            return "OFF";
        case Mode::kStandby:
            return "STANDBY";
        case Mode::kActive:
            return "ACTIVE";
        case Mode::kOverride:
            return "OVERRIDE";
        case Mode::kFault:
            return "FAULT";
    }
    return "UNKNOWN";
}

AccController::AccController() noexcept
    : e2e_radar_object_(kRadarObjectE2e), age_radar_object_(kRadarObjectTimeoutCycles), e2e_ego_state_(kEgoStateE2e), age_ego_state_(kEgoStateTimeoutCycles), e2e_driver_input_(kDriverInputE2e), age_driver_input_(kDriverInputTimeoutCycles), e2e_tx_(kAccCommandE2e) {
    send();  // a valid OFF frame is available before the first cycle
}

void AccController::on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept {
    Mailbox* mb = nullptr;
    switch (can_id) {
        case kRadarObjectCanId:
            mb = &mb_radar_object_;
            break;
        case kEgoStateCanId:
            mb = &mb_ego_state_;
            break;
        case kDriverInputCanId:
            mb = &mb_driver_input_;
            break;
        default:
            return;
    }
    if (payload.size() > mb->data.size()) {
        return;
    }
    std::memcpy(mb->data.data(), payload.data(), payload.size());
    mb->length = payload.size();
    mb->fresh = true;
}

void AccController::receive() noexcept {
    {
        // RadarObject: frames failing E2E or decoding count as not received (REQ: timeout supervision).
        bool received = false;
        if (mb_radar_object_.fresh) {
            mb_radar_object_.fresh = false;
            const ea::ConstByteSpan frame(mb_radar_object_.data.data(), mb_radar_object_.length);
            if (ea::e2e::is_usable(e2e_radar_object_.check(frame))) {
                const ea::Result<RadarObject> d = decode_radar_object(frame);
                if (d.ok()) {
                    rx_radar_object_ = *d;
                    received = true;
                }
            }
        }
        age_radar_object_ = received ? static_cast<std::uint16_t>(0U) : ea::sat_inc(age_radar_object_);
    }
    {
        // EgoState: frames failing E2E or decoding count as not received (REQ: timeout supervision).
        bool received = false;
        if (mb_ego_state_.fresh) {
            mb_ego_state_.fresh = false;
            const ea::ConstByteSpan frame(mb_ego_state_.data.data(), mb_ego_state_.length);
            if (ea::e2e::is_usable(e2e_ego_state_.check(frame))) {
                const ea::Result<EgoState> d = decode_ego_state(frame);
                if (d.ok()) {
                    rx_ego_state_ = *d;
                    received = true;
                }
            }
        }
        age_ego_state_ = received ? static_cast<std::uint16_t>(0U) : ea::sat_inc(age_ego_state_);
    }
    {
        // DriverInput: frames failing E2E or decoding count as not received (REQ: timeout supervision).
        bool received = false;
        if (mb_driver_input_.fresh) {
            mb_driver_input_.fresh = false;
            const ea::ConstByteSpan frame(mb_driver_input_.data.data(), mb_driver_input_.length);
            if (ea::e2e::is_usable(e2e_driver_input_.check(frame))) {
                const ea::Result<DriverInput> d = decode_driver_input(frame);
                if (d.ok()) {
                    rx_driver_input_ = *d;
                    received = true;
                }
            }
        }
        age_driver_input_ = received ? static_cast<std::uint16_t>(0U) : ea::sat_inc(age_driver_input_);
    }
}

bool AccController::inputs_healthy() const noexcept {
    return (age_radar_object_ < kRadarObjectTimeoutCycles) && (age_ego_state_ < kEgoStateTimeoutCycles) && (age_driver_input_ < kDriverInputTimeoutCycles);
}

void AccController::update_mode() noexcept {
    if (age_driver_input_ >= kDriverInputTimeoutCycles) {
        if (mode_ != Mode::kOff) {
            mode_ = Mode::kFault;  // no valid driver input: FAULT (latched)
        }
        return;
    }
    // Driver inputs (newest valid values) and rising edges of the buttons.
    const bool set_now = on(rx_driver_input_.set_button);
    const bool resume_now = on(rx_driver_input_.resume_button);
    const bool cancel_now = on(rx_driver_input_.cancel_button);
    const bool up_now = on(rx_driver_input_.speed_up_button);
    const bool down_now = on(rx_driver_input_.speed_down_button);
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
    gap_setting_ = static_cast<std::uint32_t>(ea::round_half_away(rx_driver_input_.gap_setting));  // [1, 3] by decode

    if (!on(rx_driver_input_.main_switch)) {
        mode_ = Mode::kOff;
        set_speed_kph_ = 0.0;
        return;
    }
    if (mode_ == Mode::kOff) {
        mode_ = Mode::kStandby;
        return;
    }
    if (mode_ == Mode::kFault) {
        return;  // latched until the main switch is turned off
    }
    if (!inputs_healthy()) {
        mode_ = Mode::kFault;
        return;
    }
    const double v_kph = rx_ego_state_.vehicle_speed;
    const bool brake = on(rx_driver_input_.brake_pedal);
    const bool can_engage = !brake && v_kph >= kMinActivationSpeedKph;
    switch (mode_) {
        case Mode::kStandby:
            if (set_edge && can_engage) {
                set_speed_kph_ = clamp(static_cast<double>(ea::round_half_away(v_kph)), kMinActivationSpeedKph, kMaxSetSpeedKph);
                mode_ = Mode::kActive;
            } else if (resume_edge && can_engage && set_speed_kph_ > 0.0) {
                mode_ = Mode::kActive;
            } else {
                // stay
            }
            break;
        case Mode::kActive:
        case Mode::kOverride:
            if (brake || cancel_edge) {
                mode_ = Mode::kStandby;  // set speed stays stored
                break;
            }
            if (mode_ == Mode::kActive && v_kph < kDisengageSpeedKph) {
                mode_ = Mode::kStandby;
                break;
            }
            if (up_edge) {
                set_speed_kph_ = clamp(set_speed_kph_ + kSetSpeedStepKph, kMinActivationSpeedKph, kMaxSetSpeedKph);
            }
            if (down_edge) {
                set_speed_kph_ = clamp(set_speed_kph_ - kSetSpeedStepKph, kMinActivationSpeedKph, kMaxSetSpeedKph);
            }
            mode_ = (rx_driver_input_.accel_pedal_pos > kOverridePedalPct) ? Mode::kOverride : Mode::kActive;
            break;
        case Mode::kOff:
        case Mode::kFault:
            break;
    }
}

void AccController::control() noexcept {
    if (mode_ != Mode::kActive) {
        accel_raw_ = 0;  // no request outside ACTIVE; jerk limiting restarts from 0
        return;
    }
    const double v = rx_ego_state_.vehicle_speed / 3.6;
    double a = kSpeedGainPerS * (set_speed_kph_ / 3.6 - v);
    const bool in_path = on(rx_radar_object_.obj_valid) && rx_radar_object_.obj_range <= kMaxTargetRangeM &&
                         std::fabs(rx_radar_object_.obj_lateral_pos) <= kLaneHalfWidthM;
    if (in_path) {
        const double desired = kStandstillDistanceM + kTimeGapsS[gap_setting_ - 1U] * v;
        const double a_gap = kGapGainPerS2 * (rx_radar_object_.obj_range - desired) + kRangeRateGainPerS * rx_radar_object_.obj_range_rate;
        a = (a_gap < a) ? a_gap : a;
    }
    a = clamp(a, -kDecelMaxMps2, kAccelMaxMps2);
    const std::int32_t target = static_cast<std::int32_t>(ea::round_half_away(a / kAccelResolution));
    const std::int32_t delta = target - accel_raw_;
    accel_raw_ += (delta > kJerkUpStep) ? kJerkUpStep : ((delta < -kJerkDownStep) ? -kJerkDownStep : delta);
}

void AccController::send() noexcept {
    AccCommand cmd{};
    cmd.acc_status = static_cast<double>(static_cast<std::uint8_t>(mode_));
    cmd.accel_request = (accel_raw_ > 0) ? static_cast<double>(accel_raw_) * kAccelResolution : 0.0;
    cmd.brake_request = (accel_raw_ < 0) ? static_cast<double>(-accel_raw_) * kAccelResolution : 0.0;
    cmd.steering_angle_request = 0.0;  // longitudinal function only
    cmd.set_speed = set_speed_kph_;
    cmd.time_gap_setting = static_cast<double>(gap_setting_);
    tx_frame_.fill(0U);
    // Values are within the ranges proven by GEN-061; a failure is a contract violation.
    const ea::Result<void> enc = encode_acc_command(cmd, ea::ByteSpan(tx_frame_.data(), tx_frame_.size()));
    EA_ASSERT(enc.ok());
    const ea::Result<void> prot = e2e_tx_.protect(ea::ByteSpan(tx_frame_.data(), tx_frame_.size()));
    EA_ASSERT(prot.ok());
}

void AccController::step() noexcept {
    receive();
    update_mode();
    control();
    send();
}

}  // namespace adas::acc
