// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb_monitor (pattern: can_function)
// Inputs:    sha256:4edac0782e43a3effd02948398696bdcee44fdc1992b83dad1769ef283390f0c
// Regenerate: python3 -m tools.gen

#pragma once

#include "ea/plant/plant.hpp"
#include "ea/plant/monitored_actuator.hpp"
#include "ea/plant/command_stub.hpp"
#include "aeb_monitor/aeb_monitor.hpp"

namespace adas::aeb_monitor {

// Closed-loop SIL harness: the generated function on the plant's CAN bus. The plant simulates
// vehicles, radar, driver and bus; the feature observer decodes the function's output frames.
class AebMonitorSil final : public ea::plant::ControllerPort {
public:
    static constexpr double kDriverActionS = 0.2;  // time allowed for each driver action

    void on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept override { ctrl_.on_frame(can_id, payload); }
    void step() noexcept override { ctrl_.step(); }
    std::size_t tx_count() const noexcept override { return AebMonitor::kTxCount; }
    std::uint32_t tx_can_id(std::size_t i) const noexcept override { return ctrl_.tx_can_id(i); }
    ea::ConstByteSpan tx_frame(std::size_t i) const noexcept override { return ctrl_.tx_frame(i); }

    // The stimulus node stands in for the function's command source and steps first.
    void run(double seconds) noexcept {
        ea::plant::ControllerPort* const nodes[] = {&stim_, this};
        ea::plant::FeatureObserver* const observers[] = {&observer_};
        plant_.run(ea::Span<ea::plant::ControllerPort* const>(nodes), ea::Span<ea::plant::FeatureObserver* const>(observers),
                   0U, seconds);
    }
    ea::plant::AebCommandStub& stimulus() noexcept { return stim_; }
    ea::plant::Plant& plant() noexcept { return plant_; }
    ea::plant::AebActuator& feature() noexcept { return observer_; }
    const AebMonitor& controller() const noexcept { return ctrl_; }

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
    // ---- feature 'aeb_monitor' command sequences (the stimulus stands in for the controller) ----
    ea::plant::MonitoredActuator& actuator() noexcept { return observer_; }
    // Faults on the next n frames of the stimulus (controller) or of the monitor under test.
    void command_fault(ea::plant::NodeFault kind, std::uint32_t n) noexcept {
        plant_.inject_node_fault(ea::plant::can::kAebCommandId, kind, n, ea::plant::can::kAebCommandE2e);
    }
    void monitor_fault(ea::plant::NodeFault kind, std::uint32_t n) noexcept {
        plant_.inject_node_fault(ea::plant::can::kAebMonitorId, kind, n, ea::plant::can::kAebMonitorE2e);
    }
    void command(double status_code, double brake_mps2) noexcept {
        stim_.set(ea::plant::can::aeb_command::kAebStatus, status_code);
        stim_.set(ea::plant::can::aeb_command::kAebBrakeRequest, brake_mps2);
    }

private:
    AebMonitor ctrl_;
    ea::plant::Plant plant_;
    ea::plant::AebActuator observer_;
    ea::plant::AebCommandStub stim_;
};

}  // namespace adas::aeb_monitor
