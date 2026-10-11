#include "ea/codec/bits.hpp"

namespace ea::codec {

Result<std::uint64_t> extract(ConstByteSpan data, ByteOrder order, std::uint32_t start_bit,
                              std::uint32_t length) noexcept {
    if (!signal_fits(order, start_bit, length, data.size())) {
        return ErrorCode::kOutOfRange;
    }
    return detail::extract_unchecked(data.data(), order, start_bit, length);
}

Result<void> insert(ByteSpan data, ByteOrder order, std::uint32_t start_bit, std::uint32_t length,
                    std::uint64_t raw) noexcept {
    if (!signal_fits(order, start_bit, length, data.size())) {
        return ErrorCode::kOutOfRange;
    }
    if (length < kMaxSignalBits && (raw >> length) != 0U) {
        return ErrorCode::kOutOfRange;
    }
    detail::insert_unchecked(data.data(), order, start_bit, length, raw);
    return ok_result();
}

Result<std::uint64_t> to_twos_complement(std::int64_t value, std::uint32_t length) noexcept {
    if (length == 0U || length > kMaxSignalBits) {
        return ErrorCode::kInvalidArgument;
    }
    if (length == kMaxSignalBits) {
        return static_cast<std::uint64_t>(value);
    }
    const std::int64_t min = -(std::int64_t{1} << (length - 1U));
    const std::int64_t max = (std::int64_t{1} << (length - 1U)) - 1;
    if (value < min || value > max) {
        return ErrorCode::kOutOfRange;
    }
    return static_cast<std::uint64_t>(value) & ((std::uint64_t{1} << length) - 1U);
}

}  // namespace ea::codec
