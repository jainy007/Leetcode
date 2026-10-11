// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc_monitor (pattern: can_function)
// Inputs:    sha256:7aee4feecefbc18e2ad7476e4d93efffab091cab221dd8b1b9699ac5d31b6524
// Regenerate: python3 -m tools.gen

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "ea/core/span.hpp"
#include "ea/e2e/p11.hpp"
#include "acc_monitor/acc_monitor_codec.hpp"
#include "acc_monitor/acc_monitor_logic.hpp"
#include "acc_monitor/acc_monitor_types.hpp"

namespace adas::acc_monitor {

// AccMonitor: CAN-in / CAN-out vehicle function 'acc_monitor' (ASIL B(D), profile asil).
// Call on_frame() for every received frame and step() once per 20 ms cycle:
//   1. receive: E2E P11 check + decode of the newest frame per message, timeout supervision -> Inputs
//   2. logic:   AccMonitorLogic (feature 'acc_monitor') maps Inputs to Outputs
//   3. send:    encode and E2E-protect the tx messages
class AccMonitor {
public:
    static constexpr std::size_t kTxCount = 1U;

    AccMonitor() noexcept;

    // Stores the newest frame per message; unknown ids and oversized payloads are ignored.
    void on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept;
    void step() noexcept;

    std::uint32_t tx_can_id(std::size_t i) const noexcept;
    ea::ConstByteSpan tx_frame(std::size_t i) const noexcept;
    const Inputs& inputs() const noexcept { return in_; }
    const AccMonitorLogic& logic() const noexcept { return logic_; }

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
    // AccCommand: timeout 3 cycles
    Mailbox mb_acc_command_{};
    ea::e2e::P11Checker e2e_acc_command_;
    AccCommand rx_acc_command_{};
    std::uint16_t age_acc_command_;
    ea::e2e::P11Protector e2e_acc_monitor_status_;
    std::array<std::uint8_t, kAccMonitorStatusPayloadLength> tx_acc_monitor_status_{};
    Inputs in_{};
    Outputs out_{};
    AccMonitorLogic logic_;
};

}  // namespace adas::acc_monitor
