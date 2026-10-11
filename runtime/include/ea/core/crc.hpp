// Table-driven CRCs used by automotive protocols. Tables are built at compile time.
//
//   CRC8  SAE-J1850   poly 0x1D,       init 0xFF,       xorout 0xFF,       check(123456789) = 0x4B
//   CRC16 CCITT-FALSE poly 0x1021,     init 0xFFFF,     xorout 0x0000,     check(123456789) = 0x29B1
//   CRC32 P4 (E2E P4) poly 0xF4ACFB13, init 0xFFFFFFFF, xorout 0xFFFFFFFF, reflected, check = 0x1697D06A
//   CRC32 IEEE 802.3  poly 0x04C11DB7, init 0xFFFFFFFF, xorout 0xFFFFFFFF, reflected, check = 0xCBF43926
#pragma once

#include <array>
#include <cstdint>

#include "ea/core/span.hpp"

namespace ea::crc {

namespace detail {

constexpr std::array<std::uint8_t, 256> make_crc8_table(std::uint8_t poly) {
    std::array<std::uint8_t, 256> t{};
    for (std::uint32_t i = 0U; i < 256U; ++i) {
        std::uint8_t c = static_cast<std::uint8_t>(i);
        for (int b = 0; b < 8; ++b) {
            c = ((c & 0x80U) != 0U) ? static_cast<std::uint8_t>((c << 1U) ^ poly) : static_cast<std::uint8_t>(c << 1U);
        }
        t[i] = c;
    }
    return t;
}

constexpr std::array<std::uint16_t, 256> make_crc16_table(std::uint16_t poly) {
    std::array<std::uint16_t, 256> t{};
    for (std::uint32_t i = 0U; i < 256U; ++i) {
        std::uint16_t c = static_cast<std::uint16_t>(i << 8U);
        for (int b = 0; b < 8; ++b) {
            c = ((c & 0x8000U) != 0U) ? static_cast<std::uint16_t>((c << 1U) ^ poly)
                                      : static_cast<std::uint16_t>(c << 1U);
        }
        t[i] = c;
    }
    return t;
}

constexpr std::array<std::uint32_t, 256> make_crc32_reflected_table(std::uint32_t reflected_poly) {
    std::array<std::uint32_t, 256> t{};
    for (std::uint32_t i = 0U; i < 256U; ++i) {
        std::uint32_t c = i;
        for (int b = 0; b < 8; ++b) {
            c = ((c & 1U) != 0U) ? ((c >> 1U) ^ reflected_poly) : (c >> 1U);
        }
        t[i] = c;
    }
    return t;
}

inline constexpr auto kCrc8J1850Table = make_crc8_table(0x1DU);
inline constexpr auto kCrc16CcittTable = make_crc16_table(0x1021U);
inline constexpr auto kCrc32P4Table = make_crc32_reflected_table(0xC8DF352FU);    // reflect(0xF4ACFB13)
inline constexpr auto kCrc32IeeeTable = make_crc32_reflected_table(0xEDB88320U);  // reflect(0x04C11DB7)

}  // namespace detail

inline std::uint8_t crc8_j1850(ConstByteSpan data) noexcept {
    std::uint8_t c = 0xFFU;
    for (const std::uint8_t b : data) {
        c = detail::kCrc8J1850Table[static_cast<std::uint8_t>(c ^ b)];
    }
    return static_cast<std::uint8_t>(c ^ 0xFFU);
}

inline std::uint16_t crc16_ccitt_false(ConstByteSpan data) noexcept {
    std::uint16_t c = 0xFFFFU;
    for (const std::uint8_t b : data) {
        c = static_cast<std::uint16_t>((c << 8U) ^ detail::kCrc16CcittTable[static_cast<std::uint8_t>((c >> 8U) ^ b)]);
    }
    return c;
}

// Streaming reflected CRC32 (used where a CRC spans non-contiguous regions, e.g. E2E P4).
template <const std::array<std::uint32_t, 256>& Table>
class Crc32Reflected {
public:
    void update(ConstByteSpan data) noexcept {
        for (const std::uint8_t b : data) {
            reg_ = (reg_ >> 8U) ^ Table[static_cast<std::uint8_t>(reg_ ^ b)];
        }
    }
    std::uint32_t final() const noexcept { return reg_ ^ 0xFFFFFFFFU; }

private:
    std::uint32_t reg_ = 0xFFFFFFFFU;
};

using Crc32P4 = Crc32Reflected<detail::kCrc32P4Table>;
using Crc32Ieee = Crc32Reflected<detail::kCrc32IeeeTable>;

inline std::uint32_t crc32_p4(ConstByteSpan data) noexcept {
    Crc32P4 c;
    c.update(data);
    return c.final();
}

inline std::uint32_t crc32_ieee(ConstByteSpan data) noexcept {
    Crc32Ieee c;
    c.update(data);
    return c.final();
}

}  // namespace ea::crc
