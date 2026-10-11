// Hand-written layout of the AEB output message AebCommand (examples/aeb/interfaces/aeb.dbc).
#pragma once

#include "ea/plant/vehicle_can.hpp"

namespace ea::plant::can {

inline constexpr std::uint32_t kAebCommandId = 784U;
inline constexpr e2e::P11Config kAebCommandE2e{0x0310U, 0U, 8U, 8U, 2U};

namespace aeb_command {
inline constexpr Sig kAebStatus{12U, 3U, false, 1.0};
inline constexpr Sig kFcwWarning{15U, 1U, false, 1.0};
inline constexpr Sig kAebBrakeRequest{16U, 10U, false, 0.01};
}  // namespace aeb_command

}  // namespace ea::plant::can
