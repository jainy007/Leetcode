// Closed-loop plant for SIL of CAN vehicle functions: ego and lead vehicle kinematics, radar
// sensor, driver, a CAN bus between one or more generated nodes (e.g. controller -> safety
// monitor), fault injection on sensor streams and on node outputs, and vehicle-level metrics.
// Decoding of node outputs lives in FeatureObservers (acc_observer.hpp, ...). One observer is
// the actuation observer: it models the actuator ECU and tells the plant what is applied.
// Hand-written test infrastructure, independent of the generator.
//
// Time advances in fixed 20 ms ticks. Within a tick:
//   1. due sensor frames (radar every 40 ms, ego state and driver input every 20 ms) -> all nodes
//   2. nodes step in order; each node's tx frames pass the node-output fault injector and are
//      delivered to all other nodes at once (later nodes use them in this tick)
//   3. observers see every frame sent on the bus this tick
//   4. vehicles integrate over 20 ms using the actuation observer's request
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "ea/core/span.hpp"
#include "ea/e2e/p11.hpp"

namespace ea::plant {

enum class Button : std::uint8_t { kSet, kResume, kCancel, kSpeedUp, kSpeedDown };
enum class Stream : std::uint8_t { kRadar, kEgoState, kDriverInput };

// Fault on frames sent by a node (simulates a faulty controller ECU or bus segment).
enum class NodeFault : std::uint8_t {
    kLose,      // frames disappear
    kFreeze,    // the last good frame is repeated byte for byte (stuck sender, counter frozen)
    kCorrupt,   // a payload bit flips after E2E protection
    kOverride,  // a signal gets a wrong value; CRC recomputed (logic fault behind valid E2E)
};

struct BusFrame {
    std::uint32_t can_id;
    std::array<std::uint8_t, 8> data;
    std::size_t length;
    ConstByteSpan payload() const noexcept { return ConstByteSpan(data.data(), length); }
};

// The function under test, as seen from the bus.
class ControllerPort {
public:
    virtual ~ControllerPort() = default;
    virtual void on_frame(std::uint32_t can_id, ConstByteSpan payload) noexcept = 0;
    virtual void step() noexcept = 0;
    virtual std::size_t tx_count() const noexcept = 0;
    virtual std::uint32_t tx_can_id(std::size_t i) const noexcept = 0;
    virtual ConstByteSpan tx_frame(std::size_t i) const noexcept = 0;

protected:
    ControllerPort() = default;
    ControllerPort(const ControllerPort&) = default;
    ControllerPort& operator=(const ControllerPort&) = default;
};

// What an observer may see of the world at the end of a tick.
struct WorldView {
    double time_s;
    double ego_speed_mps;
    bool lead_present;
    bool lead_in_lane;
    double distance_m;          // lead x - ego x
    double closing_speed_mps;   // ego v - lead v (> 0: approaching)
};

// Decodes a feature's output frames and tells the plant what the function commands.
class FeatureObserver {
public:
    virtual ~FeatureObserver() = default;
    // `frames`: every frame nodes sent on the bus this tick (after fault injection).
    virtual void observe(const WorldView& world, Span<const BusFrame> frames) noexcept = 0;
    // Longitudinal acceleration the function currently commands (m/s2); false if none.
    virtual bool longitudinal_request(double& accel_mps2) const noexcept = 0;
    // Current state code reported by the function (for driver-action latency measurement).
    virtual std::int32_t status_code() const noexcept = 0;

protected:
    FeatureObserver() = default;
    FeatureObserver(const FeatureObserver&) = default;
    FeatureObserver& operator=(const FeatureObserver&) = default;
};

struct VehicleMetrics {
    double min_distance_m = 1e9;  // to an in-lane lead vehicle
    bool collision = false;
    double impact_speed_kph = 0.0;  // closing speed at the first collision
    double max_ego_speed_kph = 0.0;
    double max_accel_mps2 = 0.0;   // largest actual ego acceleration
    double max_decel_mps2 = 0.0;   // largest actual ego deceleration (positive)
    double first_fault_s = -1.0;   // time the first injected fault took effect
    std::uint32_t ticks = 0;
};

class Plant {
public:
    static constexpr double kTickS = 0.02;
    static constexpr double kActuatorTauS = 0.3;     // first-order lag of powertrain and brakes
    static constexpr double kDriverBrakeMps2 = 2.0;  // deceleration while the driver brakes
    static constexpr double kPedalGainMps2PerPct = 0.02;
    static constexpr double kRadarRangeM = 250.0;
    static constexpr double kLaneHalfWidthM = 1.8;   // "in lane" for vehicle metrics only

