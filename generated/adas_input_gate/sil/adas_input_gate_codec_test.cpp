// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   adas_input_gate (pattern: input_gate)
// Inputs:    sha256:e0c4923740db7d928336d8abc9859ca05e37446222ba5a081ee7c47bc53cdec6
// Regenerate: python3 -m tools.gen

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <limits>

#include "ea/codec/bits.hpp"
#include "adas_input_gate/adas_input_gate_codec.hpp"

// Back-to-back test of the generated codec against the runtime's generic bit codec
// (ea::codec::extract / insert), with vectors the generator derives from the interface
// definition: every signal at the low, high and middle raw value of its physical range, values
// just outside the range, NaN and wrong payload lengths. Complements the closed-loop SIL, which
// exercises only the messages and values a scenario reaches.

namespace adas::input_gate {
namespace {

std::uint64_t ref_extract(const std::uint8_t* data, std::size_t size, ea::codec::ByteOrder order, std::uint32_t start,
                          std::uint32_t length) {
    const ea::Result<std::uint64_t> r = ea::codec::extract(ea::ConstByteSpan(data, size), order, start, length);
    EXPECT_TRUE(r.ok());
    return r.ok() ? *r : 0U;
}

GateStatus gate_status_vector(int which) {
    GateStatus v{};
    v.state = which == 0 ? 0.0 : (which == 1 ? 3.0 : 1.0);
    v.qualifier = which == 0 ? 0.0 : (which == 1 ? 1.0 : 0.0);
    v.alive_counter = which == 0 ? 0.0 : (which == 1 ? 7.0 : 3.0);
    v.nearest_distance = which == 0 ? 0.0 : (which == 1 ? 250.0 : 125.0);
    v.e2e_error_count = which == 0 ? 0.0 : (which == 1 ? 255.0 : 127.0);
    return v;
}

TEST(AdasInputGateCodecTest, GateStatusLayoutMatchesReference) {
    for (int which = 0; which < 3; ++which) {
        std::array<std::uint8_t, kGateStatusPayloadLength> buf{};
        ASSERT_TRUE(encode_gate_status(gate_status_vector(which), ea::ByteSpan(buf.data(), buf.size())).ok());
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kBigEndian, 7U, 4U),
                  which == 0 ? 0ULL : (which == 1 ? 3ULL : 1ULL)) << "state";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kBigEndian, 3U, 1U),
                  which == 0 ? 0ULL : (which == 1 ? 1ULL : 0ULL)) << "qualifier";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kBigEndian, 2U, 3U),
                  which == 0 ? 0ULL : (which == 1 ? 7ULL : 3ULL)) << "alive_counter";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kBigEndian, 15U, 16U),
                  which == 0 ? 0ULL : (which == 1 ? 25000ULL : 12500ULL)) << "nearest_distance";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kBigEndian, 31U, 8U),
                  which == 0 ? 0ULL : (which == 1 ? 255ULL : 127ULL)) << "e2e_error_count";
        const ea::Result<GateStatus> d = decode_gate_status(ea::ConstByteSpan(buf.data(), buf.size()));
        ASSERT_TRUE(d.ok());
        EXPECT_DOUBLE_EQ(d->state, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 3LL : 1LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->qualifier, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 1LL : 0LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->alive_counter, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 7LL : 3LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->nearest_distance, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 25000LL : 12500LL)) * 0.01 + 0.0);
        EXPECT_DOUBLE_EQ(d->e2e_error_count, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 255LL : 127LL)) * 1.0 + 0.0);
    }
}

