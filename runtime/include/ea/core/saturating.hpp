// Saturating arithmetic for counters and debouncers (rule CPU-010: no silent wrap-around).
#pragma once

#include <limits>
#include <type_traits>

namespace ea {

template <typename T>
constexpr T sat_inc(T v) noexcept {
    static_assert(std::is_unsigned<T>::value, "sat_inc requires an unsigned type");
    return (v == std::numeric_limits<T>::max()) ? v : static_cast<T>(v + 1U);
}

template <typename T>
constexpr T sat_add(T a, T b) noexcept {
    static_assert(std::is_unsigned<T>::value, "sat_add requires an unsigned type");
    return (a > static_cast<T>(std::numeric_limits<T>::max() - b)) ? std::numeric_limits<T>::max()
                                                                    : static_cast<T>(a + b);
}

template <typename T>
constexpr T sat_sub(T a, T b) noexcept {
    static_assert(std::is_unsigned<T>::value, "sat_sub requires an unsigned type");
    return (a < b) ? T{0} : static_cast<T>(a - b);
}

}  // namespace ea
