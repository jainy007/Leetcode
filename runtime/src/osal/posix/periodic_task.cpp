#include "ea/osal/periodic_task.hpp"

#include <time.h>

#include <cstring>

#include "ea/osal/realtime.hpp"

namespace ea::osal {

namespace {

timespec to_timespec(Nanoseconds t) noexcept {
    timespec ts{};
    ts.tv_sec = static_cast<time_t>(t / kNsPerSec);
    ts.tv_nsec = static_cast<long>(t % kNsPerSec);
    return ts;
}

}  // namespace

PeriodicTask::~PeriodicTask() {
    stop();
}

Result<void> PeriodicTask::start(const TaskConfig& cfg, TaskFn fn, void* ctx) noexcept {
    if (running()) {
        return ErrorCode::kAlreadyInitialized;
    }
    if (fn == nullptr || cfg.period <= 0) {
        return ErrorCode::kInvalidArgument;
    }
    cfg_ = cfg;
    fn_ = fn;
    ctx_ = ctx;
    stop_requested_.store(false, std::memory_order_release);
    running_.store(true, std::memory_order_release);
    if (pthread_create(&thread_, nullptr, &PeriodicTask::entry, this) != 0) {
        running_.store(false, std::memory_order_release);
        return ErrorCode::kOsError;
    }
    return ok_result();
}

void PeriodicTask::stop() noexcept {
    if (!running()) {
        return;
    }
    stop_requested_.store(true, std::memory_order_release);
    (void)pthread_join(thread_, nullptr);
    running_.store(false, std::memory_order_release);
}

void* PeriodicTask::entry(void* self) noexcept {
    static_cast<PeriodicTask*>(self)->loop();
    return nullptr;
}

void PeriodicTask::loop() noexcept {
#if defined(__linux__)
    if (cfg_.name != nullptr) {
        char name[16] = {};
        std::strncpy(name, cfg_.name, sizeof(name) - 1U);
        (void)pthread_setname_np(pthread_self(), name);
    }
#endif
    // Scheduling setup failures are not fatal here (e.g. unprivileged SIL runs); deployment
    // configurations verify priorities at startup through the execution manager.
    (void)set_current_thread_fifo(cfg_.fifo_priority);
    (void)set_current_thread_affinity(cfg_.cpu_mask);

    const MonotonicClock clock;
    Nanoseconds next = clock.now() + cfg_.period;
    while (!stop_requested_.load(std::memory_order_acquire)) {
        const timespec ts = to_timespec(next);
        while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, nullptr) != 0) {
            // EINTR: retry with the same absolute deadline.
        }
        if (stop_requested_.load(std::memory_order_acquire)) {
            break;
        }
        fn_(ctx_);
        activations_.fetch_add(1U, std::memory_order_relaxed);
        next += cfg_.period;
        const Nanoseconds now = clock.now();
        if (now > next) {
            overruns_.fetch_add(1U, std::memory_order_relaxed);
            next = now + cfg_.period;  // re-anchor, do not burst
        }
    }
}

}  // namespace ea::osal
