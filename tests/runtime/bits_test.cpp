#include <gtest/gtest.h>

#include <array>

#include "ea/codec/bits.hpp"

namespace {

using ea::codec::ByteOrder;

TEST(Bits, IntelExtractInsertRoundTrip) {
    std::array<std::uint8_t, 8> d{};
    ASSERT_TRUE(ea::codec::insert(d, ByteOrder::kLittleEndian, 4U, 12U, 0xABCU).ok());
    EXPECT_EQ(d[0], 0xC0U);
    EXPECT_EQ(d[1], 0xABU);
    EXPECT_EQ(*ea::codec::extract(d, ByteOrder::kLittleEndian, 4U, 12U), 0xABCU);
}

TEST(Bits, MotorolaDbcSawtooth) {
    // DBC "SG_ X : 15|16@0+" -> bytes 1..2 big endian.
    std::array<std::uint8_t, 8> d{};
    ASSERT_TRUE(ea::codec::insert(d, ByteOrder::kBigEndian, 15U, 16U, 0x1234U).ok());
    EXPECT_EQ(d[1], 0x12U);
    EXPECT_EQ(d[2], 0x34U);
    EXPECT_EQ(*ea::codec::extract(d, ByteOrder::kBigEndian, 15U, 16U), 0x1234U);
    // "SG_ Y : 7|4@0+" -> high nibble of byte 0; "2|3@0+" -> bits 2..0.
    ASSERT_TRUE(ea::codec::insert(d, ByteOrder::kBigEndian, 7U, 4U, 0x9U).ok());
    ASSERT_TRUE(ea::codec::insert(d, ByteOrder::kBigEndian, 2U, 3U, 0x5U).ok());
    EXPECT_EQ(d[0], 0x95U);
    EXPECT_EQ(*ea::codec::extract(d, ByteOrder::kBigEndian, 7U, 4U), 0x9U);
    EXPECT_EQ(*ea::codec::extract(d, ByteOrder::kBigEndian, 2U, 3U), 0x5U);
}

TEST(Bits, MotorolaCrossesThreeBytes) {
    std::array<std::uint8_t, 4> d{};
    // 20-bit signal starting at bit 3 of byte 0: bits 3..0 | byte1 | byte2[7..0]
    ASSERT_TRUE(ea::codec::insert(d, ByteOrder::kBigEndian, 3U, 20U, 0xABCDEU).ok());
    EXPECT_EQ(d[0], 0x0AU);
    EXPECT_EQ(d[1], 0xBCU);
    EXPECT_EQ(d[2], 0xDEU);
    EXPECT_EQ(*ea::codec::extract(d, ByteOrder::kBigEndian, 3U, 20U), 0xABCDEU);
}

TEST(Bits, BoundsAreChecked) {
    std::array<std::uint8_t, 2> d{};
    EXPECT_FALSE(ea::codec::extract(d, ByteOrder::kLittleEndian, 10U, 8U).ok());
    EXPECT_FALSE(ea::codec::extract(d, ByteOrder::kBigEndian, 8U, 10U).ok());
    EXPECT_FALSE(ea::codec::insert(d, ByteOrder::kLittleEndian, 0U, 4U, 0x10U).ok());  // raw too wide
    EXPECT_FALSE(ea::codec::signal_fits(ByteOrder::kLittleEndian, 0U, 0U, 2U));
    EXPECT_TRUE(ea::codec::signal_fits(ByteOrder::kBigEndian, 7U, 16U, 2U));
}

TEST(Bits, SignHandling) {
    EXPECT_EQ(ea::codec::sign_extend(0xFFFU, 12U), -1);
    EXPECT_EQ(ea::codec::sign_extend(0x7FFU, 12U), 2047);
    EXPECT_EQ(*ea::codec::to_twos_complement(-2, 8U), 0xFEU);
    EXPECT_FALSE(ea::codec::to_twos_complement(128, 8U).ok());
    EXPECT_FALSE(ea::codec::to_twos_complement(-129, 8U).ok());
}

}  // namespace

