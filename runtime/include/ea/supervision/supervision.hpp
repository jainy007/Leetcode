// Health supervision primitives (rule MP-006), modelled on AUTOSAR PHM semantics:
//   Alive    - a periodic entity reports often enough (and not too often) per reference cycle
//   Deadline - time between two checkpoints lies within [min, max]
//   Logical  - checkpoints are reached in an allowed order
// All state is fixed-size; no heap. Not thread-safe unless stated (one owner per supervisor).
#pragma once

#include <atomic>
#include <cstdint>

#include "ea/osal/clock.hpp"

namespace ea::sup {

enum class Status : std::uint8_t { kOk, kFailed, kExpired };

const char* to_string(Status s) noexcept;

// Lock-free heartbeat counter: written by the supervised task, read by any supervisor.
class Heartbeat {
public:
    void beat() noexcept { count_.fetch_add(1U, std::memory_order_relaxed); }
    std::uint64_t count() const noexcept { return count_.load(std::memory_order_relaxed); }

private:
    std::atomic<std::uint64_t> count_{0U};
};

class AliveSupervision {
public:
    struct Config {
        std::uint32_t expected_per_cycle;  // alive indications expected per reference cycle
        std::uint32_t min_margin;          // tolerated shortfall
        std::uint32_t max_margin;          // tolerated excess
        std::uint32_t failed_tolerance;    // failed cycles tolerated before kExpired
    };
    explicit AliveSupervision(const Config& cfg) noexcept : cfg_(cfg) {}

    // Called once per reference cycle with the current heartbeat count.
    Status check(std::uint64_t heartbeat_count) noexcept;
    Status status() const noexcept { return status_; }

private:
    Config cfg_;
    std::uint64_t last_count_ = 0U;
    bool first_ = true;
    std::uint32_t failed_cycles_ = 0U;
    Status status_ = Status::kOk;
};

class DeadlineSupervision {
public:
    DeadlineSupervision(osal::Nanoseconds min, osal::Nanoseconds max) noexcept : min_(min), max_(max) {}

    void start(osal::Nanoseconds t) noexcept {
        started_ = true;
        t_start_ = t;
    }
    // Returns kFailed if the elapsed time lies outside [min, max] or start() was not called.
    Status stop(osal::Nanoseconds t) noexcept;
    osal::Nanoseconds last_elapsed() const noexcept { return last_elapsed_; }
    std::uint32_t violations() const noexcept { return violations_; }

private:
    osal::Nanoseconds min_;
    osal::Nanoseconds max_;
    osal::Nanoseconds t_start_ = 0;
    osal::Nanoseconds last_elapsed_ = 0;
    std::uint32_t violations_ = 0U;
    bool started_ = false;
};

// Up to 32 checkpoints; transitions given as a successor bitmask per checkpoint.
class LogicalSupervision {
public:
    static constexpr std::uint32_t kMaxCheckpoints = 32U;

    LogicalSupervision(const std::uint32_t* successors, std::uint32_t count, std::uint32_t initial) noexcept;
    Status reach(std::uint32_t checkpoint) noexcept;
    Status status() const noexcept { return status_; }

private:
    const std::uint32_t* successors_;
    std::uint32_t count_;
    std::uint32_t current_;
    Status status_ = Status::kOk;
};

}  // namespace ea::sup
