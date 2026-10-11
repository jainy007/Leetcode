#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>

#include "ea/osal/realtime.hpp"

namespace ea::osal {

Result<void> lock_memory() noexcept {
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
        return ErrorCode::kOsError;
    }
    return ok_result();
}

void prefault_stack(std::size_t bytes) noexcept {
    constexpr std::size_t kMaxPrefault = std::size_t{512U} * 1024U;
    constexpr std::size_t kChunk = 4096U;
    volatile unsigned char page[kChunk];
    const std::size_t todo = bytes < kMaxPrefault ? bytes : kMaxPrefault;
    for (std::size_t i = 0U; i < kChunk; i += 64U) {
        page[i] = 0U;
    }
    static_cast<void>(page[0]);  // volatile read keeps the stores
    if (todo > kChunk) {
        prefault_stack(todo - kChunk);
    }
}

Result<void> set_current_thread_fifo(int priority) noexcept {
    if (priority == 0) {
        return ok_result();
    }
    const int min = sched_get_priority_min(SCHED_FIFO);
    const int max = sched_get_priority_max(SCHED_FIFO);
    if (priority < min || priority > max) {
        return ErrorCode::kInvalidArgument;
    }
    sched_param sp{};
    sp.sched_priority = priority;
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp) != 0) {
        return ErrorCode::kOsError;
    }
    return ok_result();
}

}  // namespace ea::osal
