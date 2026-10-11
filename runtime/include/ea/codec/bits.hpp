// Signal packing per DBC conventions (rule IPC-003: endian-explicit, bounds-checked codecs).
//
// Bit numbering: bit n lives in byte n/8 at position n%8 (bit 0 = LSB of byte 0).
//   Intel    (little endian, "@1"): start_bit is the signal's LSB; bits ascend.
//   Motorola (big endian,    "@0"): start_bit is the signal's MSB; bits descend within a byte
//                                    and continue at bit 7 of the next byte ("sawtooth").
//
// Algorithms work byte-chunk-wise (at most 9 iterations for 64 bits). Three entry points:
//   extract()/insert()          runtime-checked, return Result
//   extract_fixed<>/insert_fixed<>  compile-time position; callers prove the bounds with
//                                   static_assert(signal_fits(...)) and check the payload length
//                                   once (used by generated codecs)
#pragma once

#include <cstddef>
#include <cstdint>

#include "ea/core/result.hpp"
#include "ea/core/span.hpp"

namespace ea::codec {

enum class ByteOrder : std::uint8_t { kLittleEndian, kBigEndian };

inline constexpr std::uint32_t kMaxSignalBits = 64U;

// True if the signal lies completely inside a payload of `payload_bytes` bytes.
constexpr bool signal_fits(ByteOrder order, std::uint32_t start_bit, std::uint32_t length,
                           std::size_t payload_bytes) noexcept {
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
    // Motorola: the first byte holds (start_bit % 8 + 1) bits, then whole bytes follow.
    const std::uint32_t first = (start_bit % 8U) + 1U;
    if (length <= first) {
        return true;
    }
    const std::uint64_t extra_bytes = (static_cast<std::uint64_t>(length - first) + 7U) / 8U;
    return (start_bit / 8U) + extra_bytes < payload_bytes;
}

namespace detail {

constexpr std::uint32_t low_mask8(std::uint32_t bits) noexcept {
    return (bits >= 8U) ? 0xFFU : ((1U << bits) - 1U);
}

// Preconditions (not checked): signal_fits(order, start, len, size of p).
constexpr std::uint64_t extract_unchecked(const std::uint8_t* p, ByteOrder order, std::uint32_t start,
                                          std::uint32_t len) noexcept {
    std::uint64_t raw = 0U;
    if (order == ByteOrder::kLittleEndian) {
        std::uint32_t pos = start;
        std::uint32_t done = 0U;
        while (done < len) {
            const std::uint32_t bit = pos % 8U;
            const std::uint32_t take = ((8U - bit) < (len - done)) ? (8U - bit) : (len - done);
            const std::uint32_t chunk = (static_cast<std::uint32_t>(p[pos / 8U]) >> bit) & low_mask8(take);
            raw |= static_cast<std::uint64_t>(chunk) << done;
            done += take;
            pos += take;
        }
    } else {
        std::uint32_t pos = start;
        std::uint32_t remaining = len;
        while (remaining > 0U) {
            const std::uint32_t bit = pos % 8U;
            const std::uint32_t take = ((bit + 1U) < remaining) ? (bit + 1U) : remaining;
            const std::uint32_t chunk =
                (static_cast<std::uint32_t>(p[pos / 8U]) >> (bit + 1U - take)) & low_mask8(take);
            raw = (raw << take) | chunk;  // take <= 8
            remaining -= take;
            pos = (pos / 8U + 1U) * 8U + 7U;
        }
    }
    return raw;
}

// Preconditions (not checked): signal fits, and raw < 2^len.
constexpr void insert_unchecked(std::uint8_t* p, ByteOrder order, std::uint32_t start, std::uint32_t len,
                                std::uint64_t raw) noexcept {
    if (order == ByteOrder::kLittleEndian) {
        std::uint32_t pos = start;
        std::uint32_t done = 0U;
        while (done < len) {
            const std::uint32_t bit = pos % 8U;
            const std::uint32_t take = ((8U - bit) < (len - done)) ? (8U - bit) : (len - done);
            const std::uint32_t mask = low_mask8(take) << bit;
            const std::uint32_t chunk = (static_cast<std::uint32_t>(raw >> done) << bit) & mask;
            p[pos / 8U] = static_cast<std::uint8_t>((static_cast<std::uint32_t>(p[pos / 8U]) & ~mask) | chunk);
            done += take;
            pos += take;
        }
    } else {
        std::uint32_t pos = start;
        std::uint32_t remaining = len;
        while (remaining > 0U) {
            const std::uint32_t bit = pos % 8U;
            const std::uint32_t take = ((bit + 1U) < remaining) ? (bit + 1U) : remaining;
            const std::uint32_t shift = bit + 1U - take;
            const std::uint32_t mask = low_mask8(take) << shift;
            const std::uint32_t chunk = (static_cast<std::uint32_t>(raw >> (remaining - take)) << shift) & mask;
            p[pos / 8U] = static_cast<std::uint8_t>((static_cast<std::uint32_t>(p[pos / 8U]) & ~mask) | chunk);
            remaining -= take;
            pos = (pos / 8U + 1U) * 8U + 7U;
        }
    }
}

}  // namespace detail

Result<std::uint64_t> extract(ConstByteSpan data, ByteOrder order, std::uint32_t start_bit,
                              std::uint32_t length) noexcept;

// Writes the low `length` bits of `raw`. Fails if raw does not fit into `length` bits.
Result<void> insert(ByteSpan data, ByteOrder order, std::uint32_t start_bit, std::uint32_t length,
                    std::uint64_t raw) noexcept;

// Compile-time positioned access for generated codecs. The caller guarantees the payload is at
// least as long as the length the position was proven against.
template <ByteOrder Order, std::uint32_t Start, std::uint32_t Len>
inline std::uint64_t extract_fixed(const std::uint8_t* p) noexcept {
    static_assert(Len >= 1U && Len <= kMaxSignalBits, "signal length out of range");
    return detail::extract_unchecked(p, Order, Start, Len);
}

template <ByteOrder Order, std::uint32_t Start, std::uint32_t Len>
inline void insert_fixed(std::uint8_t* p, std::uint64_t raw) noexcept {
    static_assert(Len >= 1U && Len <= kMaxSignalBits, "signal length out of range");
    detail::insert_unchecked(p, Order, Start, Len, raw);
}

// Two's-complement sign extension of a `length`-bit raw value.
constexpr std::int64_t sign_extend(std::uint64_t raw, std::uint32_t length) noexcept {
    if (length == 0U || length >= kMaxSignalBits) {
        return static_cast<std::int64_t>(raw);
    }
    const std::uint64_t sign_bit = std::uint64_t{1} << (length - 1U);
    if ((raw & sign_bit) != 0U) {
        return static_cast<std::int64_t>(raw | ~((std::uint64_t{1} << length) - 1U));
    }
    return static_cast<std::int64_t>(raw);
}

// Encode a signed value as `length`-bit two's complement. Fails if out of range.
Result<std::uint64_t> to_twos_complement(std::int64_t value, std::uint32_t length) noexcept;

// Byte-aligned big-endian helpers (SOME/IP serialization, E2E headers).
inline std::uint16_t load_be16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>((static_cast<std::uint16_t>(p[0]) << 8U) | p[1]);
}
inline std::uint32_t load_be32(const std::uint8_t* p) noexcept {
    return (static_cast<std::uint32_t>(p[0]) << 24U) | (static_cast<std::uint32_t>(p[1]) << 16U) |
           (static_cast<std::uint32_t>(p[2]) << 8U) | static_cast<std::uint32_t>(p[3]);
}
inline void store_be16(std::uint8_t* p, std::uint16_t v) noexcept {
    p[0] = static_cast<std::uint8_t>(v >> 8U);
    p[1] = static_cast<std::uint8_t>(v);
}
inline void store_be32(std::uint8_t* p, std::uint32_t v) noexcept {
    p[0] = static_cast<std::uint8_t>(v >> 24U);
    p[1] = static_cast<std::uint8_t>(v >> 16U);
    p[2] = static_cast<std::uint8_t>(v >> 8U);
    p[3] = static_cast<std::uint8_t>(v);
}

}  // namespace ea::codec
