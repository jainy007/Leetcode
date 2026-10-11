// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   adas_input_gate (pattern: input_gate)
// Inputs:    sha256:40eb09e64e978032abfa2454beacdfe360c3791c60f99448b683e92325260385
// Regenerate: python3 -m tools.gen

#pragma once

#include <array>
#include <cmath>
#include <cstdint>

#include "ea/codec/bits.hpp"
#include "ea/codec/someip.hpp"
#include "ea/core/assert.hpp"
#include "ea/core/rounding.hpp"
#include "ea/e2e/p4.hpp"
#include "adas_input_gate/adas_input_gate.hpp"

namespace adas::input_gate {

// Test-only encoder: accepts values outside the physical range (to inject range faults) but
// never outside what the raw signal can carry. Not part of the production library.
inline ea::Result<void> encode_object_summary_unchecked(const ObjectSummary& in, ea::ByteSpan payload) noexcept {
    if (payload.size() != kObjectSummaryPayloadLength) {
        return ea::ErrorCode::kLengthMismatch;
    }
    {
        // object_count: start bit 103, 8 bit, big endian, unsigned, factor 1.0, offset 0.0, range [0.0, 64.0] 1
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 103U, 8U, kObjectSummaryPayloadLength), "GEN-031");
        const double scaled = (in.object_count - 0.0) / 1.0;
        if (!(scaled >= -0.5 && scaled <= 255.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 255LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 255] lies inside the 8-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kBigEndian, 103U, 8U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // nearest_distance: start bit 111, 16 bit, big endian, unsigned, factor 0.01, offset 0.0, range [0.0, 250.0] m
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 111U, 16U, kObjectSummaryPayloadLength), "GEN-031");
        const double scaled = (in.nearest_distance - 0.0) / 0.01;
        if (!(scaled >= -0.5 && scaled <= 65535.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 65535LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 65535] lies inside the 16-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kBigEndian, 111U, 16U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    {
        // nearest_rel_speed: start bit 127, 16 bit, big endian, signed, factor 0.01, offset 0.0, range [-80.0, 80.0] m/s
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 127U, 16U, kObjectSummaryPayloadLength), "GEN-031");
        const double scaled = (in.nearest_rel_speed - 0.0) / 0.01;
        if (!(scaled >= -32768.5 && scaled <= 32767.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < -32768LL || value > 32767LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [-32768, 32767] lies inside the 16-bit signed range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kBigEndian, 127U, 16U>(payload.data(), static_cast<std::uint64_t>(value) & 0xFFFFULL);
    }
    {
        // nearest_confidence: start bit 143, 8 bit, big endian, unsigned, factor 0.5, offset 0.0, range [0.0, 100.0] %
        static_assert(ea::codec::signal_fits(ea::codec::ByteOrder::kBigEndian, 143U, 8U, kObjectSummaryPayloadLength), "GEN-031");
        const double scaled = (in.nearest_confidence - 0.0) / 0.5;
        if (!(scaled >= -0.5 && scaled <= 255.5)) {  // also rejects NaN
            return ea::ErrorCode::kOutOfRange;
        }
        const std::int64_t value = ea::round_half_away(scaled);  // IPC-005, equals std::llround here
        if (value < 0LL || value > 255LL) {
            return ea::ErrorCode::kOutOfRange;
        }
        // [0, 255] lies inside the 8-bit unsigned range (GEN-033), so the raw value fits.
        ea::codec::insert_fixed<ea::codec::ByteOrder::kBigEndian, 143U, 8U>(payload.data(), static_cast<std::uint64_t>(value));
    }
    return ea::ok_result();
}

// SIL harness for AdasInputGate: builds SOME/IP + E2E frames from the model and drives one
// step() per simulated cycle.
class AdasInputGateSil {
public:
    static constexpr std::size_t kFrameSize = ea::someip::kHeaderSize + kObjectSummaryPayloadLength;
    using Frame = std::array<std::uint8_t, kFrameSize>;
    // First byte of the E2E CRC field inside the frame.
    static constexpr std::size_t kCrcByte = ea::someip::kHeaderSize + 0U + 8U;

    AdasInputGateSil() noexcept : protector_(kObjectSummaryE2e) {}

