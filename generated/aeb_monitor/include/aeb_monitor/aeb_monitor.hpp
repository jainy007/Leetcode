// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb_monitor (pattern: can_function)
// Inputs:    sha256:4c2c50398135b7072ac31e6b5ee98281a3ac11962a726a5375f5b4a9eb38232c
// Regenerate: python3 -m tools.gen

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "ea/core/span.hpp"
#include "ea/e2e/p11.hpp"
#include "aeb_monitor/aeb_monitor_codec.hpp"
#include "aeb_monitor/aeb_monitor_logic.hpp"
#include "aeb_monitor/aeb_monitor_types.hpp"

namespace adas::aeb_monitor {

// AebMonitor: CAN-in / CAN-out vehicle function 'aeb_monitor' (ASIL B(D), profile asil).
// Call on_frame() for every received frame and step() once per 20 ms cycle:
//   1. receive: E2E P11 check + decode of the newest frame per message, timeout supervision -> Inputs
//   2. logic:   AebMonitorLogic (feature 'aeb_monitor') maps Inputs to Outputs
//   3. send:    encode and E2E-protect the tx messages
class AebMonitor {
public:
    static constexpr std::size_t kTxCount = 1U;

    AebMonitor() noexcept;

    // Stores the newest frame per message; unknown ids and oversized payloads are ignored.
    void on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept;
    void step() noexcept;

    std::uint32_t tx_can_id(std::size_t i) const noexcept;
    ea::ConstByteSpan tx_frame(std::size_t i) const noexcept;
    const Inputs& inputs() const noexcept { return in_; }
    const AebMonitorLogic& logic() const noexcept { return logic_; }

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
    // AebCommand: timeout 3 cycles
    Mailbox mb_aeb_command_{};
    ea::e2e::P11Checker e2e_aeb_command_;
    AebCommand rx_aeb_command_{};
    std::uint16_t age_aeb_command_;
    ea::e2e::P11Protector e2e_aeb_monitor_status_;
    std::array<std::uint8_t, kAebMonitorStatusPayloadLength> tx_aeb_monitor_status_{};
    Inputs in_{};
    Outputs out_{};
    AebMonitorLogic logic_;
};

}  // namespace adas::aeb_monitor
