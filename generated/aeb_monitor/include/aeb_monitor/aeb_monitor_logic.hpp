// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb_monitor (pattern: can_function)
// Inputs:    sha256:4c2c50398135b7072ac31e6b5ee98281a3ac11962a726a5375f5b4a9eb38232c
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstdint>

#include "aeb_monitor/aeb_monitor_types.hpp"

namespace adas::aeb_monitor {

// Feature 'aeb_monitor' logic: justification of the AEB controller's braking requests from an
// independent evaluation of the radar target. Every new command (new E2E counter) is checked;
// a violation latches SAFE_STATE. In MONITORING the permit is granted and the counter of the
// last checked command is echoed for the actuator ECU.
class AebMonitorLogic {
public:
    enum class Reason : std::uint8_t {
        kNone = 0U,
        kCommandTimeout = 1U,
        kRequestLimit = 2U,
        kRequestOutsideBraking = 3U,
        kUnjustifiedBraking = 4U,
        kUnjustifiedFullBraking = 5U,
    };

    static constexpr double kPartialStatusCode = 2.0;
    static constexpr double kFullStatusCode = 3.0;
    static constexpr double kPartialDecelMps2 = 4.0;
    static constexpr double kFullDecelMps2 = 8.0;
    static constexpr double kRequestToleranceMps2 = 0.1;
    static constexpr double kPartialBrakeTtcS = 1.8;
    static constexpr double kFullBrakeTtcS = 1.2;
    static constexpr double kTtcMarginS = 0.3;
    static constexpr double kMaxTargetRangeM = 150.0;
    static constexpr double kLaneHalfWidthM = 1.8;
    static constexpr double kLaneMarginM = 0.3;

    void step(const Inputs& in, Outputs& out) noexcept;
    State state() const noexcept { return state_; }
    Reason reason() const noexcept { return reason_; }

private:
    void check_command(const Inputs& in) noexcept;
    void violation(Reason r) noexcept;

    State state_ = State::kInit;
    Reason reason_ = Reason::kNone;
    double last_counter_ = -1.0;  // counter of the last checked command
    bool braking_justified_ = false;
    bool full_braking_justified_ = false;
};

}  // namespace adas::aeb_monitor
