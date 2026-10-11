// Virtual monotonic clock for SIL: time only moves when the test says so (deterministic replay).
#pragma once

#include "ea/core/assert.hpp"
#include "ea/osal/clock.hpp"

namespace ea::sil {

class VirtualClock final : public osal::IClock {
public:
    osal::Nanoseconds now() const noexcept override { return now_; }
    void set(osal::Nanoseconds t) noexcept {
        EA_ASSERT(t >= now_);  // monotonic
        now_ = t;
    }
    void advance(osal::Nanoseconds dt) noexcept {
        EA_ASSERT(dt >= 0);
        now_ += dt;
    }

private:
    osal::Nanoseconds now_ = 0;
};

}  // namespace ea::sil
