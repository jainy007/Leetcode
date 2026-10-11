// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb_monitor (pattern: can_function)
// Inputs:    sha256:31e2c7fbd93dd01fd60ce38fb1537e40edd8600b328b348b62317cb35b638c06
// Regenerate: python3 -m tools.gen

#include <gtest/gtest.h>

#include <array>
#include <cstdint>

#include "ea/codec/bits.hpp"
#include "ea/e2e/p11.hpp"
#include "aeb_monitor/aeb_monitor.hpp"

// Robustness of the generated receive / send framework of AebMonitor: paths that no
// driving scenario reaches (oversized payloads, wrong CRC, out-of-range value behind a valid CRC,
// unknown ids, state names, tx index out of range). Each rx message must count such frames as
// not received and become unhealthy after its timeout.

namespace adas::aeb_monitor {
namespace {

TEST(AebMonitorComponentTest, StateNames) {
    EXPECT_STREQ(to_string(State::kInit), "INIT");
    EXPECT_STREQ(to_string(State::kMonitoring), "MONITORING");
    EXPECT_STREQ(to_string(State::kSafeState), "SAFE_STATE");
    EXPECT_STREQ(to_string(static_cast<State>(255U)), "UNKNOWN");
}

TEST(AebMonitorComponentTest, TxIndexOutOfRange) {
    const AebMonitor c;
    EXPECT_EQ(c.tx_can_id(AebMonitor::kTxCount), 0U);
    EXPECT_EQ(c.tx_frame(AebMonitor::kTxCount).size(), 0U);
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

TEST(AebMonitorComponentTest, RadarObjectInvalidFramesCountAsMissing) {
    AebMonitor c;
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

std::array<std::uint8_t, kAebCommandPayloadLength> aeb_command_frame(ea::e2e::P11Protector& prot, bool out_of_range) {
    AebCommand m{};
    m.crc = 127.0;
    m.counter = 7.0;
    m.aeb_status = 2.0;
    m.fcw_warning = 0.0;
    m.aeb_brake_request = 5.0;
    std::array<std::uint8_t, kAebCommandPayloadLength> f{};
    EXPECT_TRUE(encode_aeb_command(m, ea::ByteSpan(f.data(), f.size())).ok());
    if (out_of_range) {  // aeb_status one step above its range, behind a valid CRC
        EXPECT_TRUE(ea::codec::insert(ea::ByteSpan(f.data(), f.size()), ea::codec::ByteOrder::kLittleEndian, 12U, 3U, 5ULL).ok());
    }
    EXPECT_TRUE(prot.protect(ea::ByteSpan(f.data(), f.size())).ok());
    return f;
}

TEST(AebMonitorComponentTest, AebCommandInvalidFramesCountAsMissing) {
    AebMonitor c;
    ea::e2e::P11Protector prot(ea::e2e::P11Config{0x0310U, 0U, 8U, 8U, 3U});
    std::array<std::uint8_t, kAebCommandPayloadLength> f = aeb_command_frame(prot, false);
    c.on_frame(kAebCommandCanId, ea::ConstByteSpan(f.data(), f.size()));
    c.on_frame(0x7FFU, ea::ConstByteSpan(f.data(), f.size()));  // unknown id: ignored
    c.step();
    ASSERT_TRUE(c.inputs().cmd_brake_ok);
    const std::array<std::uint8_t, kAebCommandPayloadLength + 1U> oversized{};
    for (std::uint32_t k = 0U; k < 3U; ++k) {
        if (k % 3U == 0U) {
            f = aeb_command_frame(prot, true);
            c.on_frame(kAebCommandCanId, ea::ConstByteSpan(f.data(), f.size()));
        } else if (k % 3U == 1U) {
            f = aeb_command_frame(prot, false);
            f[0U] = static_cast<std::uint8_t>(f[0U] ^ 0x01U);  // wrong CRC
            c.on_frame(kAebCommandCanId, ea::ConstByteSpan(f.data(), f.size()));
        } else {
            c.on_frame(kAebCommandCanId, ea::ConstByteSpan(oversized.data(), oversized.size()));
        }
        c.step();
    }
    EXPECT_FALSE(c.inputs().cmd_brake_ok);
    EXPECT_FALSE(c.inputs().all_ok);
}

}  // namespace
}  // namespace adas::aeb_monitor
