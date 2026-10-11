#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <pthread.h>
#include <sched.h>

#include "ea/osal/realtime.hpp"

namespace ea::osal {

Result<void> set_current_thread_affinity(std::uint64_t cpu_mask) noexcept {
    if (cpu_mask == 0U) {
        return ok_result();
    }
    cpu_set_t set;
    CPU_ZERO(&set);
    for (unsigned cpu = 0U; cpu < 64U; ++cpu) {
        if (((cpu_mask >> cpu) & 1U) != 0U) {
            CPU_SET(cpu, &set);
        }
    }
    if (pthread_setaffinity_np(pthread_self(), sizeof(set), &set) != 0) {
        return ErrorCode::kOsError;
    }
    return ok_result();
}

}  // namespace ea::osal
