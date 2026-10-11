#include "ea/plant/signals.hpp"

#include <cmath>

#include "ea/codec/bits.hpp"
#include "ea/core/assert.hpp"

namespace ea::plant {

void put(std::array<std::uint8_t, 8>& frame, const can::Sig& s, double physical) noexcept {
    const double raw_d = std::round(physical / s.factor);
    const std::int64_t lo = s.is_signed ? -(std::int64_t{1} << (s.length - 1U)) : 0;
    const std::int64_t hi = s.is_signed ? (std::int64_t{1} << (s.length - 1U)) - 1 : (std::int64_t{1} << s.length) - 1;
    const double clamped = raw_d < static_cast<double>(lo) ? static_cast<double>(lo)
                                                           : (raw_d > static_cast<double>(hi) ? static_cast<double>(hi) : raw_d);
    const std::uint64_t bits =
        static_cast<std::uint64_t>(static_cast<std::int64_t>(clamped)) & ((std::uint64_t{1} << s.length) - 1U);
    const Result<void> r = codec::insert(ByteSpan(frame.data(), frame.size()), can::kOrder, s.start, s.length, bits);
    EA_ASSERT(r.ok());
}

std::int64_t get_raw(ConstByteSpan frame, const can::Sig& s) noexcept {
    const Result<std::uint64_t> r = codec::extract(frame, can::kOrder, s.start, s.length);
    EA_ASSERT(r.ok());
    return s.is_signed ? codec::sign_extend(*r, s.length) : static_cast<std::int64_t>(*r);
}

}  // namespace ea::plant
