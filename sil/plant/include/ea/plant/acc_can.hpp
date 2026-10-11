// Hand-written layout of the ACC output message AccCommand (examples/acc/interfaces/acc.dbc).
#pragma once

#include "ea/plant/vehicle_can.hpp"

namespace ea::plant::can {

inline constexpr std::uint32_t kAccCommandId = 768U;
inline constexpr e2e::P11Config kAccCommandE2e{0x0300U, 0U, 8U, 8U, 2U};

namespace acc_command {
inline constexpr Sig kAccStatus{12U, 3U, false, 1.0};
inline constexpr Sig kAccelRequest{16U, 10U, false, 0.01};
inline constexpr Sig kBrakeRequest{26U, 10U, false, 0.01};
inline constexpr Sig kSteeringAngleRequest{36U, 12U, true, 0.1};
inline constexpr Sig kSetSpeed{48U, 8U, false, 1.0};
inline constexpr Sig kTimeGapSetting{56U, 2U, false, 1.0};
}  // namespace acc_command

}  // namespace ea::plant::can
