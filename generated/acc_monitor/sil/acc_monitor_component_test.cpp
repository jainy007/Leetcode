// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc_monitor (pattern: can_function)
// Inputs:    sha256:870db9709bc2042a12245115be83e147f59dcb471925f63ce067348e3f85098c
// Regenerate: python3 -m tools.gen

#include <gtest/gtest.h>

#include <array>
#include <cstdint>

#include "ea/codec/bits.hpp"
#include "ea/e2e/p11.hpp"
#include "acc_monitor/acc_monitor.hpp"

// Robustness of the generated receive / send framework of AccMonitor: paths that no
// driving scenario reaches (oversized payloads, wrong CRC, out-of-range value behind a valid CRC,
// unknown ids, state names, tx index out of range). Each rx message must count such frames as
// not received and become unhealthy after its timeout.

namespace adas::acc_monitor {
namespace {

TEST(AccMonitorComponentTest, StateNames) {
    EXPECT_STREQ(to_string(State::kInit), "INIT");
    EXPECT_STREQ(to_string(State::kMonitoring), "MONITORING");
    EXPECT_STREQ(to_string(State::kSafeState), "SAFE_STATE");
    EXPECT_STREQ(to_string(static_cast<State>(255U)), "UNKNOWN");
}

TEST(AccMonitorComponentTest, TxIndexOutOfRange) {
    const AccMonitor c;
    EXPECT_EQ(c.tx_can_id(AccMonitor::kTxCount), 0U);
    EXPECT_EQ(c.tx_frame(AccMonitor::kTxCount).size(), 0U);
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

TEST(AccMonitorComponentTest, RadarObjectInvalidFramesCountAsMissing) {
    AccMonitor c;
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

std::array<std::uint8_t, kAccCommandPayloadLength> acc_command_frame(ea::e2e::P11Protector& prot, bool out_of_range) {
    AccCommand m{};
    m.crc = 127.0;
    m.counter = 7.0;
    m.acc_status = 2.0;
    m.accel_request = 2.5;
    m.brake_request = 5.0;
    m.steering_angle_request = 0.0;
    m.set_speed = 125.0;
    m.time_gap_setting = 1.0;
    std::array<std::uint8_t, kAccCommandPayloadLength> f{};
    EXPECT_TRUE(encode_acc_command(m, ea::ByteSpan(f.data(), f.size())).ok());
    if (out_of_range) {  // acc_status one step above its range, behind a valid CRC
        EXPECT_TRUE(ea::codec::insert(ea::ByteSpan(f.data(), f.size()), ea::codec::ByteOrder::kLittleEndian, 12U, 3U, 5ULL).ok());
    }
    EXPECT_TRUE(prot.protect(ea::ByteSpan(f.data(), f.size())).ok());
    return f;
}

TEST(AccMonitorComponentTest, AccCommandInvalidFramesCountAsMissing) {
    AccMonitor c;
    ea::e2e::P11Protector prot(ea::e2e::P11Config{0x0300U, 0U, 8U, 8U, 3U});
    std::array<std::uint8_t, kAccCommandPayloadLength> f = acc_command_frame(prot, false);
    c.on_frame(kAccCommandCanId, ea::ConstByteSpan(f.data(), f.size()));
    c.on_frame(0x7FFU, ea::ConstByteSpan(f.data(), f.size()));  // unknown id: ignored
    c.step();
    ASSERT_TRUE(c.inputs().cmd_accel_ok);
    const std::array<std::uint8_t, kAccCommandPayloadLength + 1U> oversized{};
    for (std::uint32_t k = 0U; k < 3U; ++k) {
        if (k % 3U == 0U) {
            f = acc_command_frame(prot, true);
            c.on_frame(kAccCommandCanId, ea::ConstByteSpan(f.data(), f.size()));
        } else if (k % 3U == 1U) {
            f = acc_command_frame(prot, false);
            f[0U] = static_cast<std::uint8_t>(f[0U] ^ 0x01U);  // wrong CRC
            c.on_frame(kAccCommandCanId, ea::ConstByteSpan(f.data(), f.size()));
        } else {
            c.on_frame(kAccCommandCanId, ea::ConstByteSpan(oversized.data(), oversized.size()));
        }
        c.step();
    }
    EXPECT_FALSE(c.inputs().cmd_accel_ok);
    EXPECT_FALSE(c.inputs().all_ok);
}

}  // namespace
}  // namespace adas::acc_monitor
