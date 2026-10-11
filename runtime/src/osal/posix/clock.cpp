#include <time.h>

#include "ea/osal/clock.hpp"

namespace ea::osal {

Nanoseconds MonotonicClock::now() const noexcept {
    timespec ts{};
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<Nanoseconds>(ts.tv_sec) * kNsPerSec + static_cast<Nanoseconds>(ts.tv_nsec);
}

}  // namespace ea::osal
