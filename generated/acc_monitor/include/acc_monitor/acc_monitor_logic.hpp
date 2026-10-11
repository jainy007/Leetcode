// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc_monitor (pattern: can_function)
// Inputs:    sha256:aaed2b82deaf8ac2890ea08cf061dfa671e7be14789da61b08e9a29f91792b3a
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstdint>

#include "acc_monitor/acc_monitor_types.hpp"

namespace adas::acc_monitor {

// Feature 'acc_monitor' logic: safety envelope of the ACC controller's longitudinal command.
// Every new command (new E2E counter) is checked; a violation latches SAFE_STATE. In
// MONITORING the permit is granted and the counter of the last checked command is echoed, so
// the actuator ECU applies exactly the commands this monitor has checked.
class AccMonitorLogic {
public:
    enum class Reason : std::uint8_t {
        kNone = 0U,
        kCommandTimeout = 1U,
        kRequestLimit = 2U,
        kAccelAndBrake = 3U,
        kRequestOutsideActive = 4U,
        kRequestRate = 5U,
        kUnintendedAccel = 6U,
    };

    static constexpr double kActiveStatusCode = 2.0;
    static constexpr double kAccelMaxMps2 = 2.0;
    static constexpr double kDecelMaxMps2 = 3.5;
    static constexpr double kRequestToleranceMps2 = 0.1;
    static constexpr double kAccelRatePerCycle = 0.07;  // (jerk + tolerance) * cycle
    static constexpr double kBrakeRatePerCycle = 0.12;
    static constexpr double kMinTtcForAccelS = 2.0;
    static constexpr double kMaxTargetRangeM = 200.0;
    static constexpr double kLaneHalfWidthM = 1.8;
    static constexpr std::uint32_t kDebounceCycles = 25U;

    void step(const Inputs& in, Outputs& out) noexcept;
    State state() const noexcept { return state_; }
    Reason reason() const noexcept { return reason_; }

private:
    void check_command(const Inputs& in) noexcept;
    void check_unintended_accel(const Inputs& in) noexcept;
    void violation(Reason r) noexcept;

    State state_ = State::kInit;
    Reason reason_ = Reason::kNone;
    double last_counter_ = -1.0;  // counter of the last checked command
    double prev_accel_ = 0.0;
    double prev_brake_ = 0.0;
    std::uint32_t cycles_since_command_ = 1U;
    std::uint32_t accel_toward_target_cycles_ = 0U;
};

}  // namespace adas::acc_monitor
