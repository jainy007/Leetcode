// Time source abstraction. Only monotonic time is used for control and supervision
// (rule CPU-008: never CLOCK_REALTIME for timing). SIL substitutes a virtual clock.
#pragma once

#include <cstdint>

namespace ea::osal {

using Nanoseconds = std::int64_t;

inline constexpr Nanoseconds kNsPerUs = 1000;
inline constexpr Nanoseconds kNsPerMs = 1000 * 1000;
inline constexpr Nanoseconds kNsPerSec = 1000 * 1000 * 1000;

class IClock {
public:
    virtual ~IClock() = default;
    virtual Nanoseconds now() const noexcept = 0;

protected:
    IClock() = default;
    IClock(const IClock&) = default;
    IClock& operator=(const IClock&) = default;
};

class MonotonicClock final : public IClock {
public:
    Nanoseconds now() const noexcept override;
};

}  // namespace ea::osal
