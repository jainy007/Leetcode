// Real-time process/thread setup (rules CPU-005..CPU-007).
#pragma once

#include <cstddef>
#include <cstdint>

#include "ea/core/result.hpp"

namespace ea::osal {

// mlockall(MCL_CURRENT | MCL_FUTURE): no page faults after init.
Result<void> lock_memory() noexcept;

// Touch `bytes` of stack so the pages are resident before the cyclic phase.
void prefault_stack(std::size_t bytes) noexcept;

// SCHED_FIFO with the given priority for the calling thread. 0 leaves the policy unchanged.
Result<void> set_current_thread_fifo(int priority) noexcept;

// Pin the calling thread to the CPUs in `cpu_mask` (bit n = CPU n). 0 leaves affinity unchanged.
// Implemented per OS: Linux (pthread_setaffinity_np), QNX (ThreadCtl _NTO_TCTL_RUNMASK).
Result<void> set_current_thread_affinity(std::uint64_t cpu_mask) noexcept;

}  // namespace ea::osal
