#include "ea/codec/bits.hpp"

namespace ea::codec {

namespace {

constexpr std::uint32_t kMaxSignalBits = 64U;

// Position of the i-th bit (i = 0 is the first bit visited) for the given byte order.
// Intel visits LSB..MSB, Motorola visits MSB..LSB.
std::uint32_t next_motorola(std::uint32_t pos) noexcept {
    return ((pos % 8U) == 0U) ? (pos + 15U) : (pos - 1U);
}

}  // namespace

bool signal_fits(ByteOrder order, std::uint32_t start_bit, std::uint32_t length, std::size_t payload_bytes) noexcept {
    if (length == 0U || length > kMaxSignalBits) {
        return false;
    }
    const std::uint64_t total_bits = static_cast<std::uint64_t>(payload_bytes) * 8U;
    if (start_bit >= total_bits) {
        return false;
    }
    if (order == ByteOrder::kLittleEndian) {
        return static_cast<std::uint64_t>(start_bit) + length <= total_bits;
    }
    std::uint32_t pos = start_bit;
    for (std::uint32_t i = 1U; i < length; ++i) {
        pos = next_motorola(pos);
        if (pos >= total_bits) {
            return false;
        }
    }
    return true;
}

Result<std::uint64_t> extract(ConstByteSpan data, ByteOrder order, std::uint32_t start_bit,
                              std::uint32_t length) noexcept {
    if (!signal_fits(order, start_bit, length, data.size())) {
        return ErrorCode::kOutOfRange;
    }
    std::uint64_t raw = 0U;
    if (order == ByteOrder::kLittleEndian) {
        for (std::uint32_t i = 0U; i < length; ++i) {
            const std::uint32_t pos = start_bit + i;
            const std::uint64_t bit = (static_cast<std::uint32_t>(data[pos / 8U]) >> (pos % 8U)) & 1U;
            raw |= bit << i;
        }
    } else {
        std::uint32_t pos = start_bit;
        for (std::uint32_t i = 0U; i < length; ++i) {
            const std::uint64_t bit = (static_cast<std::uint32_t>(data[pos / 8U]) >> (pos % 8U)) & 1U;
            raw = (raw << 1U) | bit;
            if (i + 1U < length) {
                pos = next_motorola(pos);
            }
        }
    }
    return raw;
}

Result<void> insert(ByteSpan data, ByteOrder order, std::uint32_t start_bit, std::uint32_t length,
                    std::uint64_t raw) noexcept {
    if (!signal_fits(order, start_bit, length, data.size())) {
        return ErrorCode::kOutOfRange;
    }
    if (length < kMaxSignalBits && (raw >> length) != 0U) {
        return ErrorCode::kOutOfRange;
    }
    const auto write_bit = [&data](std::uint32_t pos, std::uint64_t bit) {
        const std::uint8_t mask = static_cast<std::uint8_t>(1U << (pos % 8U));
        if (bit != 0U) {
            data[pos / 8U] = static_cast<std::uint8_t>(data[pos / 8U] | mask);
        } else {
            data[pos / 8U] = static_cast<std::uint8_t>(data[pos / 8U] & static_cast<std::uint8_t>(~mask));
        }
    };
    if (order == ByteOrder::kLittleEndian) {
        for (std::uint32_t i = 0U; i < length; ++i) {
            write_bit(start_bit + i, (raw >> i) & 1U);
        }
    } else {
        std::uint32_t pos = start_bit;
        for (std::uint32_t i = 0U; i < length; ++i) {
            write_bit(pos, (raw >> (length - 1U - i)) & 1U);
            if (i + 1U < length) {
                pos = next_motorola(pos);
            }
        }
    }
    return ok_result();
}

std::int64_t sign_extend(std::uint64_t raw, std::uint32_t length) noexcept {
    if (length == 0U || length >= kMaxSignalBits) {
        return static_cast<std::int64_t>(raw);
    }
    const std::uint64_t sign_bit = std::uint64_t{1} << (length - 1U);
    if ((raw & sign_bit) != 0U) {
        const std::uint64_t extended = raw | ~((std::uint64_t{1} << length) - 1U);
        return static_cast<std::int64_t>(extended);
    }
    return static_cast<std::int64_t>(raw);
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
