// Hand-written CAN layout of the shared vehicle bus (RadarObject, EgoState, DriverInput), as in
// examples/*/interfaces/*.dbc. Deliberately NOT derived from generated code: a codec bug in a
// generated function shows up as a disagreement with this table (independent test oracle).
// Feature output messages have their own tables (acc_can.hpp, aeb_can.hpp, ...).
#pragma once

#include <cstdint>

#include "ea/codec/bits.hpp"
#include "ea/e2e/p11.hpp"

namespace ea::plant::can {

struct Sig {
    std::uint32_t start;
    std::uint32_t length;
    bool is_signed;
    double factor;
};

inline constexpr codec::ByteOrder kOrder = codec::ByteOrder::kLittleEndian;  // all signals are Intel

inline constexpr std::uint32_t kRadarObjectId = 512U;
inline constexpr std::uint32_t kEgoStateId = 528U;
inline constexpr std::uint32_t kDriverInputId = 544U;

// Every message: CRC byte 0, counter low nibble of byte 1, 8 bytes. The max delta is the
// plant's own check tolerance for frames it receives (irrelevant for frames it sends).
inline constexpr e2e::P11Config kRadarE2e{0x0200U, 0U, 8U, 8U, 2U};
inline constexpr e2e::P11Config kEgoE2e{0x0210U, 0U, 8U, 8U, 2U};
inline constexpr e2e::P11Config kDriverE2e{0x0220U, 0U, 8U, 8U, 2U};

namespace radar {
inline constexpr Sig kObjValid{12U, 1U, false, 1.0};
inline constexpr Sig kObjRange{16U, 12U, false, 0.1};
inline constexpr Sig kObjRangeRate{28U, 12U, true, 0.05};
inline constexpr Sig kObjLateralPos{40U, 10U, true, 0.02};
}  // namespace radar

namespace ego {
inline constexpr Sig kVehicleSpeed{16U, 16U, false, 0.01};
}  // namespace ego

namespace driver {
inline constexpr Sig kMainSwitch{12U, 1U, false, 1.0};
inline constexpr Sig kSetButton{13U, 1U, false, 1.0};
inline constexpr Sig kResumeButton{14U, 1U, false, 1.0};
inline constexpr Sig kCancelButton{15U, 1U, false, 1.0};
inline constexpr Sig kSpeedUpButton{16U, 1U, false, 1.0};
inline constexpr Sig kSpeedDownButton{17U, 1U, false, 1.0};
inline constexpr Sig kGapSetting{18U, 2U, false, 1.0};
inline constexpr Sig kBrakePedal{20U, 1U, false, 1.0};
inline constexpr Sig kAccelPedalPos{24U, 8U, false, 0.5};
}  // namespace driver


}  // namespace ea::plant::can