TEST(AdasInputGateCodecTest, GateStatusRejectsInvalidData) {
    std::array<std::uint8_t, kGateStatusPayloadLength> buf{};
    std::array<std::uint8_t, kGateStatusPayloadLength + 1U> longer{};
    EXPECT_EQ(encode_gate_status(gate_status_vector(2), ea::ByteSpan(longer.data(), longer.size())).error(), ea::ErrorCode::kLengthMismatch);
    EXPECT_EQ(decode_gate_status(ea::ConstByteSpan(buf.data(), buf.size() - 1U)).error(), ea::ErrorCode::kLengthMismatch);
    {
        GateStatus v = gate_status_vector(2);
        v.state = 4.0;
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.state = -1.0;
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.state = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        ASSERT_TRUE(encode_gate_status(gate_status_vector(2), ea::ByteSpan(buf.data(), buf.size())).ok());
        {
            std::array<std::uint8_t, kGateStatusPayloadLength> bad = buf;
            ASSERT_TRUE(ea::codec::insert(ea::ByteSpan(bad.data(), bad.size()), ea::codec::ByteOrder::kBigEndian, 7U, 4U, 4ULL).ok());
            EXPECT_EQ(decode_gate_status(ea::ConstByteSpan(bad.data(), bad.size())).error(), ea::ErrorCode::kOutOfRange) << "state";
        }
    }
    {
        GateStatus v = gate_status_vector(2);
        v.qualifier = 2.0;
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.qualifier = -1.0;
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.qualifier = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        GateStatus v = gate_status_vector(2);
        v.alive_counter = 8.0;
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.alive_counter = -1.0;
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.alive_counter = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        GateStatus v = gate_status_vector(2);
        v.nearest_distance = 250.01000000000002;
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.nearest_distance = -0.01;
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.nearest_distance = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        ASSERT_TRUE(encode_gate_status(gate_status_vector(2), ea::ByteSpan(buf.data(), buf.size())).ok());
        {
            std::array<std::uint8_t, kGateStatusPayloadLength> bad = buf;
            ASSERT_TRUE(ea::codec::insert(ea::ByteSpan(bad.data(), bad.size()), ea::codec::ByteOrder::kBigEndian, 15U, 16U, 25001ULL).ok());
            EXPECT_EQ(decode_gate_status(ea::ConstByteSpan(bad.data(), bad.size())).error(), ea::ErrorCode::kOutOfRange) << "nearest_distance";
        }
    }
    {
        GateStatus v = gate_status_vector(2);
        v.e2e_error_count = 256.0;
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.e2e_error_count = -1.0;
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.e2e_error_count = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_gate_status(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
}

ObjectSummary object_summary_vector(int which) {
    ObjectSummary v{};
    v.object_count = which == 0 ? 0.0 : (which == 1 ? 64.0 : 32.0);
    v.nearest_distance = which == 0 ? 0.0 : (which == 1 ? 250.0 : 125.0);
    v.nearest_rel_speed = which == 0 ? -80.0 : (which == 1 ? 80.0 : 0.0);
    v.nearest_confidence = which == 0 ? 0.0 : (which == 1 ? 100.0 : 50.0);
    return v;
}

TEST(AdasInputGateCodecTest, ObjectSummaryLayoutMatchesReference) {
    for (int which = 0; which < 3; ++which) {
        std::array<std::uint8_t, kObjectSummaryPayloadLength> buf{};
        ASSERT_TRUE(encode_object_summary(object_summary_vector(which), ea::ByteSpan(buf.data(), buf.size())).ok());
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kBigEndian, 103U, 8U),
                  which == 0 ? 0ULL : (which == 1 ? 64ULL : 32ULL)) << "object_count";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kBigEndian, 111U, 16U),
                  which == 0 ? 0ULL : (which == 1 ? 25000ULL : 12500ULL)) << "nearest_distance";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kBigEndian, 127U, 16U),
                  which == 0 ? 57536ULL : (which == 1 ? 8000ULL : 0ULL)) << "nearest_rel_speed";
        EXPECT_EQ(ref_extract(buf.data(), buf.size(), ea::codec::ByteOrder::kBigEndian, 143U, 8U),
                  which == 0 ? 0ULL : (which == 1 ? 200ULL : 100ULL)) << "nearest_confidence";
        const ea::Result<ObjectSummary> d = decode_object_summary(ea::ConstByteSpan(buf.data(), buf.size()));
        ASSERT_TRUE(d.ok());
        EXPECT_DOUBLE_EQ(d->object_count, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 64LL : 32LL)) * 1.0 + 0.0);
        EXPECT_DOUBLE_EQ(d->nearest_distance, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 25000LL : 12500LL)) * 0.01 + 0.0);
        EXPECT_DOUBLE_EQ(d->nearest_rel_speed, static_cast<double>(which == 0 ? -8000LL : (which == 1 ? 8000LL : 0LL)) * 0.01 + 0.0);
        EXPECT_DOUBLE_EQ(d->nearest_confidence, static_cast<double>(which == 0 ? 0LL : (which == 1 ? 200LL : 100LL)) * 0.5 + 0.0);
    }
}

