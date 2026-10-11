// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc_monitor (pattern: can_function)
// Inputs:    sha256:7aee4feecefbc18e2ad7476e4d93efffab091cab221dd8b1b9699ac5d31b6524
// Regenerate: python3 -m tools.gen

#include "acc_monitor/acc_monitor.hpp"

#include <cstring>

#include "ea/core/assert.hpp"
#include "ea/core/saturating.hpp"

namespace adas::acc_monitor {

namespace {
constexpr ea::e2e::P11Config kRadarObjectE2e{0x0200U, 0U, 8U, 8U, 3U};
constexpr std::uint16_t kRadarObjectTimeoutCycles = 6U;
constexpr ea::e2e::P11Config kAccCommandE2e{0x0300U, 0U, 8U, 8U, 3U};
constexpr std::uint16_t kAccCommandTimeoutCycles = 3U;
constexpr ea::e2e::P11Config kAccMonitorStatusE2e{0x0301U, 0U, 8U, 4U, 2U};

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

AccMonitor::AccMonitor() noexcept
    : e2e_radar_object_(kRadarObjectE2e), age_radar_object_(kRadarObjectTimeoutCycles), e2e_acc_command_(kAccCommandE2e), age_acc_command_(kAccCommandTimeoutCycles), e2e_acc_monitor_status_(kAccMonitorStatusE2e) {
    send();  // all-zero outputs (state code 0 = initial state) are valid before the first cycle
}

void AccMonitor::on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept {
    Mailbox* mb = nullptr;
    switch (can_id) {
        case kRadarObjectCanId:
            mb = &mb_radar_object_;
            break;
        case kAccCommandCanId:
            mb = &mb_acc_command_;
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

void AccMonitor::receive() noexcept {
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
        // AccCommand: frames failing E2E or decoding count as not received.
        bool received = false;
        if (mb_acc_command_.fresh) {
            mb_acc_command_.fresh = false;
            const ea::ConstByteSpan frame(mb_acc_command_.data.data(), mb_acc_command_.length);
            if (ea::e2e::is_usable(e2e_acc_command_.check(frame))) {
                const ea::Result<AccCommand> d = decode_acc_command(frame);
                if (d.ok()) {
                    rx_acc_command_ = *d;
                    received = true;
                }
            }
        }
        age_acc_command_ = received ? static_cast<std::uint16_t>(0U) : ea::sat_inc(age_acc_command_);
    }
    const bool radar_object_ok = age_radar_object_ < kRadarObjectTimeoutCycles;
    const bool acc_command_ok = age_acc_command_ < kAccCommandTimeoutCycles;
    in_.cmd_accel = rx_acc_command_.accel_request;
    in_.cmd_accel_ok = acc_command_ok;
    in_.cmd_brake = rx_acc_command_.brake_request;
    in_.cmd_brake_ok = acc_command_ok;
    in_.cmd_counter = rx_acc_command_.counter;
    in_.cmd_counter_ok = acc_command_ok;
    in_.cmd_status = rx_acc_command_.acc_status;
    in_.cmd_status_ok = acc_command_ok;
    in_.target_lateral = rx_radar_object_.obj_lateral_pos;
    in_.target_lateral_ok = radar_object_ok;
    in_.target_range = rx_radar_object_.obj_range;
    in_.target_range_ok = radar_object_ok;
    in_.target_range_rate = rx_radar_object_.obj_range_rate;
    in_.target_range_rate_ok = radar_object_ok;
    in_.target_valid = rx_radar_object_.obj_valid;
    in_.target_valid_ok = radar_object_ok;
    in_.all_ok = radar_object_ok && acc_command_ok;
}

void AccMonitor::send() noexcept {
    {
        AccMonitorStatus msg{};
        msg.approved_counter = out_.approved_counter;
        msg.fault_reason = out_.fault_reason;
        msg.permit = out_.permit;
        msg.mon_status = out_.status;
        tx_acc_monitor_status_.fill(0U);
        // Values are within the ranges proven by GEN-061; a failure is a contract violation.
        const ea::Result<void> enc = encode_acc_monitor_status(msg, ea::ByteSpan(tx_acc_monitor_status_.data(), tx_acc_monitor_status_.size()));
        EA_ASSERT(enc.ok());
        const ea::Result<void> prot = e2e_acc_monitor_status_.protect(ea::ByteSpan(tx_acc_monitor_status_.data(), tx_acc_monitor_status_.size()));
        EA_ASSERT(prot.ok());
    }
}

std::uint32_t AccMonitor::tx_can_id(std::size_t i) const noexcept {
    switch (i) {
        case 0U:
            return kAccMonitorStatusCanId;
        default:
            return 0U;
    }
}

ea::ConstByteSpan AccMonitor::tx_frame(std::size_t i) const noexcept {
    switch (i) {
        case 0U:
            return ea::ConstByteSpan(tx_acc_monitor_status_.data(), tx_acc_monitor_status_.size());
        default:
            return ea::ConstByteSpan();
    }
}

void AccMonitor::step() noexcept {
    receive();
    logic_.step(in_, out_);
    send();
}

}  // namespace adas::acc_monitor
