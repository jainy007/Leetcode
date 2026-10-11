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
