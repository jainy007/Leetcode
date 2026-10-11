// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc (pattern: can_function)
// Inputs:    sha256:a92210828e1b938796052c00f628d3a6b33bcb8bcd815b565468a28dfeb74b5a
// Regenerate: python3 -m tools.gen

#include <gtest/gtest.h>

#include <array>
#include <cstdint>

#include "ea/codec/bits.hpp"
#include "ea/e2e/p11.hpp"
#include "acc/acc_controller.hpp"

// Robustness of the generated receive / send framework of AccController: paths that no
// driving scenario reaches (oversized payloads, wrong CRC, out-of-range value behind a valid CRC,
// unknown ids, state names, tx index out of range). Each rx message must count such frames as
// not received and become unhealthy after its timeout.

namespace adas::acc {
namespace {

TEST(AccControllerComponentTest, StateNames) {
    EXPECT_STREQ(to_string(State::kOff), "OFF");
    EXPECT_STREQ(to_string(State::kStandby), "STANDBY");
    EXPECT_STREQ(to_string(State::kActive), "ACTIVE");
    EXPECT_STREQ(to_string(State::kOverride), "OVERRIDE");
    EXPECT_STREQ(to_string(State::kFault), "FAULT");
    EXPECT_STREQ(to_string(static_cast<State>(255U)), "UNKNOWN");
}

TEST(AccControllerComponentTest, TxIndexOutOfRange) {
    const AccController c;
    EXPECT_EQ(c.tx_can_id(AccController::kTxCount), 0U);
    EXPECT_EQ(c.tx_frame(AccController::kTxCount).size(), 0U);
}

std::array<std::uint8_t, kRadarObjectPayloadLength> radar_object_frame(ea::e2e::P11Protector& prot, bool out_of_range) {
    RadarObject m{};
    m.crc = 127.0;
    m.counter = 7.0;
    m.obj_valid = 0.0;
    m.obj_range = 204.70000000000002;
    m.obj_range_rate = -0.05;
    m.obj_lateral_pos = -0.02;
    std::array<std::uint8_t, kRadarObjectPayloadLength> f{};
    EXPECT_TRUE(encode_radar_object(m, ea::ByteSpan(f.data(), f.size())).ok());
    (void)out_of_range;  // every raw value of every signal is in range
    EXPECT_TRUE(prot.protect(ea::ByteSpan(f.data(), f.size())).ok());
    return f;
}

TEST(AccControllerComponentTest, RadarObjectInvalidFramesCountAsMissing) {
    AccController c;
    ea::e2e::P11Protector prot(ea::e2e::P11Config{0x0200U, 0U, 8U, 8U, 3U});
    std::array<std::uint8_t, kRadarObjectPayloadLength> f = radar_object_frame(prot, false);
    c.on_frame(kRadarObjectCanId, ea::ConstByteSpan(f.data(), f.size()));
    c.on_frame(0x7FFU, ea::ConstByteSpan(f.data(), f.size()));  // unknown id: ignored
    c.step();
    ASSERT_TRUE(c.inputs().target_lateral_ok);
    const std::array<std::uint8_t, kRadarObjectPayloadLength + 1U> oversized{};
    for (std::uint32_t k = 0U; k < 6U; ++k) {
        if (k % 3U == 0U) {
            f = radar_object_frame(prot, true);
            f[0U] = static_cast<std::uint8_t>(f[0U] ^ 0x01U);
            c.on_frame(kRadarObjectCanId, ea::ConstByteSpan(f.data(), f.size()));
        } else if (k % 3U == 1U) {
            f = radar_object_frame(prot, false);
            f[0U] = static_cast<std::uint8_t>(f[0U] ^ 0x01U);  // wrong CRC
            c.on_frame(kRadarObjectCanId, ea::ConstByteSpan(f.data(), f.size()));
        } else {
            c.on_frame(kRadarObjectCanId, ea::ConstByteSpan(oversized.data(), oversized.size()));
        }
        c.step();
    }
    EXPECT_FALSE(c.inputs().target_lateral_ok);
    EXPECT_FALSE(c.inputs().all_ok);
}

std::array<std::uint8_t, kEgoStatePayloadLength> ego_state_frame(ea::e2e::P11Protector& prot, bool out_of_range) {
    EgoState m{};
    m.crc = 127.0;
    m.counter = 7.0;
    m.vehicle_speed = 327.67;
    std::array<std::uint8_t, kEgoStatePayloadLength> f{};
    EXPECT_TRUE(encode_ego_state(m, ea::ByteSpan(f.data(), f.size())).ok());
    (void)out_of_range;  // every raw value of every signal is in range
    EXPECT_TRUE(prot.protect(ea::ByteSpan(f.data(), f.size())).ok());
    return f;
}

TEST(AccControllerComponentTest, EgoStateInvalidFramesCountAsMissing) {
    AccController c;
    ea::e2e::P11Protector prot(ea::e2e::P11Config{0x0210U, 0U, 8U, 8U, 3U});
    std::array<std::uint8_t, kEgoStatePayloadLength> f = ego_state_frame(prot, false);
    c.on_frame(kEgoStateCanId, ea::ConstByteSpan(f.data(), f.size()));
    c.on_frame(0x7FFU, ea::ConstByteSpan(f.data(), f.size()));  // unknown id: ignored
    c.step();
    ASSERT_TRUE(c.inputs().ego_speed_ok);
    const std::array<std::uint8_t, kEgoStatePayloadLength + 1U> oversized{};
    for (std::uint32_t k = 0U; k < 3U; ++k) {
        if (k % 3U == 0U) {
            f = ego_state_frame(prot, true);
            f[0U] = static_cast<std::uint8_t>(f[0U] ^ 0x01U);
            c.on_frame(kEgoStateCanId, ea::ConstByteSpan(f.data(), f.size()));
        } else if (k % 3U == 1U) {
            f = ego_state_frame(prot, false);
            f[0U] = static_cast<std::uint8_t>(f[0U] ^ 0x01U);  // wrong CRC
            c.on_frame(kEgoStateCanId, ea::ConstByteSpan(f.data(), f.size()));
        } else {
            c.on_frame(kEgoStateCanId, ea::ConstByteSpan(oversized.data(), oversized.size()));
        }
        c.step();
    }
    EXPECT_FALSE(c.inputs().ego_speed_ok);
    EXPECT_FALSE(c.inputs().all_ok);
}

std::array<std::uint8_t, kDriverInputPayloadLength> driver_input_frame(ea::e2e::P11Protector& prot, bool out_of_range) {
    DriverInput m{};
    m.crc = 127.0;
    m.counter = 7.0;
    m.main_switch = 0.0;
    m.set_button = 0.0;
    m.resume_button = 0.0;
    m.cancel_button = 0.0;
    m.speed_up_button = 0.0;
    m.speed_down_button = 0.0;
    m.gap_setting = 2.0;
    m.brake_pedal = 0.0;
    m.accel_pedal_pos = 50.0;
    std::array<std::uint8_t, kDriverInputPayloadLength> f{};
    EXPECT_TRUE(encode_driver_input(m, ea::ByteSpan(f.data(), f.size())).ok());
    if (out_of_range) {  // accel_pedal_pos one step above its range, behind a valid CRC
        EXPECT_TRUE(ea::codec::insert(ea::ByteSpan(f.data(), f.size()), ea::codec::ByteOrder::kLittleEndian, 24U, 8U, 201ULL).ok());
    }
    EXPECT_TRUE(prot.protect(ea::ByteSpan(f.data(), f.size())).ok());
    return f;
}

TEST(AccControllerComponentTest, DriverInputInvalidFramesCountAsMissing) {
    AccController c;
    ea::e2e::P11Protector prot(ea::e2e::P11Config{0x0220U, 0U, 8U, 8U, 3U});
    std::array<std::uint8_t, kDriverInputPayloadLength> f = driver_input_frame(prot, false);
    c.on_frame(kDriverInputCanId, ea::ConstByteSpan(f.data(), f.size()));
    c.on_frame(0x7FFU, ea::ConstByteSpan(f.data(), f.size()));  // unknown id: ignored
    c.step();
    ASSERT_TRUE(c.inputs().accel_pedal_ok);
    const std::array<std::uint8_t, kDriverInputPayloadLength + 1U> oversized{};
    for (std::uint32_t k = 0U; k < 3U; ++k) {
        if (k % 3U == 0U) {
            f = driver_input_frame(prot, true);
            c.on_frame(kDriverInputCanId, ea::ConstByteSpan(f.data(), f.size()));
        } else if (k % 3U == 1U) {
            f = driver_input_frame(prot, false);
            f[0U] = static_cast<std::uint8_t>(f[0U] ^ 0x01U);  // wrong CRC
            c.on_frame(kDriverInputCanId, ea::ConstByteSpan(f.data(), f.size()));
        } else {
            c.on_frame(kDriverInputCanId, ea::ConstByteSpan(oversized.data(), oversized.size()));
        }
        c.step();
    }
    EXPECT_FALSE(c.inputs().accel_pedal_ok);
    EXPECT_FALSE(c.inputs().all_ok);
}

}  // namespace
}  // namespace adas::acc
