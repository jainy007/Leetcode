// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb_monitor (pattern: can_function)
// Inputs:    sha256:230536ebb5fd8cf2e5c6eff700cb3596edf001524e2100e20ccfddcdcbbad55e
// Regenerate: python3 -m tools.gen

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <limits>

#include "ea/codec/bits.hpp"
#include "aeb_monitor/aeb_monitor_codec.hpp"

// Back-to-back test of the generated codec against the runtime's generic bit codec
// (ea::codec::extract / insert), with vectors the generator derives from the interface
// definition: every signal at the low, high and middle raw value of its physical range, values
// just outside the range, NaN and wrong payload lengths. Complements the closed-loop SIL, which
// exercises only the messages and values a scenario reaches.

namespace adas::aeb_monitor {
namespace {

std::uint64_t ref_extract(const std::uint8_t* data, std::size_t size, ea::codec::ByteOrder order, std::uint32_t start,
                          std::uint32_t length) {
    const ea::Result<std::uint64_t> r = ea::codec::extract(ea::ConstByteSpan(data, size), order, start, length);
    EXPECT_TRUE(r.ok());
    return r.ok() ? *r : 0U;
}

AebCommand aeb_command_vector(int which) {
    AebCommand v{};
    v.crc = which == 0 ? 0.0 : (which == 1 ? 255.0 : 127.0);
    v.counter = which == 0 ? 0.0 : (which == 1 ? 15.0 : 7.0);
    v.aeb_status = which == 0 ? 0.0 : (which == 1 ? 4.0 : 2.0);
    v.fcw_warning = which == 0 ? 0.0 : (which == 1 ? 1.0 : 0.0);
    v.aeb_brake_request = which == 0 ? 0.0 : (which == 1 ? 10.0 : 5.0);
    return v;
}

TEST(AebMonitorCodecTest, AebCommandLayoutMatchesReference) {
    for (int which = 0; which < 3; ++which) {
        std::array<std::uint8_t, kAebCommandPayloadLength> buf{};
        ASSERT_TRUE(encode_aeb_command(aeb_command_vector(which), ea::ByteSpan(buf.data(), buf.size())).ok());
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 0U, 8U),
                  which == 0 ? 0ULL : (which == 1 ? 255ULL : 127ULL)) << "crc";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 8U, 4U),
                  which == 0 ? 0ULL : (which == 1 ? 15ULL : 7ULL)) << "counter";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 12U, 3U),
                  which == 0 ? 0ULL : (which == 1 ? 4ULL : 2ULL)) << "aeb_status";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 15U, 1U),
                  which == 0 ? 0ULL : (which == 1 ? 1ULL : 0ULL)) << "fcw_warning";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 16U, 10U),
                  which == 0 ? 0ULL : (which == 1 ? 1000ULL : 500ULL)) << "aeb_brake_request";
        const ea::Result<AebCommand> d = decode_aeb_command(ea::ConstByteSpan(buf.data(), buf.size()));
        ASSERT_TRUE(d.ok());
        EXPECT_DOUBLE_EQ(d->crc, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 255LL : 127LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->counter, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 15LL : 7LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->aeb_status, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 4LL : 2LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->fcw_warning, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 1LL : 0LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->aeb_brake_request, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 1000LL : 500LL)) * 0.01 + 0.0);
    }
}

