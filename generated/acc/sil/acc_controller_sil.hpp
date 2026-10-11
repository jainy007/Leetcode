// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc (pattern: can_function)
// Inputs:    sha256:e3f0b69ab4e6da8dcf4e68125db331147578831a646589377e6c96cffacf9544
// Regenerate: python3 -m tools.gen

#pragma once

#include "ea/plant/plant.hpp"
#include "ea/plant/acc_observer.hpp"
#include "acc/acc_controller.hpp"

namespace adas::acc {

// Closed-loop SIL harness: the generated function on the plant's CAN bus. The plant simulates
// vehicles, radar, driver and bus; the feature observer decodes the function's output frames.
class AccControllerSil final : public ea::plant::ControllerPort {
public:
    static constexpr double kDriverActionS = 0.2;  // time allowed for each driver action

    void on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept override { ctrl_.on_frame(can_id, payload); }
    void step() noexcept override { ctrl_.step(); }
    std::size_t tx_count() const noexcept override { return AccController::kTxCount; }
    std::uint32_t tx_can_id(std::size_t i) const noexcept override { return ctrl_.tx_can_id(i); }
    ea::ConstByteSpan tx_frame(std::size_t i) const noexcept override { return ctrl_.tx_frame(i); }

    void run(double seconds) noexcept { plant_.run(*this, observer_, seconds); }
    ea::plant::Plant& plant() noexcept { return plant_; }
    ea::plant::AccObserver& feature() noexcept { return observer_; }
    const AccController& controller() const noexcept { return ctrl_; }

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
    AccController ctrl_;
    ea::plant::Plant plant_;
    ea::plant::AccObserver observer_;
};

}  // namespace adas::acc
