// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb_monitor (pattern: can_function)
// Inputs:    sha256:230536ebb5fd8cf2e5c6eff700cb3596edf001524e2100e20ccfddcdcbbad55e
// Regenerate: python3 -m tools.gen

#include "aeb_monitor/aeb_monitor.hpp"

#include <cstring>

#include "ea/core/assert.hpp"
#include "ea/core/saturating.hpp"

namespace adas::aeb_monitor {

namespace {
constexpr ea::e2e::P11Config kRadarObjectE2e{0x0200U, 0U, 8U, 8U, 3U};
constexpr std::uint16_t kRadarObjectTimeoutCycles = 6U;
constexpr ea::e2e::P11Config kAebCommandE2e{0x0310U, 0U, 8U, 8U, 3U};
constexpr std::uint16_t kAebCommandTimeoutCycles = 3U;
constexpr ea::e2e::P11Config kAebMonitorStatusE2e{0x0311U, 0U, 8U, 4U, 2U};

}  // namespace

const char* to_string(State s) noexcept {
    switch (s) {
        case State::kInit:
            return "INIT";
        case State::kMonitoring:
            return "MONITORING";
        case State::kSafeState:
            return "SAFE_STATE";
    }
    return "UNKNOWN";
}

AebMonitor::AebMonitor() noexcept
    : e2e_radar_object_(kRadarObjectE2e), age_radar_object_(kRadarObjectTimeoutCycles), e2e_aeb_command_(kAebCommandE2e), age_aeb_command_(kAebCommandTimeoutCycles), e2e_aeb_monitor_status_(kAebMonitorStatusE2e) {
    send();  // all-zero outputs (state code 0 = initial state) are valid before the first cycle
}

void AebMonitor::on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept {
    Mailbox* mb = nullptr;
    switch (can_id) {
        case kRadarObjectCanId:
            mb = &mb_radar_object_;
            break;
        case kAebCommandCanId:
            mb = &mb_aeb_command_;
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

void AebMonitor::receive() noexcept {
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
        // AebCommand: frames failing E2E or decoding count as not received.
        bool received = false;
        if (mb_aeb_command_.fresh) {
            mb_aeb_command_.fresh = false;
            const ea::ConstByteSpan frame(mb_aeb_command_.data.data(), mb_aeb_command_.length);
            if (ea::e2e::is_usable(e2e_aeb_command_.check(frame))) {
                const ea::Result<AebCommand> d = decode_aeb_command(frame);
                if (d.ok()) {
                    rx_aeb_command_ = *d;
                    received = true;
                }
            }
        }
        age_aeb_command_ = received ? static_cast<std::uint16_t>(0U) : ea::sat_inc(age_aeb_command_);
    }
    const bool radar_object_ok = age_radar_object_ < kRadarObjectTimeoutCycles;
    const bool aeb_command_ok = age_aeb_command_ < kAebCommandTimeoutCycles;
    in_.cmd_brake = rx_aeb_command_.aeb_brake_request;
    in_.cmd_brake_ok = aeb_command_ok;
    in_.cmd_counter = rx_aeb_command_.counter;
    in_.cmd_counter_ok = aeb_command_ok;
    in_.cmd_status = rx_aeb_command_.aeb_status;
    in_.cmd_status_ok = aeb_command_ok;
    in_.target_lateral = rx_radar_object_.obj_lateral_pos;
    in_.target_lateral_ok = radar_object_ok;
    in_.target_range = rx_radar_object_.obj_range;
    in_.target_range_ok = radar_object_ok;
    in_.target_range_rate = rx_radar_object_.obj_range_rate;
    in_.target_range_rate_ok = radar_object_ok;
    in_.target_valid = rx_radar_object_.obj_valid;
    in_.target_valid_ok = radar_object_ok;
    in_.all_ok = radar_object_ok && aeb_command_ok;
}

void AebMonitor::send() noexcept {
    {
        AebMonitorStatus msg{};
        msg.approved_counter = out_.approved_counter;
        msg.fault_reason = out_.fault_reason;
        msg.permit = out_.permit;
        msg.mon_status = out_.status;
        tx_aeb_monitor_status_.fill(0U);
        // Values are within the ranges proven by GEN-061; a failure is a contract violation.
        const ea::Result<void> enc = encode_aeb_monitor_status(msg, ea::ByteSpan(tx_aeb_monitor_status_.data(), tx_aeb_monitor_status_.size()));
        EA_ASSERT(enc.ok());
        const ea::Result<void> prot = e2e_aeb_monitor_status_.protect(ea::ByteSpan(tx_aeb_monitor_status_.data(), tx_aeb_monitor_status_.size()));
        EA_ASSERT(prot.ok());
    }
}

std::uint32_t AebMonitor::tx_can_id(std::size_t i) const noexcept {
    switch (i) {
        case 0U:
            return kAebMonitorStatusCanId;
        default:
            return 0U;
    }
}

ea::ConstByteSpan AebMonitor::tx_frame(std::size_t i) const noexcept {
    switch (i) {
        case 0U:
            return ea::ConstByteSpan(tx_aeb_monitor_status_.data(), tx_aeb_monitor_status_.size());
        default:
            return ea::ConstByteSpan();
    }
}

void AebMonitor::step() noexcept {
    receive();
    logic_.step(in_, out_);
    send();
}

}  // namespace adas::aeb_monitor