TEST(AebMonitorCodecTest, AebCommandRejectsInvalidData) {
    std::array<std::uint8_t, kAebCommandPayloadLength> buf{};
    std::array<std::uint8_t, kAebCommandPayloadLength + 1U> longer{};
    EXPECT_EQ(encode_aeb_command(aeb_command_vector(2), ea::ByteSpan(longer.data(), longer.size())).error(), ea::ErrorCode::kLengthMismatch);
    EXPECT_EQ(decode_aeb_command(ea::ConstByteSpan(buf.data(), buf.size() - 1U)).error(), ea::ErrorCode::kLengthMismatch);
    {
        AebCommand v = aeb_command_vector(2);
        v.crc = 256.0;
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.crc = -1.0;
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.crc = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        AebCommand v = aeb_command_vector(2);
        v.counter = 16.0;
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.counter = -1.0;
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.counter = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        AebCommand v = aeb_command_vector(2);
        v.aeb_status = 5.0;
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.aeb_status = -1.0;
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.aeb_status = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        ASSERT_TRUE(encode_aeb_command(aeb_command_vector(2), ea::ByteSpan(buf.data(), buf.size())).ok());
        {
            std::array<std::uint8_t, kAebCommandPayloadLength> bad = buf;
            ASSERT_TRUE(ea::codec::insert(ea::ByteSpan(bad.data(), bad.size()), ea::codec::ByteOrder::kLittleEndian, 12U, 3U, 5ULL).ok());
            EXPECT_EQ(decode_aeb_command(ea::ConstByteSpan(bad.data(), bad.size())).error(), ea::ErrorCode::kOutOfRange) << "aeb_status";
        }
    }
    {
        AebCommand v = aeb_command_vector(2);
        v.fcw_warning = 2.0;
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.fcw_warning = -1.0;
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.fcw_warning = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        AebCommand v = aeb_command_vector(2);
        v.aeb_brake_request = 10.01;
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.aeb_brake_request = -0.01;
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.aeb_brake_request = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_aeb_command(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        ASSERT_TRUE(encode_aeb_command(aeb_command_vector(2), ea::ByteSpan(buf.data(), buf.size())).ok());
        {
            std::array<std::uint8_t, kAebCommandPayloadLength> bad = buf;
            ASSERT_TRUE(ea::codec::insert(ea::ByteSpan(bad.data(), bad.size()), ea::codec::ByteOrder::kLittleEndian, 16U, 10U, 1001ULL).ok());
            EXPECT_EQ(decode_aeb_command(ea::ConstByteSpan(bad.data(), bad.size())).error(), ea::ErrorCode::kOutOfRange) << "aeb_brake_request";
        }
    }
}

AebMonitorStatus aeb_monitor_status_vector(int which) {
    AebMonitorStatus v{};
    v.crc = which == 0 ? 0.0 : (which == 1 ? 255.0 : 127.0);
    v.counter = which == 0 ? 0.0 : (which == 1 ? 15.0 : 7.0);
    v.mon_status = which == 0 ? 0.0 : (which == 1 ? 2.0 : 1.0);
    v.permit = which == 0 ? 0.0 : (which == 1 ? 1.0 : 0.0);
    v.approved_counter = which == 0 ? 0.0 : (which == 1 ? 15.0 : 7.0);
    v.fault_reason = which == 0 ? 0.0 : (which == 1 ? 15.0 : 7.0);
    return v;
}

TEST(AebMonitorCodecTest, AebMonitorStatusLayoutMatchesReference) {
    for (int which = 0; which < 3; ++which) {
        std::array<std::uint8_t, kAebMonitorStatusPayloadLength> buf{};
        ASSERT_TRUE(encode_aeb_monitor_status(aeb_monitor_status_vector(which), ea::ByteSpan(buf.data(), buf.size())).ok());
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 0U, 8U),
                  which == 0 ? 0ULL : (which == 1 ? 255ULL : 127ULL)) << "crc";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 8U, 4U),
                  which == 0 ? 0ULL : (which == 1 ? 15ULL : 7ULL)) << "counter";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 12U, 2U),
                  which == 0 ? 0ULL : (which == 1 ? 2ULL : 1ULL)) << "mon_status";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 14U, 1U),
                  which == 0 ? 0ULL : (which == 1 ? 1ULL : 0ULL)) << "permit";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 16U, 4U),
                  which == 0 ? 0ULL : (which == 1 ? 15ULL : 7ULL)) << "approved_counter";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 20U, 4U),
                  which == 0 ? 0ULL : (which == 1 ? 15ULL : 7ULL)) << "fault_reason";
        const ea::Result<AebMonitorStatus> d = decode_aeb_monitor_status(ea::ConstByteSpan(buf.data(), buf.size()));
        ASSERT_TRUE(d.ok());
        EXPECT_DOUBLE_EQ(d->crc, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 255LL : 127LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->counter, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 15LL : 7LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->mon_status, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 2LL : 1LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->permit, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 1LL : 0LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->approved_counter, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 15LL : 7LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->fault_reason, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 15LL : 7LL)) * 1.0 + 0.0);
    }
}

