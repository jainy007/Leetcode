// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb (pattern: can_function)
// Inputs:    sha256:6606fd57f798610e5c596e91f29bec679e2b42414be7c909658cc37bce340b0d
// Regenerate: python3 -m tools.gen

#include "aeb/aeb_controller_codec.hpp"

#include <cmath>

#include "ea/codec/bits.hpp"
#include "ea/core/rounding.hpp"

namespace adas::aeb {

ea::Result<AebCommand> decode_aeb_command(ea::ConstByteSpan payload) noexcept {
    if (payload.size() != kAebCommandPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    AebCommand out{};
    {
        // crc: start bit 0, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 0U, 8U, kAebCommandPayloadLength), "GEN-031");
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
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 8U, 4U, kAebCommandPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 8U, 4U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 15LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.counter = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // aeb_status: start bit 12, 3 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 4.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 12U, 3U, kAebCommandPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 12U, 3U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 4LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.aeb_status = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // fcw_warning: start bit 15, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 15U, 1U, kAebCommandPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 15U, 1U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.fcw_warning = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // aeb_brake_request: start bit 16, 10 bit, little endian, unsigned, factor 0.01, offset 0.0, range [0.0, 10.0] m/s2
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 16U, 10U, kAebCommandPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 16U, 10U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 1000LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.aeb_brake_request = static_cast<double>(value) * 0.01 + 0.0;
    }
    return out;
}

ea::Result<void> encode_aeb_command(const AebCommand& in, ea::ByteSpan payload) noexcept {
    if (payload.size() != kAebCommandPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    {
        // crc: start bit 0, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 0U, 8U, kAebCommandPayloadLength), "GEN-031");
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
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 8U, 4U, kAebCommandPayloadLength), "GEN-031");
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
        // aeb_status: start bit 12, 3 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 4.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 12U, 3U, kAebCommandPayloadLength), "GEN-031");
        const double scaled = (in.aeb_status - 0.0) / 1.0;
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
        // fcw_warning: start bit 15, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 15U, 1U, kAebCommandPayloadLength), "GEN-031");
        const double scaled = (in.fcw_warning - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 1.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 1] lies inside the 1-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 15U, 1U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // aeb_brake_request: start bit 16, 10 bit, little endian, unsigned, factor 0.01, offset 0.0, range [0.0, 10.0] m/s2
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 16U, 10U, kAebCommandPayloadLength), "GEN-031");
        const double scaled = (in.aeb_brake_request - 0.0) / 0.01;
        if (!(scaled >= -0.5 && scaled <= 1000.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 1000LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 1000] lies inside the 10-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 16U, 10U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    return ea::ok_result();
}

ea::Result<DriverInput> decode_driver_input(ea::ConstByteSpan payload) noexcept {
    if (payload.size() != kDriverInputPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    DriverInput out{};
    {
        // crc: start bit 0, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 0U, 8U, kDriverInputPayloadLength), "GEN-031");
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
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 8U, 4U, kDriverInputPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 8U, 4U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 15LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.counter = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // main_switch: start bit 12, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 12U, 1U, kDriverInputPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 12U, 1U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.main_switch = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // set_button: start bit 13, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 13U, 1U, kDriverInputPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 13U, 1U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.set_button = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // resume_button: start bit 14, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 14U, 1U, kDriverInputPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 14U, 1U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.resume_button = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // cancel_button: start bit 15, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 15U, 1U, kDriverInputPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 15U, 1U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.cancel_button = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // speed_up_button: start bit 16, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 16U, 1U, kDriverInputPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 16U, 1U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.speed_up_button = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // speed_down_button: start bit 17, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 17U, 1U, kDriverInputPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 17U, 1U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.speed_down_button = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // gap_setting: start bit 18, 2 bit, little endian, unsigned, factor 1.0, offset 0.0, range [1.0, 3.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 18U, 2U, kDriverInputPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 18U, 2U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 1LL || value > 3LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.gap_setting = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // brake_pedal: start bit 20, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 20U, 1U, kDriverInputPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 20U, 1U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.brake_pedal = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // accel_pedal_pos: start bit 24, 8 bit, little endian, unsigned, factor 0.5, offset 0.0, range [0.0, 100.0] %
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 24U, 8U, kDriverInputPayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 24U, 8U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 200LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.accel_pedal_pos = static_cast<double>(value) * 0.5 + 0.0;
    }
    return out;
}

ea::Result<void> encode_driver_input(const DriverInput& in, ea::ByteSpan payload) noexcept {
    if (payload.size() != kDriverInputPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    {
        // crc: start bit 0, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 0U, 8U, kDriverInputPayloadLength), "GEN-031");
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
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 8U, 4U, kDriverInputPayloadLength), "GEN-031");
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
        // main_switch: start bit 12, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 12U, 1U, kDriverInputPayloadLength), "GEN-031");
        const double scaled = (in.main_switch - 0.0) / 1.0;
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
        // set_button: start bit 13, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 13U, 1U, kDriverInputPayloadLength), "GEN-031");
        const double scaled = (in.set_button - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 1.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 1] lies inside the 1-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 13U, 1U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // resume_button: start bit 14, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 14U, 1U, kDriverInputPayloadLength), "GEN-031");
        const double scaled = (in.resume_button - 0.0) / 1.0;
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
        // cancel_button: start bit 15, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 15U, 1U, kDriverInputPayloadLength), "GEN-031");
        const double scaled = (in.cancel_button - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 1.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 1] lies inside the 1-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 15U, 1U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // speed_up_button: start bit 16, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 16U, 1U, kDriverInputPayloadLength), "GEN-031");
        const double scaled = (in.speed_up_button - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 1.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 1] lies inside the 1-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 16U, 1U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // speed_down_button: start bit 17, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 17U, 1U, kDriverInputPayloadLength), "GEN-031");
        const double scaled = (in.speed_down_button - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 1.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 1] lies inside the 1-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 17U, 1U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // gap_setting: start bit 18, 2 bit, little endian, unsigned, factor 1.0, offset 0.0, range [1.0, 3.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 18U, 2U, kDriverInputPayloadLength), "GEN-031");
        const double scaled = (in.gap_setting - 0.0) / 1.0;
        if (!(scaled >= 0.5 && scaled <= 3.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 1LL || value > 3LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [1, 3] lies inside the 2-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 18U, 2U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // brake_pedal: start bit 20, 1 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 1.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 20U, 1U, kDriverInputPayloadLength), "GEN-031");
        const double scaled = (in.brake_pedal - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 1.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 1LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 1] lies inside the 1-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 20U, 1U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // accel_pedal_pos: start bit 24, 8 bit, little endian, unsigned, factor 0.5, offset 0.0, range [0.0, 100.0] %
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 24U, 8U, kDriverInputPayloadLength), "GEN-031");
        const double scaled = (in.accel_pedal_pos - 0.0) / 0.5;
        if (!(scaled >= -0.5 && scaled <= 200.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 200LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 200] lies inside the 8-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 24U, 8U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    return ea::ok_result();
}

ea::Result<EgoState> decode_ego_state(ea::ConstByteSpan payload) noexcept {
    if (payload.size() != kEgoStatePayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    EgoState out{};
    {
        // crc: start bit 0, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 0U, 8U, kEgoStatePayloadLength), "GEN-031");
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
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 8U, 4U, kEgoStatePayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 8U, 4U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 15LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.counter = static_cast<double>(value) * 1.0 + 0.0;
    }
    {
        // vehicle_speed: start bit 16, 16 bit, little endian, unsigned, factor 0.01, offset 0.0, range [0.0, 655.35] km/h
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 16U, 16U, kEgoStatePayloadLength), "GEN-031");
        const std::uint64_t raw = ea::codec::extract_fixed<ea::codec::ByteOrder::kLittleEndian, 16U, 16U>(payload.data());
        const std::int64_t value = static_cast<std::int64_t>(raw);
        // IPC-002: physical range, checked exactly in the raw domain.
        if (value < 0LL || value > 65535LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        out.vehicle_speed = static_cast<double>(value) * 0.01 + 0.0;
    }
    return out;
}

ea::Result<void> encode_ego_state(const EgoState& in, ea::ByteSpan payload) noexcept {
    if (payload.size() != kEgoStatePayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    {
        // crc: start bit 0, 8 bit, little endian, unsigned, factor 1.0, offset 0.0, range [0.0, 255.0] 
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 0U, 8U, kEgoStatePayloadLength), "GEN-031");
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
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 8U, 4U, kEgoStatePayloadLength), "GEN-031");
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
        // vehicle_speed: start bit 16, 16 bit, little endian, unsigned, factor 0.01, offset 0.0, range [0.0, 655.35] km/h
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kLittleEndian, 16U, 16U, kEgoStatePayloadLength), "GEN-031");
        const double scaled = (in.vehicle_speed - 0.0) / 0.01;
        if (!(scaled >= -0.5 && scaled <= 65535.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 65535LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 65535] lies inside the 16-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kLittleEndian, 16U, 16U>(payload.data(), static_cast<std::uint64_t>(value));
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

}  // namespace adas::aeb
