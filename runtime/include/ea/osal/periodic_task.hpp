// Periodic task on a dedicated POSIX thread (rule CPU-006).
// - Absolute-time sleeping (clock_nanosleep + TIMER_ABSTIME) so the period does not drift.
// - Overruns are counted and the schedule is re-anchored, never "caught up" in a burst.
// - The callback is a plain function pointer + context: no heap, no std::function.
#pragma once

#include <pthread.h>

#include <atomic>
#include <cstdint>

#include "ea/core/result.hpp"
#include "ea/osal/clock.hpp"

namespace ea::osal {

struct TaskConfig {
    const char* name;           // <= 15 chars on Linux
    Nanoseconds period;         // > 0
    int fifo_priority;          // 0 = inherit scheduling policy
    std::uint64_t cpu_mask;     // 0 = no pinning
};

using TaskFn = void (*)(void* ctx);

class PeriodicTask {
public:
    PeriodicTask() = default;
    ~PeriodicTask();
    PeriodicTask(const PeriodicTask&) = delete;
    PeriodicTask& operator=(const PeriodicTask&) = delete;

    Result<void> start(const TaskConfig& cfg, TaskFn fn, void* ctx) noexcept;
    void stop() noexcept;

    bool running() const noexcept { return running_.load(std::memory_order_acquire); }
    std::uint64_t activations() const noexcept { return activations_.load(std::memory_order_relaxed); }
    std::uint64_t overruns() const noexcept { return overruns_.load(std::memory_order_relaxed); }

private:
    static void* entry(void* self) noexcept;
    void loop() noexcept;

    TaskConfig cfg_{};
    TaskFn fn_ = nullptr;
    void* ctx_ = nullptr;
    pthread_t thread_{};
    std::atomic<bool> running_{false};
    std::atomic<bool> stop_requested_{false};
    std::atomic<std::uint64_t> activations_{0U};
    std::atomic<std::uint64_t> overruns_{0U};
};

}  // namespace ea::osal
