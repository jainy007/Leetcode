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
