// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc (pattern: can_function)
// Inputs:    sha256:7daf72a7ff67fdde3af69c03ce9ff58b4ecdb3b7244d701c28de0793bc835631
// Regenerate: python3 -m tools.gen

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "acc/acc_controller_types.hpp"

namespace adas::acc {

// Feature 'acc' logic: driver mode machine, speed and time-gap control with acceleration,
// deceleration and jerk limits. Pure function of Inputs and its own state; no I/O.
class AccControllerLogic {
public:
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
    // Acceleration in integer steps of the command resolution: transmitted values honour the
    // jerk limits exactly.
    static constexpr double kAccelResolution = 0.01;
    static constexpr std::int32_t kJerkUpStep = 5;    // per cycle
    static constexpr std::int32_t kJerkDownStep = 10;  // per cycle

    void step(const Inputs& in, Outputs& out) noexcept;

    State state() const noexcept { return state_; }
    double set_speed_kph() const noexcept { return set_speed_kph_; }

private:
    void update_mode(const Inputs& in) noexcept;
    void control(const Inputs& in) noexcept;

    State state_ = State::kOff;
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
