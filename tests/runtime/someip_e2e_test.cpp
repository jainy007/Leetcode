#include <gtest/gtest.h>

#include <array>

#include "ea/codec/someip.hpp"
#include "ea/e2e/p4.hpp"

namespace {

constexpr ea::someip::Expected kId{0x1234U, 0x8001U, 1U, ea::someip::MessageType::kNotification};
constexpr ea::e2e::P4Config kCfg{0x0A0B0C0DU, 0U, 16U, 64U, 2U};

TEST(SomeIp, HeaderRoundTrip) {
    std::array<std::uint8_t, 24> frame{};
    ASSERT_TRUE(ea::someip::write_header(frame, kId, 7U, 8U).ok());
    const auto payload = ea::someip::parse(frame, kId);
    ASSERT_TRUE(payload.ok());
    EXPECT_EQ(payload->size(), 8U);
}

TEST(SomeIp, RejectsWrongIdentityAndLength) {
    std::array<std::uint8_t, 24> frame{};
    ASSERT_TRUE(ea::someip::write_header(frame, kId, 7U, 8U).ok());
    auto other = kId;
    other.method_id = 0x8002U;
    EXPECT_EQ(ea::someip::parse(frame, other).error(), ea::ErrorCode::kProtocolError);
    EXPECT_EQ(ea::someip::parse(ea::ConstByteSpan(frame.data(), 20U), kId).error(), ea::ErrorCode::kLengthMismatch);
}

TEST(E2eP4, ProtectThenCheckSequence) {
    ea::e2e::P4Protector tx(kCfg);
    ea::e2e::P4Checker rx(kCfg);
    std::array<std::uint8_t, 20> p{};
    p[15] = 0x42U;
    ASSERT_TRUE(tx.protect(p).ok());
    EXPECT_EQ(rx.check(p), ea::e2e::P4Status::kOk);
    EXPECT_EQ(rx.check(p), ea::e2e::P4Status::kRepeated);
    ASSERT_TRUE(tx.protect(p).ok());  // counter 1, skipped
    ASSERT_TRUE(tx.protect(p).ok());  // counter 2
    EXPECT_EQ(rx.check(p), ea::e2e::P4Status::kOkSomeLost);
    for (int i = 0; i < 3; ++i) {
        ASSERT_TRUE(tx.protect(p).ok());
    }
    EXPECT_EQ(rx.check(p), ea::e2e::P4Status::kWrongSequence);
    EXPECT_EQ(rx.check(ea::ConstByteSpan()), ea::e2e::P4Status::kNoNewData);
}

TEST(E2eP4, DetectsCorruption) {
    ea::e2e::P4Protector tx(kCfg);
    ea::e2e::P4Checker rx(kCfg);
    std::array<std::uint8_t, 20> p{};
    ASSERT_TRUE(tx.protect(p).ok());
    p[17] ^= 0x01U;
    EXPECT_EQ(rx.check(p), ea::e2e::P4Status::kError);
    ea::e2e::P4Config other = kCfg;
    other.data_id = 1U;
    ea::e2e::P4Checker wrong_id(other);
    p[17] ^= 0x01U;
    EXPECT_EQ(wrong_id.check(p), ea::e2e::P4Status::kError);
    EXPECT_EQ(rx.check(ea::ConstByteSpan(p.data(), 15U)), ea::e2e::P4Status::kError);
}

}  // namespace

#include "ea/core/crc.hpp"
#include "ea/e2e/p11.hpp"

namespace {

constexpr ea::e2e::P11Config kP11{0x0123U, 0U, 8U, 8U, 2U};

TEST(E2eP11, CrcCoversDataIdAndPayloadExceptCrcByte) {
    std::array<std::uint8_t, 8> p{0x00U, 0x05U, 1U, 2U, 3U, 4U, 5U, 6U};
    // Independent reference: CRC8-J1850 over the byte string [id_lo, id_hi, p1..p7].
    const std::uint8_t ref_in[] = {0x23U, 0x01U, 0x05U, 1U, 2U, 3U, 4U, 5U, 6U};
    EXPECT_EQ(ea::e2e::p11_crc(kP11, p), ea::crc::crc8_j1850(ref_in));
    p[0] = 0xAAU;  // the CRC byte itself is excluded
    EXPECT_EQ(ea::e2e::p11_crc(kP11, p), ea::crc::crc8_j1850(ref_in));
}

TEST(E2eP11, SequenceAndWrapAt15) {
    ea::e2e::P11Protector tx(kP11);
    ea::e2e::P11Checker rx(kP11);
    std::array<std::uint8_t, 8> p{};
    for (int i = 0; i < 40; ++i) {  // crosses the 14 -> 0 wrap twice
        ASSERT_TRUE(tx.protect(p).ok());
        ASSERT_EQ(rx.check(p), ea::e2e::P4Status::kOk) << i;
        ASSERT_LT(p[1] & 0x0FU, 15U);
    }
    EXPECT_EQ(rx.check(p), ea::e2e::P4Status::kRepeated);
    ASSERT_TRUE(tx.protect(p).ok());
    ASSERT_TRUE(tx.protect(p).ok());
    EXPECT_EQ(rx.check(p), ea::e2e::P4Status::kOkSomeLost);
    for (int i = 0; i < 3; ++i) {
        ASSERT_TRUE(tx.protect(p).ok());
    }
    EXPECT_EQ(rx.check(p), ea::e2e::P4Status::kWrongSequence);
}

TEST(E2eP11, DetectsCorruptionReservedCounterAndLength) {
    ea::e2e::P11Protector tx(kP11);
    ea::e2e::P11Checker rx(kP11);
    std::array<std::uint8_t, 8> p{};
    ASSERT_TRUE(tx.protect(p).ok());
    p[5] ^= 0x10U;
    EXPECT_EQ(rx.check(p), ea::e2e::P4Status::kError);
    p[5] ^= 0x10U;
    p[1] = static_cast<std::uint8_t>(p[1] | 0x0FU);  // counter 15 is reserved
    p[0] = ea::e2e::p11_crc(kP11, p);
    EXPECT_EQ(rx.check(p), ea::e2e::P4Status::kError);
    EXPECT_EQ(rx.check(ea::ConstByteSpan(p.data(), 7U)), ea::e2e::P4Status::kError);
    EXPECT_FALSE(ea::e2e::p11_config_valid({0x1U, 1U, 8U, 8U, 2U}));  // counter inside CRC byte
    EXPECT_FALSE(ea::e2e::p11_config_valid({0x1U, 0U, 9U, 8U, 2U}));  // unaligned counter
}

}  // namespace
