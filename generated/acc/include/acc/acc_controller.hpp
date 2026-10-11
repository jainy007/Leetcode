// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc (pattern: can_function)
// Inputs:    sha256:e3f0b69ab4e6da8dcf4e68125db331147578831a646589377e6c96cffacf9544
// Regenerate: python3 -m tools.gen

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "ea/core/span.hpp"
#include "ea/e2e/p11.hpp"
#include "acc/acc_controller_codec.hpp"
#include "acc/acc_controller_logic.hpp"
#include "acc/acc_controller_types.hpp"

namespace adas::acc {

// AccController: CAN-in / CAN-out vehicle function 'acc' (ASIL B, profile asil).
// Call on_frame() for every received frame and step() once per 20 ms cycle:
//   1. receive: E2E P11 check + decode of the newest frame per message, timeout supervision -> Inputs
//   2. logic:   AccControllerLogic (feature 'acc') maps Inputs to Outputs
//   3. send:    encode and E2E-protect the tx messages
class AccController {
public:
    static constexpr std::size_t kTxCount = 1U;

    AccController() noexcept;

    // Stores the newest frame per message; unknown ids and oversized payloads are ignored.
    void on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept;
    void step() noexcept;

    std::uint32_t tx_can_id(std::size_t i) const noexcept;
    ea::ConstByteSpan tx_frame(std::size_t i) const noexcept;
    const Inputs& inputs() const noexcept { return in_; }
    const AccControllerLogic& logic() const noexcept { return logic_; }

private:
    struct Mailbox {
        std::array<std::uint8_t, 8> data{};
        std::size_t length = 0U;
        bool fresh = false;
    };

    void receive() noexcept;
    void send() noexcept;

    // RadarObject: timeout 6 cycles
    Mailbox mb_radar_object_{};
    ea::e2e::P11Checker e2e_radar_object_;
    RadarObject rx_radar_object_{};
    std::uint16_t age_radar_object_;
    // EgoState: timeout 3 cycles
    Mailbox mb_ego_state_{};
    ea::e2e::P11Checker e2e_ego_state_;
    EgoState rx_ego_state_{};
    std::uint16_t age_ego_state_;
    // DriverInput: timeout 3 cycles
    Mailbox mb_driver_input_{};
    ea::e2e::P11Checker e2e_driver_input_;
    DriverInput rx_driver_input_{};
    std::uint16_t age_driver_input_;
    ea::e2e::P11Protector e2e_acc_command_;
    std::array<std::uint8_t, kAccCommandPayloadLength> tx_acc_command_{};
    Inputs in_{};
    Outputs out_{};
    AccControllerLogic logic_;
};

}  // namespace adas::acc