    Plant() noexcept;

    // Requirement thresholds in raw units of 0.01 m/s2 command signals.
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

    // Faults on the next frames of a stream
    void lose_frames(Stream s, std::uint32_t n) noexcept;     // lost on the bus (counter consumed)
    void corrupt_frames(Stream s, std::uint32_t n) noexcept;  // bit flip after protection
    void skip_frames(Stream s, std::uint32_t n) noexcept;     // sender pause (counter not consumed)

    // Fault on the next n frames with `can_id` sent by any node. For kOverride, `sig` gets
    // `value` and the CRC is recomputed with `e2e` (counter unchanged).
    void inject_node_fault(std::uint32_t can_id, NodeFault kind, std::uint32_t n, const e2e::P11Config& e2e,
                           std::uint32_t sig_start = 0U, std::uint32_t sig_length = 0U, bool sig_signed = false,
                           double sig_factor = 1.0, double value = 0.0) noexcept;

    // Single node: the node's observer is also the actuation observer.
    void run(ControllerPort& sut, FeatureObserver& obs, double seconds) noexcept;
    // Several nodes stepping in the given order; observers[actuation] drives the vehicle and
    // observers[0] provides the status for driver-action latency.
    void run(Span<ControllerPort* const> nodes, Span<FeatureObserver* const> observers, std::size_t actuation,
             double seconds) noexcept;

    // Observation
    double ego_speed_kph() const noexcept { return ego_v_ * 3.6; }
    double distance_m() const noexcept { return lead_x_ - ego_x_; }
    bool lead_present() const noexcept { return lead_present_; }
    double time_s() const noexcept { return static_cast<double>(now_ms_) / 1000.0; }
    const VehicleMetrics& metrics() const noexcept { return m_; }
    // Time from the last driver action to the first change of the function's state (-1 if none).
    double status_change_latency_ms() const noexcept { return latency_ms_; }

private:
    void tick(Span<ControllerPort* const> nodes, Span<FeatureObserver* const> observers, std::size_t actuation) noexcept;
    void send(Span<ControllerPort* const> nodes, Stream s, std::uint32_t id, std::array<std::uint8_t, 8>& f,
              e2e::P11Protector& prot) noexcept;
    bool apply_node_fault(BusFrame& f) noexcept;  // false: frame is lost
    void mark_fault() noexcept;

    struct NodeFaultSpec {
        std::uint32_t can_id = 0U;
        NodeFault kind = NodeFault::kLose;
        std::uint32_t remaining = 0U;
        e2e::P11Config e2e{};
        std::uint32_t sig_start = 0U;
        std::uint32_t sig_length = 0U;
        bool sig_signed = false;
        double sig_factor = 1.0;
        double value = 0.0;
        bool have_last = false;
        BusFrame last{};
    };
    void mark_driver_action() noexcept;
    WorldView view() const noexcept;

    std::int64_t now_ms_ = 0;
    double ego_x_ = 0.0;
    double ego_v_ = 0.0;
    double ego_a_ = 0.0;
    bool lead_present_ = false;
    double lead_x_ = 0.0;
    double lead_v_ = 0.0;
    double lead_lat_ = 0.0;
    double lead_target_v_ = 0.0;
    double lead_accel_ = 0.0;
    bool main_switch_ = false;
    std::array<std::int64_t, 5> button_until_ms_{};
    bool brake_pedal_ = false;
    double accel_pedal_pct_ = 0.0;
    std::uint32_t gap_setting_ = 2U;
    e2e::P11Protector radar_tx_;
    e2e::P11Protector ego_tx_;
    e2e::P11Protector driver_tx_;
    std::array<std::uint32_t, 3> lose_{};
    std::array<std::uint32_t, 3> corrupt_{};
    std::array<std::uint32_t, 3> skip_{};
    static constexpr std::size_t kMaxNodeFaults = 4U;
    std::array<NodeFaultSpec, kMaxNodeFaults> node_faults_{};
    std::array<BusFrame, 16> tick_frames_{};
    std::size_t tick_frame_count_ = 0U;
    double prev_ego_v_ = 0.0;
    VehicleMetrics m_{};
    std::int64_t action_ms_ = -1;
    std::int32_t status_at_action_ = -1;
    double latency_ms_ = -1.0;
    std::int32_t last_status_ = -1;
};

}  // namespace ea::plant
