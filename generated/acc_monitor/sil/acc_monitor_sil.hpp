// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc_monitor (pattern: can_function)
// Inputs:    sha256:7aee4feecefbc18e2ad7476e4d93efffab091cab221dd8b1b9699ac5d31b6524
// Regenerate: python3 -m tools.gen

#pragma once

#include "ea/plant/plant.hpp"
#include "ea/plant/monitored_actuator.hpp"
#include "ea/plant/command_stub.hpp"
#include "acc_monitor/acc_monitor.hpp"

namespace adas::acc_monitor {

// Closed-loop SIL harness: the generated function on the plant's CAN bus. The plant simulates
// vehicles, radar, driver and bus; the feature observer decodes the function's output frames.
class AccMonitorSil final : public ea::plant::ControllerPort {
public:
    static constexpr double kDriverActionS = 0.2;  // time allowed for each driver action

    void on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept override { ctrl_.on_frame(can_id, payload); }
    void step() noexcept override { ctrl_.step(); }
    std::size_t tx_count() const noexcept override { return AccMonitor::kTxCount; }
    std::uint32_t tx_can_id(std::size_t i) const noexcept override { return ctrl_.tx_can_id(i); }
    ea::ConstByteSpan tx_frame(std::size_t i) const noexcept override { return ctrl_.tx_frame(i); }

    // The stimulus node stands in for the function's command source and steps first.
    void run(double seconds) noexcept {
        ea::plant::ControllerPort* const nodes[] = {&stim_, this};
        ea::plant::FeatureObserver* const observers[] = {&observer_};
        plant_.run(ea::Span<ea::plant::ControllerPort* const>(nodes), ea::Span<ea::plant::FeatureObserver* const>(observers),
                   0U, seconds);
    }
    ea::plant::AccCommandStub& stimulus() noexcept { return stim_; }
    ea::plant::Plant& plant() noexcept { return plant_; }
    ea::plant::AccActuator& feature() noexcept { return observer_; }
    const AccMonitor& controller() const noexcept { return ctrl_; }

    // Driver actions shared by all features (each followed by kDriverActionS of simulation).
    void switch_main(bool on_state) noexcept {
        plant_.set_main_switch(on_state);
        run(kDriverActionS);
    }
    void press(ea::plant::Button b, int times) noexcept {
        for (int i = 0; i < times; ++i) {
            plant_.press(b);
            run(kDriverActionS);
        }
    }
    void brake(bool pressed) noexcept {
        plant_.set_brake_pedal(pressed);
        run(kDriverActionS);
    }
    void accelerator(double pct) noexcept {
        plant_.set_accel_pedal(pct);
        run(kDriverActionS);
    }
    // ---- feature 'acc_monitor' command sequences (the stimulus stands in for the controller) ----
    ea::plant::MonitoredActuator& actuator() noexcept { return observer_; }
    // Faults on the next n frames of the stimulus (controller) or of the monitor under test.
    void command_fault(ea::plant::NodeFault kind, std::uint32_t n) noexcept {
        plant_.inject_node_fault(ea::plant::can::kAccCommandId, kind, n, ea::plant::can::kAccCommandE2e);
    }
    void monitor_fault(ea::plant::NodeFault kind, std::uint32_t n) noexcept {
        plant_.inject_node_fault(ea::plant::can::kAccMonitorId, kind, n, ea::plant::can::kAccMonitorE2e);
    }
    // Sets the stimulus' state code and net longitudinal request (> 0 accelerates, < 0 brakes).
    void command(double status_code, double request_mps2) noexcept {
        request_mps2_ = request_mps2;
        stim_.set(ea::plant::can::acc_command::kAccStatus, status_code);
        stim_.set(ea::plant::can::acc_command::kAccelRequest, request_mps2 > 0.0 ? request_mps2 : 0.0);
        stim_.set(ea::plant::can::acc_command::kBrakeRequest, request_mps2 < 0.0 ? -request_mps2 : 0.0);
    }
    void command_both(double accel_mps2, double brake_mps2) noexcept {
        stim_.set(ea::plant::can::acc_command::kAccStatus, static_cast<double>(ea::plant::AccStatus::kActive));
        stim_.set(ea::plant::can::acc_command::kAccelRequest, accel_mps2);
        stim_.set(ea::plant::can::acc_command::kBrakeRequest, brake_mps2);
    }
    // Moves the net request to `target_mps2` in 0.01 m/s2 steps of at most jerk * 20 ms per cycle.
    void ramp_request(double status_code, double target_mps2, double jerk_mps3) noexcept {
        const std::int32_t step = ea::plant::Plant::jerk_step_raw(jerk_mps3);
        std::int32_t raw = ea::plant::Plant::accel_raw(request_mps2_);
        const std::int32_t goal = ea::plant::Plant::accel_raw(target_mps2);
        while (raw != goal) {
            const std::int32_t d = goal - raw;
            raw += (d > step) ? step : ((d < -step) ? -step : d);
            command(status_code, static_cast<double>(raw) * 0.01);
            run(ea::plant::Plant::kTickS);
        }
    }

private:
    double request_mps2_ = 0.0;

public:

private:
    AccMonitor ctrl_;
    ea::plant::Plant plant_;
    ea::plant::AccActuator observer_;
    ea::plant::AccCommandStub stim_;
};

}  // namespace adas::acc_monitor
