#include "ea/supervision/supervision.hpp"

namespace ea::sup {

const char* to_string(Status s) noexcept {
    switch (s) {
        case Status::kOk:
            return "OK";
        case Status::kFailed:
            return "FAILED";
        case Status::kExpired:
            return "EXPIRED";
    }
    return "UNKNOWN";
}

Status AliveSupervision::check(std::uint64_t heartbeat_count) noexcept {
    if (status_ == Status::kExpired) {
        return status_;  // latched until restart
    }
    if (first_) {
        first_ = false;
        last_count_ = heartbeat_count;
        return status_;
    }
    const std::uint64_t delta = heartbeat_count - last_count_;
    last_count_ = heartbeat_count;
    const std::uint64_t lo =
        (cfg_.expected_per_cycle > cfg_.min_margin) ? (cfg_.expected_per_cycle - cfg_.min_margin) : 0U;
    const std::uint64_t hi = static_cast<std::uint64_t>(cfg_.expected_per_cycle) + cfg_.max_margin;
    if (delta >= lo && delta <= hi) {
        failed_cycles_ = 0U;
        status_ = Status::kOk;
    } else {
        ++failed_cycles_;
        status_ = (failed_cycles_ > cfg_.failed_tolerance) ? Status::kExpired : Status::kFailed;
    }
    return status_;
}

Status DeadlineSupervision::stop(osal::Nanoseconds t) noexcept {
    if (!started_) {
        ++violations_;
        return Status::kFailed;
    }
    started_ = false;
    last_elapsed_ = t - t_start_;
    if (last_elapsed_ < min_ || last_elapsed_ > max_) {
        ++violations_;
        return Status::kFailed;
    }
    return Status::kOk;
}

LogicalSupervision::LogicalSupervision(const std::uint32_t* successors, std::uint32_t count,
                                       std::uint32_t initial) noexcept
    : successors_(successors), count_(count), current_(initial) {
    if (successors_ == nullptr || count_ == 0U || count_ > kMaxCheckpoints || initial >= count_) {
        status_ = Status::kExpired;
    }
}

Status LogicalSupervision::reach(std::uint32_t checkpoint) noexcept {
    if (status_ == Status::kExpired) {
        return status_;
    }
    if (checkpoint >= count_ || ((successors_[current_] >> checkpoint) & 1U) == 0U) {
        status_ = Status::kExpired;
        return status_;
    }
    current_ = checkpoint;
    return status_;
}

}  // namespace ea::sup