TEST(AdasInputGateCodecTest, ObjectSummaryRejectsInvalidData) {
    std::array<std::uint8_t, kObjectSummaryPayloadLength> buf{};
    std::array<std::uint8_t, kObjectSummaryPayloadLength + 1U> longer{};
    EXPECT_EQ(encode_object_summary(object_summary_vector(2), ea::ByteSpan(longer.data(), longer.size())).error(), ea::ErrorCode::kLengthMismatch);
    EXPECT_EQ(decode_object_summary(ea::ConstByteSpan(buf.data(), buf.size() - 1U)).error(), ea::ErrorCode::kLengthMismatch);
    {
        ObjectSummary v = object_summary_vector(2);
        v.object_count = 65.0;
        EXPECT_EQ(encode_object_summary(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.object_count = -1.0;
        EXPECT_EQ(encode_object_summary(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.object_count = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_object_summary(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        ASSERT_TRUE(encode_object_summary(object_summary_vector(2), ea::ByteSpan(buf.data(), buf.size())).ok());
        {
            std::array<std::uint8_t, kObjectSummaryPayloadLength> bad = buf;
            ASSERT_TRUE(ea::codec::insert(ea::ByteSpan(bad.data(), bad.size()), ea::codec::ByteOrder::kBigEndian, 103U, 8U, 65ULL).ok());
            EXPECT_EQ(decode_object_summary(ea::ConstByteSpan(bad.data(), bad.size())).error(), ea::ErrorCode::kOutOfRange) << "object_count";
        }
    }
    {
        ObjectSummary v = object_summary_vector(2);
        v.nearest_distance = 250.01000000000002;
        EXPECT_EQ(encode_object_summary(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.nearest_distance = -0.01;
        EXPECT_EQ(encode_object_summary(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.nearest_distance = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_object_summary(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        ASSERT_TRUE(encode_object_summary(object_summary_vector(2), ea::ByteSpan(buf.data(), buf.size())).ok());
        {
            std::array<std::uint8_t, kObjectSummaryPayloadLength> bad = buf;
            ASSERT_TRUE(ea::codec::insert(ea::ByteSpan(bad.data(), bad.size()), ea::codec::ByteOrder::kBigEndian, 111U, 16U, 25001ULL).ok());
            EXPECT_EQ(decode_object_summary(ea::ConstByteSpan(bad.data(), bad.size())).error(), ea::ErrorCode::kOutOfRange) << "nearest_distance";
        }
    }
    {
        ObjectSummary v = object_summary_vector(2);
        v.nearest_rel_speed = 80.01;
        EXPECT_EQ(encode_object_summary(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.nearest_rel_speed = -80.01;
        EXPECT_EQ(encode_object_summary(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.nearest_rel_speed = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_object_summary(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        ASSERT_TRUE(encode_object_summary(object_summary_vector(2), ea::ByteSpan(buf.data(), buf.size())).ok());
        {
            std::array<std::uint8_t, kObjectSummaryPayloadLength> bad = buf;
            ASSERT_TRUE(ea::codec::insert(ea::ByteSpan(bad.data(), bad.size()), ea::codec::ByteOrder::kBigEndian, 127U, 16U, 8001ULL).ok());
            EXPECT_EQ(decode_object_summary(ea::ConstByteSpan(bad.data(), bad.size())).error(), ea::ErrorCode::kOutOfRange) << "nearest_rel_speed";
        }
        {
            std::array<std::uint8_t, kObjectSummaryPayloadLength> bad = buf;
            ASSERT_TRUE(ea::codec::insert(ea::ByteSpan(bad.data(), bad.size()), ea::codec::ByteOrder::kBigEndian, 127U, 16U, 57535ULL).ok());
            EXPECT_EQ(decode_object_summary(ea::ConstByteSpan(bad.data(), bad.size())).error(), ea::ErrorCode::kOutOfRange) << "nearest_rel_speed";
        }
    }
    {
        ObjectSummary v = object_summary_vector(2);
        v.nearest_confidence = 100.5;
        EXPECT_EQ(encode_object_summary(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.nearest_confidence = -0.5;
        EXPECT_EQ(encode_object_summary(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
        v.nearest_confidence = std::numeric_limits<double>::quiet_NaN();
        EXPECT_EQ(encode_object_summary(v, ea::ByteSpan(buf.data(), buf.size())).error(), ea::ErrorCode::kOutOfRange);
    }
    {
        ASSERT_TRUE(encode_object_summary(object_summary_vector(2), ea::ByteSpan(buf.data(), buf.size())).ok());
        {
            std::array<std::uint8_t, kObjectSummaryPayloadLength> bad = buf;
            ASSERT_TRUE(ea::codec::insert(ea::ByteSpan(bad.data(), bad.size()), ea::codec::ByteOrder::kBigEndian, 143U, 8U, 201ULL).ok());
            EXPECT_EQ(decode_object_summary(ea::ConstByteSpan(bad.data(), bad.size())).error(), ea::ErrorCode::kOutOfRange) << "nearest_confidence";
        }
    }
}

}  // namespace
}  // namespace adas::input_gate
