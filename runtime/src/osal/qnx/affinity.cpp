// QNX Neutrino: thread affinity via the runmask (QNX has no pthread_setaffinity_np).
// Only the low 32 CPUs are addressed; QNX SDP 8 targets in scope have <= 32 cores.
#include <sys/neutrino.h>

#include <cstdint>

#include "ea/osal/realtime.hpp"

namespace ea::osal {

Result<void> set_current_thread_affinity(std::uint64_t cpu_mask) noexcept {
    if (cpu_mask == 0U) {
        return ok_result();
    }
    if ((cpu_mask >> 32U) != 0U) {
        return ErrorCode::kNotSupported;
    }
    const std::uintptr_t runmask = static_cast<std::uintptr_t>(cpu_mask);
    if (ThreadCtl(_NTO_TCTL_RUNMASK, reinterpret_cast<void*>(runmask)) == -1) {  // NOLINT
        return ErrorCode::kOsError;
    }
    return ok_result();
}

}  // namespace ea::osal
