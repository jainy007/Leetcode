// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc_monitor (pattern: can_function)
// Inputs:    sha256:fd01fad24aac30e6912b9e2458a96cc1c9827a4af69c82078bba92de97606ff7
// Regenerate: python3 -m tools.gen

#include "acc_monitor/acc_monitor_codec.hpp"

#include <cmath>

#include "ea/codec/bits.hpp"
#include "ea/core/rounding.hpp"

namespace adas::acc_monitor {

ea::Result<AccCommand> decode_acc_command(ea::ConstByteSpan payload) noexcept {
    if (payload.size() != kAccCommandPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    AccCommand out{};
    {
        // crc: start bit 0, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 0U, 8U, kAccCommandPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 0U, 8U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 255LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.crc = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // counter: start bit 8, 4 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 15.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 8U, 4U, kAccCommandPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 8U, 4U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 15LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.counter = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // acc_status: start bit 12, 3 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 4.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 12U, 3U, kAccCommandPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 12U, 3U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 4LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.acc_status = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // accel_request: start bit 16, 10 bit, little endian, unsigned, factor 0.01, offset 0.0, range [0.0, 5.0] m/s2
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 16U, 10U, kAccCommandPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 16U, 10U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 500LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.accel_request = static_cast<double>(value) * 0.01 + 0.0;
    }
    {
        // brake_request: start bit 26, 10 bit, little endian, unsigned, factor 0.01, offset 0.0, range [0.0, 10.0] m/s2
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 26U, 10U, kAccCommandPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 26U, 10U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 1000LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.brake_request = static_cast<double>(value) * 0.01 + 0.0;
    }
    {
        // steering_angle_request: start bit 36, 12 bit, little endian, signed, factor 0.1, offset 0.0, range [-180.0, 180.0] deg
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 36U, 12U, kAccCommandPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 36U, 12U>(payload.data());
        const std::int64_t value = ea::codec::sign_extend(raw, 12U);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < -1800LL || value > 1800LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.steering_angle_request = static_cast<double>(value) * 0.1 + 0.0;
    }
    {
        // set_speed: start bit 48, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 250.0] km/h
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 48U, 8U, kAccCommandPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 48U, 8U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 250LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.set_speed = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // time_gap_setting: start bit 56, 2 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 3.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 56U, 2U, kAccCommandPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 56U, 2U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 3LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.time_gap_setting = static_cast<double>(value) * 1.0 + 0.0;
    }
    return out;
}

ea::Result<void> encode_acc_command(const AccCommand& in, ea::ByteSpan payload) noexcept {
    if (payload.size() != kAccCommandPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    {
        // crc: start bit 0, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 0U, 8U, kAccCommandPayloadLength), "GEN-031");
        const double scaled = (in.crc - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 255.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 255LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 255] lies inside the 8-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 0U, 8U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // counter: start bit 8, 4 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 15.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 8U, 4U, kAccCommandPayloadLength), "GEN-031");
        const double scaled = (in.counter - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 15.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 15LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 15] lies inside the 4-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 8U, 4U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // acc_status: start bit 12, 3 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 4.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 12U, 3U, kAccCommandPayloadLength), "GEN-031");
        const double scaled = (in.acc_status - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 4.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 4LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 4] lies inside the 3-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 12U, 3U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // accel_request: start bit 16, 10 bit, little endian, unsigned, factor 0.01, offset 0.0, range [0.0, 5.0] m/s2
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 16U, 10U, kAccCommandPayloadLength), "GEN-031");
        const double scaled = (in.accel_request - 0.0) / 0.01;
        if (!(scaled >= -0.5 && scaled <= 500.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 500LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 500] lies inside the 10-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 16U, 10U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // brake_request: start bit 26, 10 bit, little endian, unsigned, factor 0.01, offset 0.0, range [0.0, 10.0] m/s2
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 26U, 10U, kAccCommandPayloadLength), "GEN-031");
        const double scaled = (in.brake_request - 0.0) / 0.01;
        if (!(scaled >= -0.5 && scaled <= 1000.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 1000LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 1000] lies inside the 10-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 26U, 10U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // steering_angle_request: start bit 36, 12 bit, little endian, signed, factor 0.1, offset 0.0, range [-180.0, 180.0] deg
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 36U, 12U, kAccCommandPayloadLength), "GEN-031");
        const double scaled = (in.steering_angle_request - 0.0) / 0.1;
        if (!(scaled >= -1800.5 && scaled <= 1800.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < -1800LL || value > 1800LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [-1800, 1800] lies inside the 12-bit signed range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 36U, 12U>(payload.data(), static_cast<std::uint64_t>(value) & 0xFFFULL);
    }
    {
        // set_speed: start bit 48, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 250.0] km/h
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 48U, 8U, kAccCommandPayloadLength), "GEN-031");
        const double scaled = (in.set_speed - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 250.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 250LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 250] lies inside the 8-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 48U, 8U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // time_gap_setting: start bit 56, 2 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 3.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 56U, 2U, kAccCommandPayloadLength), "GEN-031");
        const double scaled = (in.time_gap_setting - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 3.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 3LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 3] lies inside the 2-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 56U, 2U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    return ea::ok_result();
}

