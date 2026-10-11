// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc_system (pattern: can_system)
// Inputs:    sha256:23346b03d4046c6dcd8a3513aba427631a7d11a306ae3f61a7a6d60428b173b5
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstddef>
#include <cstdint>

#include "ea/plant/plant.hpp"
#include "ea/plant/acc_observer.hpp"
#include "ea/plant/monitored_actuator.hpp"
#include "ea/plant/acc_can.hpp"
#include "ea/plant/monitor_can.hpp"
#include "acc/acc_controller.hpp"
#include "acc_monitor/acc_monitor.hpp"

namespace adas::acc_system {

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

// System SIL of AccSystem (ASIL D): AccController (ASIL B(D)) and
// AccMonitor (ASIL B(D)) step in this order every cycle on one bus. The controller observer
// decodes the controller's command; the actuator ECU model (AoU) applies only approved commands
// and drives the vehicle. Faults are injected on the nodes' output frames with the plant's own
// message tables.
class AccSystemSil {
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
    ea::plant::AccObserver& feature() noexcept { return observer_; }
    ea::plant::AccActuator& actuator() noexcept { return actuator_; }
    const ::adas::acc::AccController& controller() const noexcept { return controller_.node(); }
    const ::adas::acc_monitor::AccMonitor& monitor() const noexcept { return monitor_.node(); }

    // Faults on the next n output frames of a node.
    void command_fault(ea::plant::NodeFault kind, std::uint32_t n) noexcept {
        plant_.inject_node_fault(ea::plant::can::kAccCommandId, kind, n, ea::plant::can::kAccCommandE2e);
    }
    void monitor_fault(ea::plant::NodeFault kind, std::uint32_t n) noexcept {
        plant_.inject_node_fault(ea::plant::can::kAccMonitorId, kind, n, ea::plant::can::kAccMonitorE2e);
    }
    // A wrong value of one command signal behind a valid CRC (a logic fault in the controller).
    void override_command(const ea::plant::can::Sig& sig, double value, std::uint32_t n) noexcept {
        plant_.inject_node_fault(ea::plant::can::kAccCommandId, ea::plant::NodeFault::kOverride, n, ea::plant::can::kAccCommandE2e,
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

    // ---- feature 'acc' driver sequences ----
    void engage() noexcept {
        switch_main(true);
        press(ea::plant::Button::kSet, 1);
    }
    // Press Set with the brake applied, then release the brake while Set is still held.
    void release_brake_while_holding_set() noexcept {
        plant_.press(ea::plant::Button::kSet);  // held for 100 ms
        run(0.04);
        plant_.set_brake_pedal(false);
        run(kDriverActionS);
    }

private:
    NodePort<::adas::acc::AccController> controller_;
    NodePort<::adas::acc_monitor::AccMonitor> monitor_;
    ea::plant::Plant plant_;
    ea::plant::AccObserver observer_;
    ea::plant::AccActuator actuator_;
};

}  // namespace adas::acc_system
