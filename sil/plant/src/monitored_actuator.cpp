#include "ea/plant/monitored_actuator.hpp"

#include "ea/plant/acc_can.hpp"
#include "ea/plant/aeb_can.hpp"
#include "ea/plant/monitor_can.hpp"
#include "ea/plant/signals.hpp"

namespace ea::plant {

namespace {

constexpr double kTickS = Plant::kTickS;

std::uint32_t counter_of(ConstByteSpan f, const e2e::P11Config& cfg) noexcept {
    const can::Sig ctr{cfg.counter_bit, 4U, false, 1.0};
    return static_cast<std::uint32_t>(get_raw(f, ctr));
}

bool decode_acc(ConstByteSpan f, double& request) noexcept {
    if (get_raw(f, can::acc_command::kAccStatus) != 2) {  // ACTIVE (REQ-ACC-020)
        return false;
    }
    request = static_cast<double>(get_raw(f, can::acc_command::kAccelRequest) -
                                  get_raw(f, can::acc_command::kBrakeRequest)) * 0.01;
    return true;
}

bool decode_aeb(ConstByteSpan f, double& request) noexcept {
    const std::int64_t brake = get_raw(f, can::aeb_command::kAebBrakeRequest);
    if (brake <= 0) {
        return false;
    }
    request = -static_cast<double>(brake) * 0.01;
    return true;
}

}  // namespace

MonitoredActuator::MonitoredActuator(const ActuatorConfig& cfg) noexcept
    : cfg_(cfg), cmd_rx_(cfg.command_e2e), mon_rx_(cfg.monitor_e2e) {}

void MonitoredActuator::ramp_out(double now_s) noexcept {
    if (m_.degraded_s < 0.0 && has_request_) {
        m_.degraded_s = now_s;
    }
    if (applied_ > 0.0) {
        applied_ = 0.0;  // acceleration is cut at once
    } else {
        const double step = cfg_.ramp_out_mps3 * kTickS;
        applied_ = (applied_ + step < 0.0) ? applied_ + step : 0.0;
    }
    const bool had = has_request_;
    has_request_ = applied_ < 0.0;
    if (had && !has_request_ && m_.ramp_out_done_s < 0.0) {
        m_.ramp_out_done_s = now_s;
    }
}

void MonitoredActuator::observe(const WorldView& world, Span<const BusFrame> frames) noexcept {
    bool cmd_new = false;
    bool cmd_actuates = false;
    double cmd_request = 0.0;
    std::uint32_t cmd_counter = 0U;
    bool mon_new = false;
    for (const BusFrame& bf : frames) {
        if (bf.can_id == cfg_.command_id) {
            const ConstByteSpan f = bf.payload();
            ++m_.command_frames;
            if (!e2e::is_usable(cmd_rx_.check(f))) {
                ++m_.command_e2e_errors;
                continue;
            }
            cmd_new = true;
            cmd_counter = counter_of(f, cfg_.command_e2e);
            cmd_actuates = cfg_.decode(f, cmd_request);
        } else if (bf.can_id == cfg_.monitor_id) {
            const ConstByteSpan f = bf.payload();
            ++m_.monitor_frames;
            if (!e2e::is_usable(mon_rx_.check(f))) {
                ++m_.monitor_e2e_errors;
                continue;
            }
            mon_new = true;
            const std::int64_t st = get_raw(f, can::monitor::kMonStatus);
            mon_.status = (st <= 2) ? static_cast<MonitorStatus>(st) : MonitorStatus::kInvalid;
            mon_.permit = get_raw(f, can::monitor::kPermit) == 1;
            mon_.approved_counter = static_cast<std::uint32_t>(get_raw(f, can::monitor::kApprovedCounter));
            mon_.reason = static_cast<std::uint32_t>(get_raw(f, can::monitor::kFaultReason));
        } else {
            // other traffic
        }
    }
    mon_age_ = mon_new ? 0U : mon_age_ + 1U;
    if (mon_.status == MonitorStatus::kMonitoring) {
        mon_was_up_ = true;
    }
    if (mon_.status == MonitorStatus::kSafeState && m_.monitor_safe_state_s < 0.0) {
        m_.monitor_safe_state_s = world.time_s;
        m_.first_reason = mon_.reason;
    }
    // AoU-3: latched safe state.
    const bool mon_lost = mon_was_up_ && mon_age_ >= cfg_.timeout_ticks;
    if (!safe_ && (mon_.status == MonitorStatus::kSafeState || mon_lost)) {
        safe_ = true;
        m_.safe_state_s = world.time_s;
    }
    if (safe_) {
        ramp_out(world.time_s);
        return;
    }
    // AoU-1: apply only the approved command.
    const bool approved = cmd_new && mon_new && mon_.status == MonitorStatus::kMonitoring && mon_.permit &&
                          mon_.approved_counter == cmd_counter;
    if (approved) {
        hold_age_ = 0U;
        ++m_.applied_commands;
        has_request_ = cmd_actuates;
        applied_ = cmd_actuates ? cmd_request : 0.0;
        if (applied_ > m_.max_applied_accel_mps2) {
            m_.max_applied_accel_mps2 = applied_;
        }
        if (-applied_ > m_.max_applied_decel_mps2) {
            m_.max_applied_decel_mps2 = -applied_;
        }
        return;
    }
    if (cmd_new) {
        ++m_.rejected_commands;
    }
    // AoU-2: hold, then ramp out.
    ++hold_age_;
    if (hold_age_ >= cfg_.timeout_ticks) {
        ramp_out(world.time_s);
    }
}

bool MonitoredActuator::longitudinal_request(double& accel_mps2) const noexcept {
    if (!has_request_) {
        return false;
    }
    accel_mps2 = applied_;
    return true;
}

AccActuator::AccActuator() noexcept
    : MonitoredActuator(ActuatorConfig{can::kAccCommandId, can::kAccCommandE2e, can::kAccMonitorId, can::kAccMonitorE2e,
                                       &decode_acc, 3U, 20.0}) {}

AebActuator::AebActuator() noexcept
    : MonitoredActuator(ActuatorConfig{can::kAebCommandId, can::kAebCommandE2e, can::kAebMonitorId, can::kAebMonitorE2e,
                                       &decode_aeb, 3U, 20.0}) {}

}  // namespace ea::plant
