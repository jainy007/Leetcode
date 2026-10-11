// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   adas_input_gate (pattern: input_gate)
// Inputs:    sha256:6b014bf1c6088b1ee8b025ad45997456f3661570e6db76f02157c9ff08730979
// Regenerate: python3 -m tools.gen

#include "adas_input_gate/adas_input_gate_codec.hpp"

#include <cmath>

#include "ea/codec/bits.hpp"
#include "ea/core/rounding.hpp"

namespace adas::input_gate {

ea::Result<GateStatus> decode_gate_status(ea::ConstByteSpan payload) noexcept {
    if (payload.size() != kGateStatusPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    GateStatus out{};
    {
        // state: start bit 7, 4 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 3.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 7U, 4U, kGateStatusPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kBigEndian, 7U, 4U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain (only bounds the field can exceed).
        if (value > 3LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.state = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // qualifier: start bit 3, 1 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 3U, 1U, kGateStatusPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kBigEndian, 3U, 1U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: the physical range spans the whole 1-bit field, every raw value is valid.
        out.qualifier = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // alive_counter: start bit 2, 3 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 7.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 2U, 3U, kGateStatusPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kBigEndian, 2U, 3U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: the physical range spans the whole 3-bit field, every raw value is valid.
        out.alive_counter = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // nearest_distance: start bit 15, 16 bit, big endian, unsigned, factor 0.01, offset 0.0, range [0.0, 250.0] m
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 15U, 16U, kGateStatusPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kBigEndian, 15U, 16U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain (only bounds the field can exceed).
        if (value > 25000LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.nearest_distance = static_cast<double>(value) * 0.01 + 0.0;
    }
    {
        // e2e_error_count: start bit 31, 8 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 31U, 8U, kGateStatusPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kBigEndian, 31U, 8U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: the physical range spans the whole 8-bit field, every raw value is valid.
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
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 7U, 4U, kGateStatusPayloadLength), "GEN-031");
        const double scaled = (in.state - 0.0) / 1.0;
        // Open interval: every value in it rounds (half away from zero) into [0, 3].
        if (!(scaled > -0.5 && scaled < 3.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        // [0, 3] lies inside the 4-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kBigEndian, 7U, 4U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // qualifier: start bit 3, 1 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 3U, 1U, kGateStatusPayloadLength), "GEN-031");
        const double scaled = (in.qualifier - 0.0) / 1.0;
        // Open interval: every value in it rounds (half away from zero) into [0, 1].
        if (!(scaled > -0.5 && scaled < 1.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        // [0, 1] lies inside the 1-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kBigEndian, 3U, 1U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // alive_counter: start bit 2, 3 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 7.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 2U, 3U, kGateStatusPayloadLength), "GEN-031");
        const double scaled = (in.alive_counter - 0.0) / 1.0;
        // Open interval: every value in it rounds (half away from zero) into [0, 7].
        if (!(scaled > -0.5 && scaled < 7.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        // [0, 7] lies inside the 3-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kBigEndian, 2U, 3U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // nearest_distance: start bit 15, 16 bit, big endian, unsigned, factor 0.01, offset 0.0, range [0.0, 250.0] m
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 15U, 16U, kGateStatusPayloadLength), "GEN-031");
        const double scaled = (in.nearest_distance - 0.0) / 0.01;
        // Open interval: every value in it rounds (half away from zero) into [0, 25000].
        if (!(scaled > -0.5 && scaled < 25000.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        // [0, 25000] lies inside the 16-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kBigEndian, 15U, 16U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // e2e_error_count: start bit 31, 8 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 31U, 8U, kGateStatusPayloadLength), "GEN-031");
        const double scaled = (in.e2e_error_count - 0.0) / 1.0;
        // Open interval: every value in it rounds (half away from zero) into [0, 255].
        if (!(scaled > -0.5 && scaled < 255.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        // [0, 255] lies inside the 8-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kBigEndian, 31U, 8U>(payload.data(), static_cast<std::uint64_t>(value));
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
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 103U, 8U, kObjectSummaryPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kBigEndian, 103U, 8U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain (only bounds the field can exceed).
        if (value > 64LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.object_count = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // nearest_distance: start bit 111, 16 bit, big endian, unsigned, factor 0.01, offset 0.0, range [0.0, 250.0] m
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 111U, 16U, kObjectSummaryPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kBigEndian, 111U, 16U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain (only bounds the field can exceed).
        if (value > 25000LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.nearest_distance = static_cast<double>(value) * 0.01 + 0.0;
    }
    {
        // nearest_rel_speed: start bit 127, 16 bit, big endian, signed, factor 0.01, offset 0.0, range [-80.0, 80.0] m/s
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 127U, 16U, kObjectSummaryPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kBigEndian, 127U, 16U>(payload.data());
        const std::int64_t value = ea::codec::sign_extend(raw, 16U);
        // IPC-002: physical range, checked exactly in the raw domain (only bounds the field can exceed).
        if (value < -8000LL || value > 8000LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.nearest_rel_speed = static_cast<double>(value) * 0.01 + 0.0;
    }
    {
        // nearest_confidence: start bit 143, 8 bit, big endian, unsigned, factor 0.5, offset 0.0, range [0.0, 100.0] %
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 143U, 8U, kObjectSummaryPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kBigEndian, 143U, 8U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain (only bounds the field can exceed).
        if (value > 200LL) {
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
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 103U, 8U, kObjectSummaryPayloadLength), "GEN-031");
        const double scaled = (in.object_count - 0.0) / 1.0;
        // Open interval: every value in it rounds (half away from zero) into [0, 64].
        if (!(scaled > -0.5 && scaled < 64.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        // [0, 64] lies inside the 8-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kBigEndian, 103U, 8U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // nearest_distance: start bit 111, 16 bit, big endian, unsigned, factor 0.01, offset 0.0, range [0.0, 250.0] m
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 111U, 16U, kObjectSummaryPayloadLength), "GEN-031");
        const double scaled = (in.nearest_distance - 0.0) / 0.01;
        // Open interval: every value in it rounds (half away from zero) into [0, 25000].
        if (!(scaled > -0.5 && scaled < 25000.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        // [0, 25000] lies inside the 16-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kBigEndian, 111U, 16U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // nearest_rel_speed: start bit 127, 16 bit, big endian, signed, factor 0.01, offset 0.0, range [-80.0, 80.0] m/s
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 127U, 16U, kObjectSummaryPayloadLength), "GEN-031");
        const double scaled = (in.nearest_rel_speed - 0.0) / 0.01;
        // Open interval: every value in it rounds (half away from zero) into [-8000, 8000].
        if (!(scaled > -8000.5 && scaled < 8000.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        // [-8000, 8000] lies inside the 16-bit signed range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kBigEndian, 127U, 16U>(payload.data(), static_cast<std::uint64_t>(value) & 0xFFFFULL);
    }
    {
        // nearest_confidence: start bit 143, 8 bit, big endian, unsigned, factor 0.5, offset 0.0, range [0.0, 100.0] %
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 143U, 8U, kObjectSummaryPayloadLength), "GEN-031");
        const double scaled = (in.nearest_confidence - 0.0) / 0.5;
        // Open interval: every value in it rounds (half away from zero) into [0, 200].
        if (!(scaled > -0.5 && scaled < 200.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        // [0, 200] lies inside the 8-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kBigEndian, 143U, 8U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    return ea::ok_result();
}

}  // namespace adas::input_gate
