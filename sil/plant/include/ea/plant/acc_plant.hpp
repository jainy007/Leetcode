// Closed-loop plant for ACC SIL: ego and lead vehicle kinematics, radar sensor, driver, CAN bus
// timing, fault injection and metrics. Hand-written test infrastructure, independent of the
// generator. Time advances in fixed 20 ms ticks; within a tick the order is:
//   1. due frames (radar every 40 ms, ego state and driver input every 20 ms) -> controller
//   2. controller step()
//   3. command frame decoded and checked by the plant
//   4. vehicles integrate over 20 ms
#pragma once

#include <array>
#include <cstdint>

#include "ea/core/span.hpp"
#include "ea/e2e/p11.hpp"

namespace ea::plant {

// Mode codes as specified by REQ-ACC-020 / the DBC comment (not taken from generated code).
enum class AccStatus : std::uint8_t { kOff = 0, kStandby = 1, kActive = 2, kOverride = 3, kFault = 4, kInvalid = 255 };

enum class Button : std::uint8_t { kSet, kResume, kCancel, kSpeedUp, kSpeedDown };

enum class Stream : std::uint8_t { kRadar, kEgoState, kDriverInput };

// The system under test, as seen from the bus.
class ControllerPort {
public:
    virtual ~ControllerPort() = default;
    virtual void on_frame(std::uint32_t can_id, ConstByteSpan payload) noexcept = 0;
    virtual void step() noexcept = 0;
    virtual ConstByteSpan command_frame() const noexcept = 0;

protected:
    ControllerPort() = default;
    ControllerPort(const ControllerPort&) = default;
    ControllerPort& operator=(const ControllerPort&) = default;
};

struct Command {
    AccStatus status = AccStatus::kInvalid;
    std::int32_t accel_raw = 0;  // 0.01 m/s2
    std::int32_t brake_raw = 0;  // 0.01 m/s2
    double steering_deg = 0.0;
    double set_speed_kph = 0.0;
    std::uint32_t time_gap_setting = 0U;
};

struct Metrics {
    double min_distance_m = 1e9;  // to an in-lane lead vehicle
    bool collision = false;
    double max_ego_speed_kph = 0.0;
    std::int32_t max_accel_raw = 0;
    std::int32_t max_brake_raw = 0;
    std::int32_t max_jerk_up_raw = 0;    // largest increase of (accel - brake) per tick while ACTIVE
    std::int32_t max_jerk_down_raw = 0;  // largest decrease per tick while ACTIVE
    std::uint32_t both_requested = 0;
    std::uint32_t steering_nonzero = 0;
    std::uint32_t command_frames = 0;
    std::uint32_t command_e2e_errors = 0;
    std::uint32_t ticks = 0;
};

class AccPlant {
public:
    static constexpr double kTickS = 0.02;
    static constexpr double kActuatorTauS = 0.3;     // first-order lag of the powertrain/brakes
    static constexpr double kDriverBrakeMps2 = 2.0;  // deceleration while the driver brakes
    static constexpr double kPedalGainMps2PerPct = 0.02;
    static constexpr double kRadarRangeM = 250.0;
    static constexpr double kLaneHalfWidthM = 1.8;   // for the "in lane" distance metric only

    AccPlant() noexcept;

    // Requirement thresholds in raw units of the command signals (0.01 m/s2 resolution).
    static std::int32_t accel_raw(double mps2) noexcept;
    static std::int32_t jerk_step_raw(double mps3) noexcept;  // allowed change per 20 ms tick

    // Scenario set-up
    void set_ego_speed_kph(double v) noexcept;
    void set_lead(double distance_m, double speed_kph, double lateral_m) noexcept;
    void remove_lead() noexcept;
    void lead_change_speed(double target_kph, double accel_mps2) noexcept;
    void cut_in(double distance_m, double speed_kph) noexcept;

    // Driver
    void set_main_switch(bool on) noexcept;
    void press(Button b) noexcept;  // held for 100 ms
    void set_brake_pedal(bool pressed) noexcept;
    void set_accel_pedal(double pct) noexcept;
    void set_gap_setting(std::uint32_t setting) noexcept;

    // Faults (applied to the next frames of a stream)
    void lose_frames(Stream s, std::uint32_t n) noexcept;
    void corrupt_frames(Stream s, std::uint32_t n) noexcept;
    // Sender pause: the next n frames are not sent and their counters are not consumed.
    void skip_frames(Stream s, std::uint32_t n) noexcept;

    void run(ControllerPort& sut, double seconds) noexcept;

    // Observation
    double ego_speed_kph() const noexcept { return ego_v_ * 3.6; }
    double distance_m() const noexcept { return lead_x_ - ego_x_; }
    bool lead_present() const noexcept { return lead_present_; }
    const Command& last_command() const noexcept { return cmd_; }
    const Metrics& metrics() const noexcept { return m_; }
    // Time from the last driver action to the first command with a different status (-1 if none).
    double status_change_latency_ms() const noexcept { return latency_ms_; }

private:
    void tick(ControllerPort& sut) noexcept;
    void send(ControllerPort& sut, Stream s, std::uint32_t id, std::array<std::uint8_t, 8>& f,
              e2e::P11Protector& prot) noexcept;
    void decode_command(ConstByteSpan f) noexcept;
    void mark_driver_action() noexcept;

    std::int64_t now_ms_ = 0;
    // vehicles
    double ego_x_ = 0.0;
    double ego_v_ = 0.0;
    double ego_a_ = 0.0;
    bool lead_present_ = false;
    double lead_x_ = 0.0;
    double lead_v_ = 0.0;
    double lead_lat_ = 0.0;
    double lead_target_v_ = 0.0;
    double lead_accel_ = 0.0;
    // driver
    bool main_switch_ = false;
    std::array<std::int64_t, 5> button_until_ms_{};
    bool brake_pedal_ = false;
    double accel_pedal_pct_ = 0.0;
    std::uint32_t gap_setting_ = 2U;
    // bus
    e2e::P11Protector radar_tx_;
    e2e::P11Protector ego_tx_;
    e2e::P11Protector driver_tx_;
    e2e::P11Checker cmd_rx_;
    std::array<std::uint32_t, 3> lose_{};
    std::array<std::uint32_t, 3> corrupt_{};
    std::array<std::uint32_t, 3> skip_{};
    // observation
    Command cmd_{};
    bool have_prev_cmd_ = false;
    Command prev_cmd_{};
    Metrics m_{};
    std::int64_t action_ms_ = -1;
    AccStatus status_at_action_ = AccStatus::kInvalid;
    double latency_ms_ = -1.0;
};

}  // namespace ea::plant
