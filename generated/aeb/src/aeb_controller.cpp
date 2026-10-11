// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb (pattern: can_function)
// Inputs:    sha256:7985079d35cf79be077da24af2c4f9d16c2c565e0ce9d01d499b7c22d7f7c8d2
// Regenerate: python3 -m tools.gen

#include "aeb/aeb_controller.hpp"

#include <cstring>

#include "ea/core/assert.hpp"
#include "ea/core/saturating.hpp"

namespace adas::aeb {

namespace {
constexpr ea::e2e::P11Config kRadarObjectE2e{0x0200U, 0U, 8U, 8U, 3U};
constexpr std::uint16_t kRadarObjectTimeoutCycles = 6U;
constexpr ea::e2e::P11Config kEgoStateE2e{0x0210U, 0U, 8U, 8U, 3U};
constexpr std::uint16_t kEgoStateTimeoutCycles = 3U;
constexpr ea::e2e::P11Config kDriverInputE2e{0x0220U, 0U, 8U, 8U, 3U};
constexpr std::uint16_t kDriverInputTimeoutCycles = 3U;
constexpr ea::e2e::P11Config kAebCommandE2e{0x0310U, 0U, 8U, 8U, 2U};

}  // namespace

const char* to_string(State s) noexcept {
    switch (s) {
        case State::kStandby:
            return "STANDBY";
        case State::kWarning:
            return "WARNING";
        case State::kPartialBrake:
            return "PARTIAL_BRAKE";
        case State::kFullBrake:
            return "FULL_BRAKE";
        case State::kFault:
            return "FAULT";
    }
    return "UNKNOWN";
}

AebController::AebController() noexcept
    : e2e_radar_object_(kRadarObjectE2e), age_radar_object_(kRadarObjectTimeoutCycles), e2e_ego_state_(kEgoStateE2e), age_ego_state_(kEgoStateTimeoutCycles), e2e_driver_input_(kDriverInputE2e), age_driver_input_(kDriverInputTimeoutCycles), e2e_aeb_command_(kAebCommandE2e) {
    send();  // all-zero outputs (state code 0 = initial state) are valid before the first cycle
}

void AebController::on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept {
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

void AebController::receive() noexcept {
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
    in_.ego_speed = rx_ego_state_.vehicle_speed;
    in_.ego_speed_ok = ego_state_ok;
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

void AebController::send() noexcept {
    {
        AebCommand msg{};
        msg.aeb_brake_request = out_.brake_request;
        msg.fcw_warning = out_.fcw_warning;
        msg.aeb_status = out_.status;
        tx_aeb_command_.fill(0U);
        // Values are within the ranges proven by GEN-061; a failure is a contract violation.
        const ea::Result<void> enc = encode_aeb_command(msg, ea::ByteSpan(tx_aeb_command_.data(), tx_aeb_command_.size()));
        EA_ASSERT(enc.ok());
        const ea::Result<void> prot = e2e_aeb_command_.protect(ea::ByteSpan(tx_aeb_command_.data(), tx_aeb_command_.size()));
        EA_ASSERT(prot.ok());
    }
}

std::uint32_t AebController::tx_can_id(std::size_t i) const noexcept {
    switch (i) {
        case 0U:
            return kAebCommandCanId;
        default:
            return 0U;
    }
}

ea::ConstByteSpan AebController::tx_frame(std::size_t i) const noexcept {
    switch (i) {
        case 0U:
            return ea::ConstByteSpan(tx_aeb_command_.data(), tx_aeb_command_.size());
        default:
            return ea::ConstByteSpan();
    }
}

void AebController::step() noexcept {
    receive();
    logic_.step(in_, out_);
    send();
}

}  // namespace adas::aeb