ea::Result<AccMonitorStatus> decode_acc_monitor_status(ea::ConstByteSpan payload) noexcept {
    if (payload.size() != kAccMonitorStatusPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    AccMonitorStatus out{};
    {
        // crc: start bit 0, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 0U, 8U, kAccMonitorStatusPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 0U, 8U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 255LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.crc = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // counter: start bit 8, 4 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 15.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 8U, 4U, kAccMonitorStatusPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 8U, 4U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 15LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.counter = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // mon_status: start bit 12, 2 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 2.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 12U, 2U, kAccMonitorStatusPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 12U, 2U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 2LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.mon_status = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // permit: start bit 14, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 14U, 1U, kAccMonitorStatusPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 14U, 1U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.permit = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // approved_counter: start bit 16, 4 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 15.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 16U, 4U, kAccMonitorStatusPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 16U, 4U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 15LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.approved_counter = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // fault_reason: start bit 20, 4 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 15.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 20U, 4U, kAccMonitorStatusPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 20U, 4U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 15LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.fault_reason = static_cast<double>(value) * 1.0 + 0.0;
    }
    return out;
}

ea::Result<void> encode_acc_monitor_status(const AccMonitorStatus& in, ea::ByteSpan payload) noexcept {
    if (payload.size() != kAccMonitorStatusPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    {
        // crc: start bit 0, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 0U, 8U, kAccMonitorStatusPayloadLength), "GEN-031");
        const double scaled = (in.crc - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 255.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 255LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 255] lies inside the 8-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 0U, 8U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // counter: start bit 8, 4 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 15.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 8U, 4U, kAccMonitorStatusPayloadLength), "GEN-031");
        const double scaled = (in.counter - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 15.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 15LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 15] lies inside the 4-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 8U, 4U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // mon_status: start bit 12, 2 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 2.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 12U, 2U, kAccMonitorStatusPayloadLength), "GEN-031");
        const double scaled = (in.mon_status - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 2.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 2LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 2] lies inside the 2-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 12U, 2U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // permit: start bit 14, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 14U, 1U, kAccMonitorStatusPayloadLength), "GEN-031");
        const double scaled = (in.permit - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 1.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 1] lies inside the 1-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 14U, 1U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // approved_counter: start bit 16, 4 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 15.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 16U, 4U, kAccMonitorStatusPayloadLength), "GEN-031");
        const double scaled = (in.approved_counter - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 15.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 15LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 15] lies inside the 4-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 16U, 4U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // fault_reason: start bit 20, 4 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 15.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 20U, 4U, kAccMonitorStatusPayloadLength), "GEN-031");
        const double scaled = (in.fault_reason - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 15.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 15LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 15] lies inside the 4-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 20U, 4U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    return ea::ok_result();
}

ea::Result<RadarObject> decode_radar_object(ea::ConstByteSpan payload) noexcept {
    if (payload.size() != kRadarObjectPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    RadarObject out{};
    {
        // crc: start bit 0, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 0U, 8U, kRadarObjectPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 0U, 8U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 255LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.crc = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // counter: start bit 8, 4 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 15.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 8U, 4U, kRadarObjectPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 8U, 4U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 15LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.counter = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // obj_valid: start bit 12, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 12U, 1U, kRadarObjectPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 12U, 1U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.obj_valid = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // obj_range: start bit 16, 12 bit, little endian, unsigned, factor 0.1, offset 0.0, range [0.0, 409.5] m
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 16U, 12U, kRadarObjectPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 16U, 12U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 4095LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.obj_range = static_cast<double>(value) * 0.1 + 0.0;
    }
    {
        // obj_range_rate: start bit 28, 12 bit, little endian, signed, factor 0.05, offset 0.0, range [-102.4, 102.35] m/s
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 28U, 12U, kRadarObjectPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 28U, 12U>(payload.data());
        const std::int64_t value = ea::codec::sign_extend(raw, 12U);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < -2048LL || value > 2047LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.obj_range_rate = static_cast<double>(value) * 0.05 + 0.0;
    }
    {
        // obj_lateral_pos: start bit 40, 10 bit, little endian, signed, factor 0.02, offset 0.0, range [-10.24, 10.22] m
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 40U, 10U, kRadarObjectPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 40U, 10U>(payload.data());
        const std::int64_t value = ea::codec::sign_extend(raw, 10U);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < -512LL || value > 511LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.obj_lateral_pos = static_cast<double>(value) * 0.02 + 0.0;
    }
    return out;
}

ea::Result<void> encode_radar_object(const RadarObject& in, ea::ByteSpan payload) noexcept {
    if (payload.size() != kRadarObjectPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    {
        // crc: start bit 0, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 0U, 8U, kRadarObjectPayloadLength), "GEN-031");
        const double scaled = (in.crc - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 255.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 255LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 255] lies inside the 8-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 0U, 8U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // counter: start bit 8, 4 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 15.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 8U, 4U, kRadarObjectPayloadLength), "GEN-031");
        const double scaled = (in.counter - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 15.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 15LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 15] lies inside the 4-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 8U, 4U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // obj_valid: start bit 12, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 12U, 1U, kRadarObjectPayloadLength), "GEN-031");
        const double scaled = (in.obj_valid - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 1.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 1] lies inside the 1-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 12U, 1U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // obj_range: start bit 16, 12 bit, little endian, unsigned, factor 0.1, offset 0.0, range [0.0, 409.5] m
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 16U, 12U, kRadarObjectPayloadLength), "GEN-031");
        const double scaled = (in.obj_range - 0.0) / 0.1;
        if (!(scaled >= -0.5 && scaled <= 4095.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 4095LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 4095] lies inside the 12-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 16U, 12U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // obj_range_rate: start bit 28, 12 bit, little endian, signed, factor 0.05, offset 0.0, range [-102.4, 102.35] m/s
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 28U, 12U, kRadarObjectPayloadLength), "GEN-031");
        const double scaled = (in.obj_range_rate - 0.0) / 0.05;
        if (!(scaled >= -2048.5 && scaled <= 2047.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < -2048LL || value > 2047LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [-2048, 2047] lies inside the 12-bit signed range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 28U, 12U>(payload.data(), static_cast<std::uint64_t>(value) & 0xFFFULL);
    }
    {
        // obj_lateral_pos: start bit 40, 10 bit, little endian, signed, factor 0.02, offset 0.0, range [-10.24, 10.22] m
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 40U, 10U, kRadarObjectPayloadLength), "GEN-031");
        const double scaled = (in.obj_lateral_pos - 0.0) / 0.02;
        if (!(scaled >= -512.5 && scaled <= 511.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < -512LL || value > 511LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [-512, 511] lies inside the 10-bit signed range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 40U, 10U>(payload.data(), static_cast<std::uint64_t>(value) & 0x3FFULL);
    }
    return ea::ok_result();
}

}  // namespace adas::acc_monitor
