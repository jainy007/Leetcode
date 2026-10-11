// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc (pattern: acc_controller)
// Inputs:    sha256:cce2b25a21d5b0053b9588be55a9c9be1a00be09e870c4dedb9dbbf5638be09e
// Regenerate: python3 -m tools.gen

#pragma once

#include "ea/plant/acc_plant.hpp"
#include "acc/acc_controller.hpp"

namespace adas::acc {

// Closed-loop SIL harness: the generated controller on the plant's CAN bus.
class AccControllerSil final : public ea::plant::ControllerPort {
public:
    static constexpr double kDriverActionS = 0.2;  // time allowed for each driver action

    void on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept override { ctrl_.on_frame(can_id, payload); }
    void step() noexcept override { ctrl_.step(); }
    ea::ConstByteSpan command_frame() const noexcept override { return ctrl_.command_frame(); }

    void run(double seconds) noexcept { plant_.run(*this, seconds); }
    ea::plant::AccPlant& plant() noexcept { return plant_; }
    const AccController& controller() const noexcept { return ctrl_; }

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
    void engage() noexcept {
        switch_main(true);
        press(ea::plant::Button::kSet, 1);
    }
    void brake(bool pressed) noexcept {
        plant_.set_brake_pedal(pressed);
        run(kDriverActionS);
    }
    // Press Set with the brake applied, then release the brake while Set is still held.
    void release_brake_while_holding_set() noexcept {
        plant_.press(ea::plant::Button::kSet);  // held for 100 ms
        run(0.04);
        plant_.set_brake_pedal(false);
        run(kDriverActionS);
    }
    void accelerator(double pct) noexcept {
        plant_.set_accel_pedal(pct);
        run(kDriverActionS);
    }

private:
    AccController ctrl_;
    ea::plant::AccPlant plant_;
};

}  // namespace adas::acc