// ---- Equivalence of the byte-chunk implementation with a bit-by-bit reference -------------
namespace {

std::uint32_t ref_next(std::uint32_t pos) {
    return ((pos % 8U) == 0U) ? (pos + 15U) : (pos - 1U);
}

bool ref_fits(ByteOrder order, std::uint32_t start, std::uint32_t len, std::size_t bytes) {
    if (len == 0U || len > 64U || start >= bytes * 8U) {
        return false;
    }
    if (order == ByteOrder::kLittleEndian) {
        return start + len <= bytes * 8U;
    }
    std::uint32_t pos = start;
    for (std::uint32_t i = 1U; i < len; ++i) {
        pos = ref_next(pos);
        if (pos >= bytes * 8U) {
            return false;
        }
    }
    return true;
}

std::uint64_t ref_extract(const std::uint8_t* d, ByteOrder order, std::uint32_t start, std::uint32_t len) {
    std::uint64_t raw = 0U;
    std::uint32_t pos = start;
    for (std::uint32_t i = 0U; i < len; ++i) {
        const std::uint64_t bit = (static_cast<std::uint32_t>(d[pos / 8U]) >> (pos % 8U)) & 1U;
        if (order == ByteOrder::kLittleEndian) {
            raw |= bit << i;
            pos = start + i + 1U;
        } else {
            raw = (raw << 1U) | bit;
            pos = ref_next(pos);
        }
    }
    return raw;
}

void ref_insert(std::uint8_t* d, ByteOrder order, std::uint32_t start, std::uint32_t len, std::uint64_t raw) {
    std::uint32_t pos = start;
    for (std::uint32_t i = 0U; i < len; ++i) {
        const std::uint64_t bit = (order == ByteOrder::kLittleEndian) ? ((raw >> i) & 1U) : ((raw >> (len - 1U - i)) & 1U);
        const auto mask = static_cast<std::uint8_t>(1U << (pos % 8U));
        d[pos / 8U] = static_cast<std::uint8_t>(bit != 0U ? (d[pos / 8U] | mask) : (d[pos / 8U] & ~mask));
        pos = (order == ByteOrder::kLittleEndian) ? (start + i + 1U) : ref_next(pos);
    }
}

TEST(Bits, ExhaustiveEquivalenceWithReference) {
    constexpr std::size_t kBytes = 10U;
    std::uint64_t seed = 0x9E3779B97F4A7C15ULL;
    const auto next = [&seed] {
        seed ^= seed << 13U;
        seed ^= seed >> 7U;
        seed ^= seed << 17U;
        return seed;
    };
    for (const ByteOrder order : {ByteOrder::kLittleEndian, ByteOrder::kBigEndian}) {
        for (std::uint32_t start = 0U; start < kBytes * 8U + 2U; ++start) {
            for (std::uint32_t len = 0U; len <= 65U; ++len) {
                const bool fits = ref_fits(order, start, len, kBytes);
                ASSERT_EQ(ea::codec::signal_fits(order, start, len, kBytes), fits) << start << "/" << len;
                if (!fits) {
                    continue;
                }
                std::array<std::uint8_t, kBytes> a{};
                for (auto& b : a) {
                    b = static_cast<std::uint8_t>(next());
                }
                ASSERT_EQ(*ea::codec::extract(a, order, start, len), ref_extract(a.data(), order, start, len));
                const std::uint64_t raw = (len == 64U) ? next() : (next() & ((std::uint64_t{1} << len) - 1U));
                std::array<std::uint8_t, kBytes> b = a;
                std::array<std::uint8_t, kBytes> r = a;
                ASSERT_TRUE(ea::codec::insert(b, order, start, len, raw).ok());
                ref_insert(r.data(), order, start, len, raw);
                ASSERT_EQ(b, r) << start << "/" << len;  // also proves neighbouring bits are untouched
            }
        }
    }
}

TEST(Bits, FixedVariantsMatch) {
    std::array<std::uint8_t, 8> d{0x12U, 0x34U, 0x56U, 0x78U, 0x9AU, 0xBCU, 0xDEU, 0xF0U};
    static_assert(ea::codec::signal_fits(ByteOrder::kBigEndian, 3U, 20U, 8U), "compile-time proof");
    EXPECT_EQ((ea::codec::extract_fixed<ByteOrder::kBigEndian, 3U, 20U>(d.data())),
              *ea::codec::extract(d, ByteOrder::kBigEndian, 3U, 20U));
    EXPECT_EQ((ea::codec::extract_fixed<ByteOrder::kLittleEndian, 5U, 33U>(d.data())),
              *ea::codec::extract(d, ByteOrder::kLittleEndian, 5U, 33U));
    ea::codec::insert_fixed<ByteOrder::kBigEndian, 15U, 16U>(d.data(), 0xBEEFU);
    EXPECT_EQ(d[1], 0xBEU);
    EXPECT_EQ(d[2], 0xEFU);
}

}  // namespace
