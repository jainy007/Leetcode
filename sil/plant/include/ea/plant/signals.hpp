// Physical <-> raw helpers on the plant's hand-written signal tables.
#pragma once

#include <array>
#include <cstdint>

#include "ea/core/span.hpp"
#include "ea/plant/vehicle_can.hpp"

namespace ea::plant {

// Physical -> raw with round-half-away and saturation to the signal's raw range.
void put(std::array<std::uint8_t, 8>& frame, const can::Sig& s, double physical) noexcept;
std::int64_t get_raw(ConstByteSpan frame, const can::Sig& s) noexcept;
inline double get(ConstByteSpan frame, const can::Sig& s) noexcept {
    return static_cast<double>(get_raw(frame, s)) * s.factor;
}

}  // namespace ea::plant
