#include <gtest/gtest.h>

#include <cstring>

#include "ea/core/crc.hpp"
#include "ea/core/sha256.hpp"

namespace {

const std::uint8_t kCheck[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

TEST(Crc, StandardCheckValues) {
    EXPECT_EQ(ea::crc::crc8_j1850(kCheck), 0x4BU);
    EXPECT_EQ(ea::crc::crc16_ccitt_false(kCheck), 0x29B1U);
    EXPECT_EQ(ea::crc::crc32_p4(kCheck), 0x1697D06AU);
    EXPECT_EQ(ea::crc::crc32_ieee(kCheck), 0xCBF43926U);
}

TEST(Crc, StreamingEqualsOneShot) {
    ea::crc::Crc32P4 c;
    c.update(ea::ConstByteSpan(kCheck, 4U));
    c.update(ea::ConstByteSpan(kCheck + 4, 5U));
    EXPECT_EQ(c.final(), 0x1697D06AU);
}

std::string hex(const ea::Sha256Digest& d) {
    static const char* digits = "0123456789abcdef";
    std::string s;
    for (std::uint8_t b : d) {
        s += digits[b >> 4U];
        s += digits[b & 0xFU];
    }
    return s;
}

TEST(Sha256, Fips180Vectors) {
    EXPECT_EQ(hex(ea::Sha256::digest(ea::ConstByteSpan())),
              "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    const char* abc = "abc";
    EXPECT_EQ(hex(ea::Sha256::digest(ea::ConstByteSpan(reinterpret_cast<const std::uint8_t*>(abc), 3U))),
              "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    const char* two_blocks = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    EXPECT_EQ(hex(ea::Sha256::digest(
                  ea::ConstByteSpan(reinterpret_cast<const std::uint8_t*>(two_blocks), std::strlen(two_blocks)))),
              "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
}

TEST(Sha256, ParseHex) {
    ea::Sha256Digest d{};
    EXPECT_TRUE(ea::parse_sha256_hex("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad", d));
    EXPECT_EQ(d[0], 0xBAU);
    EXPECT_FALSE(ea::parse_sha256_hex("zz", d));
    EXPECT_FALSE(ea::parse_sha256_hex("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad00", d));
}

}  // namespace

namespace {

std::uint32_t crc32_bitwise(const std::uint8_t* d, std::size_t n, std::uint32_t reflected_poly) {
    std::uint32_t c = 0xFFFFFFFFU;
    for (std::size_t i = 0; i < n; ++i) {
        c ^= d[i];
        for (int b = 0; b < 8; ++b) {
            c = ((c & 1U) != 0U) ? ((c >> 1U) ^ reflected_poly) : (c >> 1U);
        }
    }
    return c ^ 0xFFFFFFFFU;
}

TEST(Crc, SlicingBy8MatchesBitwiseForAllLengthsAndSplits) {
    std::array<std::uint8_t, 300> d{};
    for (std::size_t i = 0; i < d.size(); ++i) {
        d[i] = static_cast<std::uint8_t>(i * 131U + 7U);
    }
    for (std::size_t n = 0; n <= d.size(); ++n) {
        ASSERT_EQ(ea::crc::crc32_p4(ea::ConstByteSpan(d.data(), n)), crc32_bitwise(d.data(), n, 0xC8DF352FU)) << n;
        ASSERT_EQ(ea::crc::crc32_ieee(ea::ConstByteSpan(d.data(), n)), crc32_bitwise(d.data(), n, 0xEDB88320U)) << n;
        // Streaming in two arbitrary pieces gives the same result.
        ea::crc::Crc32P4 c;
        c.update(ea::ConstByteSpan(d.data(), n / 3U));
        c.update(ea::ConstByteSpan(d.data() + n / 3U, n - n / 3U));
        ASSERT_EQ(c.final(), crc32_bitwise(d.data(), n, 0xC8DF352FU)) << n;
    }
}

}  // namespace
