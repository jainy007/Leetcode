// Signal packing per DBC conventions (rule IPC-003: endian-explicit, bounds-checked codecs).
//
// Bit numbering: bit n lives in byte n/8 at position n%8 (bit 0 = LSB of byte 0).
//   Intel    (little endian, "@1"): start_bit is the signal's LSB; bits ascend.
//   Motorola (big endian,    "@0"): start_bit is the signal's MSB; bits descend within a byte
//                                    and continue at bit 7 of the next byte ("sawtooth").
#pragma once

#include <cstdint>

#include "ea/core/result.hpp"
#include "ea/core/span.hpp"

namespace ea::codec {

enum class ByteOrder : std::uint8_t { kLittleEndian, kBigEndian };

// True if the signal lies completely inside a payload of `payload_bytes` bytes.
bool signal_fits(ByteOrder order, std::uint32_t start_bit, std::uint32_t length, std::size_t payload_bytes) noexcept;

Result<std::uint64_t> extract(ConstByteSpan data, ByteOrder order, std::uint32_t start_bit,
                              std::uint32_t length) noexcept;

// Writes the low `length` bits of `raw`. Fails if raw does not fit into `length` bits.
Result<void> insert(ByteSpan data, ByteOrder order, std::uint32_t start_bit, std::uint32_t length,
                    std::uint64_t raw) noexcept;

// Two's-complement sign extension of a `length`-bit raw value.
std::int64_t sign_extend(std::uint64_t raw, std::uint32_t length) noexcept;

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
