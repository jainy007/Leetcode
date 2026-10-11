// Model of the actuator ECU (powertrain / brake) of a decomposed ASIL D function: controller
// B(D) + independent safety monitor B(D). This is the assumption of use (AoU) that the safety
// concept places on the actuator (docs/safety/acc_aeb_safety_concept.md):
//
//   AoU-1  A command is applied only if the monitor's latest valid frame reports MONITORING,
//          grants the permit and echoes exactly that command's E2E counter.
//   AoU-2  Without a new applicable command the last applied request is held for at most
//          `timeout_ticks`; afterwards it is ramped out (not latched).
//   AoU-3  When the monitor reports SAFE_STATE, or its frames stop for `timeout_ticks` after it
//          was MONITORING, the actuator latches the safe state: acceleration is cut at once,
//          a braking request is ramped out at `ramp_out_mps3`, and a takeover request is raised.
//
// Doubles as the FeatureObserver of monitor SIL and as the actuation observer of system SIL.
#pragma once

#include <cstdint>

#include "ea/e2e/p11.hpp"
#include "ea/plant/monitor_can.hpp"
#include "ea/plant/plant.hpp"

namespace ea::plant {

enum class MonitorStatus : std::uint8_t { kInit = 0, kMonitoring = 1, kSafeState = 2, kInvalid = 255 };

struct ActuatorConfig {
    std::uint32_t command_id;
    e2e::P11Config command_e2e;
    std::uint32_t monitor_id;
    e2e::P11Config monitor_e2e;
    // Decodes a valid command frame: true and the request (m/s2, < 0 brakes) if it actuates.
    bool (*decode)(ConstByteSpan frame, double& request_mps2) noexcept;
    std::uint32_t timeout_ticks;
    double ramp_out_mps3;
};

struct MonitorView {
    MonitorStatus status = MonitorStatus::kInvalid;
    bool permit = false;
    std::uint32_t approved_counter = 0U;
    std::uint32_t reason = 0U;
};

struct ActuatorMetrics {
    double monitor_safe_state_s = -1.0;  // first monitor frame reporting SAFE_STATE
    std::uint32_t first_reason = 0U;     // fault reason in that frame
    double safe_state_s = -1.0;          // actuator latched its safe state (AoU-3)
    double degraded_s = -1.0;            // first time a request was ramped out (AoU-2 or AoU-3)
    double ramp_out_done_s = -1.0;       // a ramp-out (AoU-2 or AoU-3) first reached 0
    double max_applied_accel_mps2 = 0.0;
    double max_applied_decel_mps2 = 0.0;
    std::uint32_t applied_commands = 0U;
    std::uint32_t rejected_commands = 0U;  // valid commands that were not approved
    std::uint32_t command_frames = 0U;
    std::uint32_t monitor_frames = 0U;
    std::uint32_t command_e2e_errors = 0U;
    std::uint32_t monitor_e2e_errors = 0U;
};

class MonitoredActuator : public FeatureObserver {
public:
    explicit MonitoredActuator(const ActuatorConfig& cfg) noexcept;

    void observe(const WorldView& world, Span<const BusFrame> frames) noexcept override;
    bool longitudinal_request(double& accel_mps2) const noexcept override;
    std::int32_t status_code() const noexcept override { return static_cast<std::int32_t>(mon_.status); }

    const MonitorView& monitor() const noexcept { return mon_; }
    const ActuatorMetrics& metrics() const noexcept { return m_; }
    bool takeover_request() const noexcept { return safe_; }
    bool applying() const noexcept { return has_request_; }
    double applied_mps2() const noexcept { return applied_; }

private:
    void ramp_out(double now_s) noexcept;

    ActuatorConfig cfg_;
    e2e::P11Checker cmd_rx_;
    e2e::P11Checker mon_rx_;
    MonitorView mon_{};
    bool mon_was_up_ = false;
    std::uint32_t mon_age_ = 0U;
    std::uint32_t hold_age_ = 0U;
    bool safe_ = false;
    bool has_request_ = false;
    double applied_ = 0.0;
    ActuatorMetrics m_{};
};

// ACC: AccCommand (accel - brake while ACTIVE) gated by AccMonitor; brake ramp-out 20 m/s3.
class AccActuator final : public MonitoredActuator {
public:
    AccActuator() noexcept;
};

// AEB: AebCommand (brake request) gated by AebMonitor; brake ramp-out 20 m/s3.
class AebActuator final : public MonitoredActuator {
public:
    AebActuator() noexcept;
};

}  // namespace ea::plant