    void receive_valid(int n) noexcept {
        for (int i = 0; i < n; ++i) {
            send(build(defaults()));
        }
    }
    void receive_corrupted_crc(int n) noexcept {
        for (int i = 0; i < n; ++i) {
            Frame f = build(defaults());
            f[kCrcByte] = static_cast<std::uint8_t>(f[kCrcByte] ^ 0x01U);
            send(f);
        }
    }
    void receive_repeated_counter(int n) noexcept {
        if (!has_last_) {
            send(build(defaults()));
        }
        const Frame repeated = last_;
        for (int i = 0; i < n; ++i) {
            send(repeated);
        }
    }
    void receive_with(Signal s, double value) noexcept {
        ObjectSummary v = defaults();
        set(v, s, value);
        send(build(v));
    }
    void miss(int n) noexcept {
        for (int i = 0; i < n; ++i) {
            component_.step(ea::ConstByteSpan());
        }
    }

    State state() const noexcept { return component_.state(); }
    Qualifier qualifier() const noexcept { return component_.output().qualifier; }
    double output_signal(Signal s) const noexcept { return get(component_.output().data, s); }
    std::uint64_t heartbeat() const noexcept { return component_.heartbeat().count(); }
    State status_state() const noexcept {
        return static_cast<State>(static_cast<std::uint8_t>(status_signal(StatusSignal::kState)));
    }
    double status_signal(StatusSignal s) const noexcept {
        const ea::Result<GateStatus> msg = decode_gate_status(component_.status_payload());
        EA_ASSERT(msg.ok());
        switch (s) {
            case StatusSignal::kState:
                return msg->state;
            case StatusSignal::kQualifier:
                return msg->qualifier;
            case StatusSignal::kAliveCounter:
                return msg->alive_counter;
            case StatusSignal::kNearestDistance:
                return msg->nearest_distance;
            case StatusSignal::kE2eErrorCount:
                return msg->e2e_error_count;
        }
        return 0.0;
    }
    const AdasInputGate& component() const noexcept { return component_; }

private:
    static ObjectSummary defaults() noexcept {
        ObjectSummary v{};
        v.object_count = 3.0;
        v.nearest_distance = 42.5;
        v.nearest_rel_speed = -3.25;
        v.nearest_confidence = 87.5;
        return v;
    }
    static void set(ObjectSummary& v, Signal s, double value) noexcept {
        switch (s) {
            case Signal::kObjectCount:
                v.object_count = value;
                break;
            case Signal::kNearestDistance:
                v.nearest_distance = value;
                break;
            case Signal::kNearestRelSpeed:
                v.nearest_rel_speed = value;
                break;
            case Signal::kNearestConfidence:
                v.nearest_confidence = value;
                break;
        }
    }
    static double get(const ObjectSummary& v, Signal s) noexcept {
        switch (s) {
            case Signal::kObjectCount:
                return v.object_count;
            case Signal::kNearestDistance:
                return v.nearest_distance;
            case Signal::kNearestRelSpeed:
                return v.nearest_rel_speed;
            case Signal::kNearestConfidence:
                return v.nearest_confidence;
        }
        return 0.0;
    }
    Frame build(const ObjectSummary& v) noexcept {
        Frame f{};
        const ea::ByteSpan payload(f.data() + ea::someip::kHeaderSize, kObjectSummaryPayloadLength);
        const ea::Result<void> encoded = encode_object_summary_unchecked(v, payload);
        EA_ASSERT(encoded.ok());
        const ea::Result<void> protected_ok = protector_.protect(payload);
        EA_ASSERT(protected_ok.ok());
        const ea::Result<void> header = ea::someip::write_header(f, kObjectSummarySomeIp, session_, payload.size());
        EA_ASSERT(header.ok());
        session_ = static_cast<std::uint16_t>(session_ + 1U);
        last_ = f;
        has_last_ = true;
        return f;
    }
    void send(const Frame& f) noexcept { component_.step(ea::ConstByteSpan(f.data(), f.size())); }

    AdasInputGate component_;
    ea::e2e::P4Protector protector_;
    std::uint16_t session_ = 1U;
    Frame last_{};
    bool has_last_ = false;
};

}  // namespace adas::input_gate
