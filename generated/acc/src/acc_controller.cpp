// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc (pattern: can_function)
// Inputs:    sha256:7daf72a7ff67fdde3af69c03ce9ff58b4ecdb3b7244d701c28de0793bc835631
// Regenerate: python3 -m tools.gen

#include "acc/acc_controller.hpp"

#include <cstring>

#include "ea/core/assert.hpp"
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

}  // namespace

const char* to_string(State s) noexcept {
    switch (s) {
        case State::kOff:
            return "OFF";
        case State::kStandby:
            return "STANDBY";
        case State::kActive:
            return "ACTIVE";
        case State::kOverride:
            return "OVERRIDE";
        case State::kFault:
            return "FAULT";
    }
    return "UNKNOWN";
}

AccController::AccController() noexcept
    : e2e_radar_object_(kRadarObjectE2e), age_radar_object_(kRadarObjectTimeoutCycles), e2e_ego_state_(kEgoStateE2e), age_ego_state_(kEgoStateTimeoutCycles), e2e_driver_input_(kDriverInputE2e), age_driver_input_(kDriverInputTimeoutCycles), e2e_acc_command_(kAccCommandE2e) {
    send();  // all-zero outputs (state code 0 = initial state) are valid before the first cycle
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
        // RadarObject: frames failing E2E or decoding count as not received.
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
        // EgoState: frames failing E2E or decoding count as not received.
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
        // DriverInput: frames failing E2E or decoding count as not received.
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
    const bool radar_object_ok = age_radar_object_ < kRadarObjectTimeoutCycles;
    const bool ego_state_ok = age_ego_state_ < kEgoStateTimeoutCycles;
    const bool driver_input_ok = age_driver_input_ < kDriverInputTimeoutCycles;
    in_.accel_pedal = rx_driver_input_.accel_pedal_pos;
    in_.accel_pedal_ok = driver_input_ok;
    in_.brake_pedal = rx_driver_input_.brake_pedal;
    in_.brake_pedal_ok = driver_input_ok;
    in_.cancel_button = rx_driver_input_.cancel_button;
    in_.cancel_button_ok = driver_input_ok;
    in_.ego_speed = rx_ego_state_.vehicle_speed;
    in_.ego_speed_ok = ego_state_ok;
    in_.gap_setting = rx_driver_input_.gap_setting;
    in_.gap_setting_ok = driver_input_ok;
    in_.main_switch = rx_driver_input_.main_switch;
    in_.main_switch_ok = driver_input_ok;
    in_.resume_button = rx_driver_input_.resume_button;
    in_.resume_button_ok = driver_input_ok;
    in_.set_button = rx_driver_input_.set_button;
    in_.set_button_ok = driver_input_ok;
    in_.speed_down_button = rx_driver_input_.speed_down_button;
    in_.speed_down_button_ok = driver_input_ok;
    in_.speed_up_button = rx_driver_input_.speed_up_button;
    in_.speed_up_button_ok = driver_input_ok;
    in_.target_lateral = rx_radar_object_.obj_lateral_pos;
    in_.target_lateral_ok = radar_object_ok;
    in_.target_range = rx_radar_object_.obj_range;
    in_.target_range_ok = radar_object_ok;
    in_.target_range_rate = rx_radar_object_.obj_range_rate;
    in_.target_range_rate_ok = radar_object_ok;
    in_.target_valid = rx_radar_object_.obj_valid;
    in_.target_valid_ok = radar_object_ok;
    in_.all_ok = radar_object_ok && ego_state_ok && driver_input_ok;
}

void AccController::send() noexcept {
    {
        AccCommand msg{};
        msg.accel_request = out_.accel_request;
        msg.brake_request = out_.brake_request;
        msg.set_speed = out_.set_speed;
        msg.acc_status = out_.status;
        msg.steering_angle_request = out_.steering_request;
        msg.time_gap_setting = out_.time_gap_setting;
        tx_acc_command_.fill(0U);
        // Values are within the ranges proven by GEN-061; a failure is a contract violation.
        const ea::Result<void> enc = encode_acc_command(msg, ea::ByteSpan(tx_acc_command_.data(), tx_acc_command_.size()));
        EA_ASSERT(enc.ok());
        const ea::Result<void> prot = e2e_acc_command_.protect(ea::ByteSpan(tx_acc_command_.data(), tx_acc_command_.size()));
        EA_ASSERT(prot.ok());
    }
}

std::uint32_t AccController::tx_can_id(std::size_t i) const noexcept {
    switch (i) {
        case 0U:
            return kAccCommandCanId;
        default:
            return 0U;
    }
}

ea::ConstByteSpan AccController::tx_frame(std::size_t i) const noexcept {
    switch (i) {
        case 0U:
            return ea::ConstByteSpan(tx_acc_command_.data(), tx_acc_command_.size());
        default:
            return ea::ConstByteSpan();
    }
}

void AccController::step() noexcept {
    receive();
    logic_.step(in_, out_);
    send();
}

}  // namespace adas::acc
