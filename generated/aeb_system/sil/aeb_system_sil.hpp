// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb_system (pattern: can_system)
// Inputs:    sha256:32e090c28d3d514ed5a60fa7ce31d19eb981ea3a5a9d3b6fd2c098d99209902a
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstddef>
#include <cstdint>

#include "ea/plant/plant.hpp"
#include "ea/plant/aeb_observer.hpp"
#include "ea/plant/monitored_actuator.hpp"
#include "ea/plant/aeb_can.hpp"
#include "ea/plant/monitor_can.hpp"
#include "aeb/aeb_controller.hpp"
#include "aeb_monitor/aeb_monitor.hpp"

namespace adas::aeb_system {

// A generated node on the plant's bus.
template <typename Node>
class NodePort final : public ea::plant::ControllerPort {
public:
    void on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept override { node_.on_frame(can_id, payload); }
    void step() noexcept override { node_.step(); }
    std::size_t tx_count() const noexcept override { return Node::kTxCount; }
    std::uint32_t tx_can_id(std::size_t i) const noexcept override { return node_.tx_can_id(i); }
    ea::ConstByteSpan tx_frame(std::size_t i) const noexcept override { return node_.tx_frame(i); }
    const Node& node() const noexcept { return node_; }

private:
    Node node_;
};

// System SIL of AebSystem (ASIL D): AebController (ASIL B(D)) and
// AebMonitor (ASIL B(D)) step in this order every cycle on one bus. The controller observer
// decodes the controller's command; the actuator ECU model (AoU) applies only approved commands
// and drives the vehicle. Faults are injected on the nodes' output frames with the plant's own
// message tables.
class AebSystemSil {
public:
    static constexpr double kDriverActionS = 0.2;
    static constexpr double kActuatorReactionMs = 100.0;  // FRTI assumed for the actuator ECU

    void run(double seconds) noexcept {
        ea::plant::ControllerPort* const nodes[] = {&controller_, &monitor_};
        ea::plant::FeatureObserver* const observers[] = {&observer_, &actuator_};
        plant_.run(ea::Span<ea::plant::ControllerPort* const>(nodes), ea::Span<ea::plant::FeatureObserver* const>(observers),
                   1U, seconds);
    }
    ea::plant::Plant& plant() noexcept { return plant_; }
    ea::plant::AebObserver& feature() noexcept { return observer_; }
    ea::plant::AebActuator& actuator() noexcept { return actuator_; }
    const ::adas::aeb::AebController& controller() const noexcept { return controller_.node(); }
    const ::adas::aeb_monitor::AebMonitor& monitor() const noexcept { return monitor_.node(); }

    // Faults on the next n output frames of a node.
    void command_fault(ea::plant::NodeFault kind, std::uint32_t n) noexcept {
        plant_.inject_node_fault(ea::plant::can::kAebCommandId, kind, n, ea::plant::can::kAebCommandE2e);
    }
    void monitor_fault(ea::plant::NodeFault kind, std::uint32_t n) noexcept {
        plant_.inject_node_fault(ea::plant::can::kAebMonitorId, kind, n, ea::plant::can::kAebMonitorE2e);
    }
    // A wrong value of one command signal behind a valid CRC (a logic fault in the controller).
    void override_command(const ea::plant::can::Sig& sig, double value, std::uint32_t n) noexcept {
        plant_.inject_node_fault(ea::plant::can::kAebCommandId, ea::plant::NodeFault::kOverride, n, ea::plant::can::kAebCommandE2e,
                                 sig.start, sig.length, sig.is_signed, sig.factor, value);
    }

    // Driver actions (each followed by kDriverActionS of simulation).
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

private:
    NodePort<::adas::aeb::AebController> controller_;
    NodePort<::adas::aeb_monitor::AebMonitor> monitor_;
    ea::plant::Plant plant_;
    ea::plant::AebObserver observer_;
    ea::plant::AebActuator actuator_;
};

}  // namespace adas::aeb_system