TEST(AebMonitorCodecTest, AebMonitorStatusRejectsInvalidData) {
    std::array<std::uint8_t, kAebMonitorStatusPayloadLength> buf{};
    std::array<std::uint8_t, kAebMonitorStatusPayloadLength + 1U> longer{};
    EXPECT_EQ(encode_aeb_monitor_status(aeb_monitor_status_vector(2), ea::ByteSpan(longer.data(), longer.size())).error(), ea::ErrorCode::kLengthMismatch);
    EXPECT_EQ(decode_aeb_monitor_status(ea::ConstByteSpan(buf.data(), buf.size() - 1U)).error(), ea::ErrorCode::kLengthMismatch);
    {
        AebMonitorStatus v = aeb_monitor_status_vector(2);
        v.crc = 256.0;
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.crc = -1.0;
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.crc = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        AebMonitorStatus v = aeb_monitor_status_vector(2);
        v.counter = 16.0;
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.counter = -1.0;
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.counter = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        AebMonitorStatus v = aeb_monitor_status_vector(2);
        v.mon_status = 3.0;
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.mon_status = -1.0;
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.mon_status = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        ASSERT_TRUE(encode_aeb_monitor_status(aeb_monitor_status_vector(2), ea::ByteSpan(buf.data(), buf.size())).ok());
        {
            std::array<std::uint8_t, kAebMonitorStatusPayloadLength> bad = buf;
            ASSERT_TRUE(ea::codec::insert(ea::ByteSpan(bad.data(), bad.size()), ea::codec::ByteOrder::kLittleEndian, 12U, 2U, 3ULL).ok());
            EXPECT_EQ(decode_aeb_monitor_status(ea::ConstByteSpan(bad.data(), bad.size())).error(), ea::ErrorCode::kOutOfRange) << "mon_status";
        }
    }
    {
        AebMonitorStatus v = aeb_monitor_status_vector(2);
        v.permit = 2.0;
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.permit = -1.0;
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.permit = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        AebMonitorStatus v = aeb_monitor_status_vector(2);
        v.approved_counter = 16.0;
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.approved_counter = -1.0;
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.approved_counter = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        AebMonitorStatus v = aeb_monitor_status_vector(2);
        v.fault_reason = 16.0;
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.fault_reason = -1.0;
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.fault_reason = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_aeb_monitor_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
}

RadarObject radar_object_vector(int which) {
    RadarObject v{};
    v.crc = which == 0 ? 0.0 : (which == 1 ? 255.0 : 127.0);
    v.counter = which == 0 ? 0.0 : (which == 1 ? 15.0 : 7.0);
    v.obj_valid = which == 0 ? 0.0 : (which == 1 ? 1.0 : 0.0);
    v.obj_range = which == 0 ? 0.0 : (which == 1 ? 409.5 : 204.70000000000002);
    v.obj_range_rate = which == 0 ? -102.4 : (which == 1 ? 102.35000000000001 : -0.05);
    v.obj_lateral_pos = which == 0 ? -10.24 : (which == 1 ? 10.22 : -0.02);
    return v;
}

TEST(AebMonitorCodecTest, RadarObjectLayoutMatchesReference) {
    for (int which = 0; which < 3; ++which) {
        std::array<std::uint8_t, kRadarObjectPayloadLength> buf{};
        ASSERT_TRUE(encode_radar_object(radar_object_vector(which), ea::ByteSpan(buf.data(), buf.size())).ok());
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 0U, 8U),
                  which == 0 ? 0ULL : (which == 1 ? 255ULL : 127ULL)) << "crc";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 8U, 4U),
                  which == 0 ? 0ULL : (which == 1 ? 15ULL : 7ULL)) << "counter";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 12U, 1U),
                  which == 0 ? 0ULL : (which == 1 ? 1ULL : 0ULL)) << "obj_valid";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 16U, 12U),
                  which == 0 ? 0ULL : (which == 1 ? 4095ULL : 2047ULL)) << "obj_range";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 28U, 12U),
                  which == 0 ? 2048ULL : (which == 1 ? 2047ULL : 4095ULL)) << "obj_range_rate";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kLittleEndian, 40U, 10U),
                  which == 0 ? 512ULL : (which == 1 ? 511ULL : 1023ULL)) << "obj_lateral_pos";
        const ea::Result<RadarObject> d = decode_radar_object(ea::ConstByteSpan(buf.data(), buf.size()));
        ASSERT_TRUE(d.ok());
        EXPECT_DOUBLE_EQ(d->crc, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 255LL : 127LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->counter, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 15LL : 7LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->obj_valid, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 1LL : 0LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->obj_range, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 4095LL : 2047LL)) * 0.1 + 0.0);
        EXPECT_DOUBLE_EQ(d->obj_range_rate, static_cast<double>(which == 0 ? -2048LL : (which == 1 ? 2047LL : -1LL)) * 0.05 + 0.0);
        EXPECT_DOUBLE_EQ(d->obj_lateral_pos, static_cast<double>(which == 0 ? -512LL : (which == 1 ? 511LL : -1LL)) * 0.02 + 0.0);
    }
}

TEST(AebMonitorCodecTest, RadarObjectRejectsInvalidData) {
    std::array<std::uint8_t, kRadarObjectPayloadLength> buf{};
    std::array<std::uint8_t, kRadarObjectPayloadLength + 1U> longer{};
    EXPECT_EQ(encode_radar_object(radar_object_vector(2), ea::ByteSpan(longer.data(), longer.size())).error(), ea::ErrorCode::kLengthMismatch);
    EXPECT_EQ(decode_radar_object(ea::ConstByteSpan(buf.data(), buf.size() - 1U)).error(), ea::ErrorCode::kLengthMismatch);
    {
        RadarObject v = radar_object_vector(2);
        v.crc = 256.0;
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.crc = -1.0;
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.crc = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        RadarObject v = radar_object_vector(2);
        v.counter = 16.0;
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.counter = -1.0;
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.counter = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        RadarObject v = radar_object_vector(2);
        v.obj_valid = 2.0;
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.obj_valid = -1.0;
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.obj_valid = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        RadarObject v = radar_object_vector(2);
        v.obj_range = 409.6;
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.obj_range = -0.1;
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.obj_range = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        RadarObject v = radar_object_vector(2);
        v.obj_range_rate = 102.4;
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.obj_range_rate = -102.45;
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.obj_range_rate = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        RadarObject v = radar_object_vector(2);
        v.obj_lateral_pos = 10.24;
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.obj_lateral_pos = -10.26;
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.obj_lateral_pos = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_radar_object(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
}

}  // namespace
}  // namespace adas::aeb_monitor
