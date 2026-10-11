// Hand-written layout of the safety monitor output messages (examples/*_monitor/interfaces/*.dbc).
// Both monitors share one signal layout so that the actuator ECU model can arbitrate any
// controller / monitor pair the same way.
#pragma once

#include "ea/plant/vehicle_can.hpp"

namespace ea::plant::can {

inline constexpr std::uint32_t kAccMonitorId = 769U;
inline constexpr e2e::P11Config kAccMonitorE2e{0x0301U, 0U, 8U, 4U, 2U};
inline constexpr std::uint32_t kAebMonitorId = 785U;
inline constexpr e2e::P11Config kAebMonitorE2e{0x0311U, 0U, 8U, 4U, 2U};

namespace monitor {
inline constexpr Sig kMonStatus{12U, 2U, false, 1.0};       // 0 INIT, 1 MONITORING, 2 SAFE_STATE
inline constexpr Sig kPermit{14U, 1U, false, 1.0};          // 1: the approved command may be applied
inline constexpr Sig kApprovedCounter{16U, 4U, false, 1.0};  // E2E counter of the approved command
inline constexpr Sig kFaultReason{20U, 4U, false, 1.0};      // first violated check, 0 = none
}  // namespace monitor

// Fault reason codes as specified in the DBC comments (not taken from generated code).
enum class AccMonitorReason : std::uint32_t {
    kNone = 0, kCommandTimeout = 1, kRequestLimit = 2, kAccelAndBrake = 3, kRequestOutsideActive = 4,
    kRequestRate = 5, kUnintendedAccel = 6,
};
enum class AebMonitorReason : std::uint32_t {
    kNone = 0, kCommandTimeout = 1, kRequestLimit = 2, kRequestOutsideBraking = 3, kUnjustifiedBraking = 4,
    kUnjustifiedFullBraking = 5,
};

}  // namespace ea::plant::can
