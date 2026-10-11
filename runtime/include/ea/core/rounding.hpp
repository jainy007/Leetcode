// Exact, inline replacement for std::llround (round half away from zero) on the bounded ranges
// used by codecs. std::llround is a libm call; this compiles to a truncating conversion plus a
// compare. For |x| < 2^52, x - trunc(x) is computed exactly, so the result equals std::llround.
#pragma once

#include <cstdint>

#include "ea/core/assert.hpp"

namespace ea {

inline constexpr double kRoundExactLimit = 4503599627370496.0;  // 2^52

// Precondition: |x| < 2^52 (callers range-check first; codec ranges are far smaller).
inline std::int64_t round_half_away(double x) noexcept {
    const std::int64_t t = static_cast<std::int64_t>(x);  // truncates toward zero
    const double frac = x - static_cast<double>(t);
    if (frac >= 0.5) {
        return t + 1;
    }
    if (frac <= -0.5) {
        return t - 1;
    }
    return t;
}

}  // namespace ea
