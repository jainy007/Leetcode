// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb (pattern: can_function)
// Inputs:    sha256:a08ca97b0dae15cd9bd7ef48fa8166ab2ed0041549faf134197c8d1be4271543
// Regenerate: python3 -m tools.gen

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "ea/core/span.hpp"
#include "ea/e2e/p11.hpp"
#include "aeb/aeb_controller_codec.hpp"
#include "aeb/aeb_controller_logic.hpp"
#include "aeb/aeb_controller_types.hpp"

namespace adas::aeb {

// AebController: CAN-in / CAN-out vehicle function 'aeb' (ASIL B, profile asil).
// Call on_frame() for every received frame and step() once per 20 ms cycle:
//   1. receive: E2E P11 check + decode of the newest frame per message, timeout supervision -> Inputs
//   2. logic:   AebControllerLogic (feature 'aeb') maps Inputs to Outputs
//   3. send:    encode and E2E-protect the tx messages
class AebController {
public:
    static constexpr std::size_t kTxCount = 1U;

    AebController() noexcept;

    // Stores the newest frame per message; unknown ids and oversized payloads are ignored.
    void on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept;
    void step() noexcept;

    std::uint32_t tx_can_id(std::size_t i) const noexcept;
    ea::ConstByteSpan tx_frame(std::size_t i) const noexcept;
    const Inputs& inputs() const noexcept { return in_; }
    const AebControllerLogic& logic() const noexcept { return logic_; }

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
    ea::e2e::P11Protector e2e_aeb_command_;
    std::array<std::uint8_t, kAebCommandPayloadLength> tx_aeb_command_{};
    Inputs in_{};
    Outputs out_{};
    AebControllerLogic logic_;
};

}  // namespace adas::aeb
