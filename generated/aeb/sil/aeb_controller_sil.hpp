// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb (pattern: can_function)
// Inputs:    sha256:b615ac7a70c66498145a1cce61803118d779529b0bce381de5d78fc6a4940003
// Regenerate: python3 -m tools.gen

#pragma once

#include "ea/plant/plant.hpp"
#include "ea/plant/aeb_observer.hpp"
#include "aeb/aeb_controller.hpp"

namespace adas::aeb {

// Closed-loop SIL harness: the generated function on the plant's CAN bus. The plant simulates
// vehicles, radar, driver and bus; the feature observer decodes the function's output frames.
class AebControllerSil final : public ea::plant::ControllerPort {
public:
    static constexpr double kDriverActionS = 0.2;  // time allowed for each driver action

    void on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept override { ctrl_.on_frame(can_id, payload); }
    void step() noexcept override { ctrl_.step(); }
    std::size_t tx_count() const noexcept override { return AebController::kTxCount; }
    std::uint32_t tx_can_id(std::size_t i) const noexcept override { return ctrl_.tx_can_id(i); }
    ea::ConstByteSpan tx_frame(std::size_t i) const noexcept override { return ctrl_.tx_frame(i); }

    void run(double seconds) noexcept { plant_.run(*this, observer_, seconds); }
    ea::plant::Plant& plant() noexcept { return plant_; }
    ea::plant::AebObserver& feature() noexcept { return observer_; }
    const AebController& controller() const noexcept { return ctrl_; }

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

private:
    AebController ctrl_;
    ea::plant::Plant plant_;
    ea::plant::AebObserver observer_;
};

}  // namespace adas::aeb
