// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   adas_input_gate (pattern: input_gate)
// Inputs:    sha256:c2ae8363c4d06ee0cb2ee9ec885959083cf8629b1d677189a6ad2542626ab1f5
// Regenerate: python3 -m tools.gen

#include "adas_input_gate/adas_input_gate_codec.hpp"

#include <cmath>

#include "ea/codec/bits.hpp"

namespace adas::input_gate {

ea::Result<GateStatus> decode_gate_status(ea::ConstByteSpan payload) noexcept {
    if (payload.size() != kGateStatusPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    GateStatus out{};
    {
        // state: start bit 7, 4 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 3.0] 
        const ea::Result<std::uint64_t> raw = ea::codec::extract(payload, ea::codec::ByteOrder::kBigEndian, 7U, 4U);
        if (!raw.ok()) {
            return raw.error();
        }
        const std::int64_t value = static_cast<std::int64_t>(*raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 3LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.state = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // qualifier: start bit 3, 1 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        const ea::Result<std::uint64_t> raw = ea::codec::extract(payload, ea::codec::ByteOrder::kBigEndian, 3U, 1U);
        if (!raw.ok()) {
            return raw.error();
        }
        const std::int64_t value = static_cast<std::int64_t>(*raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.qualifier = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // alive_counter: start bit 2, 3 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 7.0] 
        const ea::Result<std::uint64_t> raw = ea::codec::extract(payload, ea::codec::ByteOrder::kBigEndian, 2U, 3U);
        if (!raw.ok()) {
            return raw.error();
        }
        const std::int64_t value = static_cast<std::int64_t>(*raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 7LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.alive_counter = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // nearest_distance: start bit 15, 16 bit, big endian, unsigned, factor 0.01, offset 0.0, range [0.0, 250.0] m
        const ea::Result<std::uint64_t> raw = ea::codec::extract(payload, ea::codec::ByteOrder::kBigEndian, 15U, 16U);
        if (!raw.ok()) {
            return raw.error();
        }
        const std::int64_t value = static_cast<std::int64_t>(*raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 25000LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.nearest_distance = static_cast<double>(value) * 0.01 + 0.0;
    }
    {
        // e2e_error_count: start bit 31, 8 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        const ea::Result<std::uint64_t> raw = ea::codec::extract(payload, ea::codec::ByteOrder::kBigEndian, 31U, 8U);
        if (!raw.ok()) {
            return raw.error();
        }
        const std::int64_t value = static_cast<std::int64_t>(*raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 255LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.e2e_error_count = static_cast<double>(value) * 1.0 + 0.0;
    }
    return out;
}

ea::Result<void> encode_gate_status(const GateStatus& in, ea::ByteSpan payload) noexcept {
    if (payload.size() != kGateStatusPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    {
        // state: start bit 7, 4 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 3.0] 
        const double scaled = (in.state - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 3.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = static_cast<std::int64_t>(std::llround(scaled));  // IPC-005
        if (value < 0LL || value > 3LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        EA_TRY(ea::codec::insert(payload, ea::codec::ByteOrder::kBigEndian, 7U, 4U, static_cast<std::uint64_t>(value)));
    }
    {
        // qualifier: start bit 3, 1 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        const double scaled = (in.qualifier - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 1.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = static_cast<std::int64_t>(std::llround(scaled));  // IPC-005
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        EA_TRY(ea::codec::insert(payload, ea::codec::ByteOrder::kBigEndian, 3U, 1U, static_cast<std::uint64_t>(value)));
    }
    {
        // alive_counter: start bit 2, 3 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 7.0] 
        const double scaled = (in.alive_counter - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 7.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = static_cast<std::int64_t>(std::llround(scaled));  // IPC-005
        if (value < 0LL || value > 7LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        EA_TRY(ea::codec::insert(payload, ea::codec::ByteOrder::kBigEndian, 2U, 3U, static_cast<std::uint64_t>(value)));
    }
    {
        // nearest_distance: start bit 15, 16 bit, big endian, unsigned, factor 0.01, offset 0.0, range [0.0, 250.0] m
        const double scaled = (in.nearest_distance - 0.0) / 0.01;
        if (!(scaled >= -0.5 && scaled <= 25000.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = static_cast<std::int64_t>(std::llround(scaled));  // IPC-005
        if (value < 0LL || value > 25000LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        EA_TRY(ea::codec::insert(payload, ea::codec::ByteOrder::kBigEndian, 15U, 16U, static_cast<std::uint64_t>(value)));
    }
    {
        // e2e_error_count: start bit 31, 8 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        const double scaled = (in.e2e_error_count - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 255.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = static_cast<std::int64_t>(std::llround(scaled));  // IPC-005
        if (value < 0LL || value > 255LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        EA_TRY(ea::codec::insert(payload, ea::codec::ByteOrder::kBigEndian, 31U, 8U, static_cast<std::uint64_t>(value)));
    }
    return ea::ok_result();
}

ea::Result<ObjectSummary> decode_object_summary(ea::ConstByteSpan payload) noexcept {
    if (payload.size() != kObjectSummaryPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    ObjectSummary out{};
    {
        // object_count: start bit 103, 8 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 64.0] 1
        const ea::Result<std::uint64_t> raw = ea::codec::extract(payload, ea::codec::ByteOrder::kBigEndian, 103U, 8U);
        if (!raw.ok()) {
            return raw.error();
        }
        const std::int64_t value = static_cast<std::int64_t>(*raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 64LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.object_count = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // nearest_distance: start bit 111, 16 bit, big endian, unsigned, factor 0.01, offset 0.0, range [0.0, 250.0] m
        const ea::Result<std::uint64_t> raw = ea::codec::extract(payload, ea::codec::ByteOrder::kBigEndian, 111U, 16U);
        if (!raw.ok()) {
            return raw.error();
        }
        const std::int64_t value = static_cast<std::int64_t>(*raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 25000LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.nearest_distance = static_cast<double>(value) * 0.01 + 0.0;
    }
    {
        // nearest_rel_speed: start bit 127, 16 bit, big endian, signed, factor 0.01, offset 0.0, range [-80.0, 80.0] m/s
        const ea::Result<std::uint64_t> raw = ea::codec::extract(payload, ea::codec::ByteOrder::kBigEndian, 127U, 16U);
        if (!raw.ok()) {
            return raw.error();
        }
        const std::int64_t value = ea::codec::sign_extend(*raw, 16U);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < -8000LL || value > 8000LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.nearest_rel_speed = static_cast<double>(value) * 0.01 + 0.0;
    }
    {
        // nearest_confidence: start bit 143, 8 bit, big endian, unsigned, factor 0.5, offset 0.0, range [0.0, 100.0] %
        const ea::Result<std::uint64_t> raw = ea::codec::extract(payload, ea::codec::ByteOrder::kBigEndian, 143U, 8U);
        if (!raw.ok()) {
            return raw.error();
        }
        const std::int64_t value = static_cast<std::int64_t>(*raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 200LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.nearest_confidence = static_cast<double>(value) * 0.5 + 0.0;
    }
    return out;
}

ea::Result<void> encode_object_summary(const ObjectSummary& in, ea::ByteSpan payload) noexcept {
    if (payload.size() != kObjectSummaryPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    {
        // object_count: start bit 103, 8 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 64.0] 1
        const double scaled = (in.object_count - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 64.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = static_cast<std::int64_t>(std::llround(scaled));  // IPC-005
        if (value < 0LL || value > 64LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        EA_TRY(ea::codec::insert(payload, ea::codec::ByteOrder::kBigEndian, 103U, 8U, static_cast<std::uint64_t>(value)));
    }
    {
        // nearest_distance: start bit 111, 16 bit, big endian, unsigned, factor 0.01, offset 0.0, range [0.0, 250.0] m
        const double scaled = (in.nearest_distance - 0.0) / 0.01;
        if (!(scaled >= -0.5 && scaled <= 25000.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = static_cast<std::int64_t>(std::llround(scaled));  // IPC-005
        if (value < 0LL || value > 25000LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        EA_TRY(ea::codec::insert(payload, ea::codec::ByteOrder::kBigEndian, 111U, 16U, static_cast<std::uint64_t>(value)));
    }
    {
        // nearest_rel_speed: start bit 127, 16 bit, big endian, signed, factor 0.01, offset 0.0, range [-80.0, 80.0] m/s
        const double scaled = (in.nearest_rel_speed - 0.0) / 0.01;
        if (!(scaled >= -8000.5 && scaled <= 8000.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = static_cast<std::int64_t>(std::llround(scaled));  // IPC-005
        if (value < -8000LL || value > 8000LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        const ea::Result<std::uint64_t> raw = ea::codec::to_twos_complement(value, 16U);
        if (!raw.ok()) {
            return raw.error();
        }
        EA_TRY(ea::codec::insert(payload, ea::codec::ByteOrder::kBigEndian, 127U, 16U, *raw));
    }
    {
        // nearest_confidence: start bit 143, 8 bit, big endian, unsigned, factor 0.5, offset 0.0, range [0.0, 100.0] %
        const double scaled = (in.nearest_confidence - 0.0) / 0.5;
        if (!(scaled >= -0.5 && scaled <= 200.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = static_cast<std::int64_t>(std::llround(scaled));  // IPC-005
        if (value < 0LL || value > 200LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        EA_TRY(ea::codec::insert(payload, ea::codec::ByteOrder::kBigEndian, 143U, 8U, static_cast<std::uint64_t>(value)));
    }
    return ea::ok_result();
}

}  // namespace adas::input_gate
