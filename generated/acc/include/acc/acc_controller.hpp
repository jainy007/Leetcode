// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc (pattern: acc_controller)
// Inputs:    sha256:cce2b25a21d5b0053b9588be55a9c9be1a00be09e870c4dedb9dbbf5638be09e
// Regenerate: python3 -m tools.gen

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "ea/core/span.hpp"
#include "ea/e2e/p11.hpp"
#include "acc/acc_controller_codec.hpp"
#include "acc/acc_controller_types.hpp"

namespace adas::acc {

// AccController: CAN-in / CAN-out adaptive cruise control (ASIL B, profile asil).
// Call on_frame() for every received frame and step() once per 20 ms cycle:
//   1. receive: E2E P11 check + decode of the newest frame per message, timeout supervision
//   2. modes:   OFF / STANDBY / ACTIVE / OVERRIDE / FAULT from driver inputs and input health
//   3. control: speed and time-gap control, acceleration, deceleration and jerk limits
//   4. send:    encode and E2E-protect AccCommand
class AccController {
public:
    // Calibration (from the model).
    static constexpr double kMinActivationSpeedKph = 30.0;
    static constexpr double kDisengageSpeedKph = 25.0;
    static constexpr double kMaxSetSpeedKph = 180.0;
    static constexpr double kSetSpeedStepKph = 5.0;
    static constexpr std::size_t kGapSettings = 3U;
    static constexpr std::array<double, kGapSettings> kTimeGapsS{1.0, 1.5, 2.0};
    static constexpr double kStandstillDistanceM = 5.0;
    static constexpr double kMaxTargetRangeM = 200.0;
    static constexpr double kLaneHalfWidthM = 1.8;
    static constexpr double kAccelMaxMps2 = 2.0;
    static constexpr double kDecelMaxMps2 = 3.5;
    static constexpr double kOverridePedalPct = 5.0;
    static constexpr double kSpeedGainPerS = 0.3;
    static constexpr double kGapGainPerS2 = 0.2;
    static constexpr double kRangeRateGainPerS = 0.6;
    // Acceleration is handled in integer steps of the command resolution so the transmitted
    // values honour the jerk limits exactly.
    static constexpr double kAccelResolution = 0.01;
    static constexpr std::int32_t kJerkUpStep = 5;    // per cycle
    static constexpr std::int32_t kJerkDownStep = 10;  // per cycle

    AccController() noexcept;

    // Stores the newest frame per message; unknown ids and oversized payloads are ignored.
    void on_frame(std::uint32_t can_id, ea::ConstByteSpan payload) noexcept;
    void step() noexcept;

    ea::ConstByteSpan command_frame() const noexcept { return ea::ConstByteSpan(tx_frame_.data(), tx_frame_.size()); }
    Mode mode() const noexcept { return mode_; }
    double set_speed_kph() const noexcept { return set_speed_kph_; }

private:
    struct Mailbox {
        std::array<std::uint8_t, 8> data{};
        std::size_t length = 0U;
        bool fresh = false;
    };

    void receive() noexcept;
    bool inputs_healthy() const noexcept;
    void update_mode() noexcept;
    void control() noexcept;
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
    ea::e2e::P11Protector e2e_tx_;
    std::array<std::uint8_t, kAccCommandPayloadLength> tx_frame_{};

    Mode mode_ = Mode::kOff;
    double set_speed_kph_ = 0.0;
    std::uint32_t gap_setting_ = 2U;
    std::int32_t accel_raw_ = 0;  // commanded acceleration in kAccelResolution steps (+ accel, - decel)
    bool prev_set_ = false;
    bool prev_resume_ = false;
    bool prev_cancel_ = false;
    bool prev_up_ = false;
    bool prev_down_ = false;
};

}  // namespace adas::acc
