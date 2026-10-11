// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   adas_input_gate (pattern: input_gate)
// Inputs:    sha256:e5e321b3b23399ab06a6215638bc4c33e4a006a901504d70a3d9199badecc08e
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstddef>
#include <cstdint>

#include "ea/codec/someip.hpp"
#include "ea/core/result.hpp"
#include "ea/core/span.hpp"
#include "ea/e2e/p4.hpp"
#include "adas_input_gate/adas_input_gate_types.hpp"

namespace adas::input_gate {

// ---- GateStatus ----
inline constexpr std::size_t kGateStatusPayloadLength = 8U;
inline constexpr std::uint32_t kGateStatusCanId = 0x18FU;

// Decodes the payload (after transport header, including the E2E header if any).
// Fails with kLengthMismatch or kOutOfRange (physical range violated).
ea::Result<GateStatus> decode_gate_status(ea::ConstByteSpan payload) noexcept;

// Encodes signals into `payload`; E2E and transport headers are not touched.
// Fails with kLengthMismatch or kOutOfRange (value outside the physical range).
ea::Result<void> encode_gate_status(const GateStatus& in, ea::ByteSpan payload) noexcept;

// ---- ObjectSummary ----
inline constexpr std::size_t kObjectSummaryPayloadLength = 18U;
inline constexpr ea::someip::Expected kObjectSummarySomeIp{0x1234U, 0x8001U, 1U, ea::someip::MessageType::kNotification};
inline constexpr ea::e2e::P4Config kObjectSummaryE2e{0x0A0B0C0DU, 0U, 18U, 18U, 3U};

// Decodes the payload (after transport header, including the E2E header if any).
// Fails with kLengthMismatch or kOutOfRange (physical range violated).
ea::Result<ObjectSummary> decode_object_summary(ea::ConstByteSpan payload) noexcept;

// Encodes signals into `payload`; E2E and transport headers are not touched.
// Fails with kLengthMismatch or kOutOfRange (value outside the physical range).
ea::Result<void> encode_object_summary(const ObjectSummary& in, ea::ByteSpan payload) noexcept;

}  // namespace adas::input_gate
