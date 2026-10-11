// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb_monitor (pattern: can_function)
// Inputs:    sha256:230536ebb5fd8cf2e5c6eff700cb3596edf001524e2100e20ccfddcdcbbad55e
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstddef>
#include <cstdint>

#include "ea/codec/someip.hpp"
#include "ea/core/result.hpp"
#include "ea/core/span.hpp"
#include "ea/e2e/p4.hpp"
#include "aeb_monitor/aeb_monitor_types.hpp"

namespace adas::aeb_monitor {

// ---- AebCommand ----
inline constexpr std::size_t kAebCommandPayloadLength = 8U;
inline constexpr std::uint32_t kAebCommandCanId = 0x310U;

// Decodes the payload (after transport header, including the E2E header if any).
// Fails with kLengthMismatch or kOutOfRange (physical range violated).
ea::Result<AebCommand> decode_aeb_command(ea::ConstByteSpan payload) noexcept;

// Encodes signals into `payload`; E2E and transport headers are not touched.
// Fails with kLengthMismatch or kOutOfRange (value outside the physical range).
ea::Result<void> encode_aeb_command(const AebCommand& in, ea::ByteSpan payload) noexcept;

// ---- AebMonitorStatus ----
inline constexpr std::size_t kAebMonitorStatusPayloadLength = 4U;
inline constexpr std::uint32_t kAebMonitorStatusCanId = 0x311U;

// Decodes the payload (after transport header, including the E2E header if any).
// Fails with kLengthMismatch or kOutOfRange (physical range violated).
ea::Result<AebMonitorStatus> decode_aeb_monitor_status(ea::ConstByteSpan payload) noexcept;

// Encodes signals into `payload`; E2E and transport headers are not touched.
// Fails with kLengthMismatch or kOutOfRange (value outside the physical range).
ea::Result<void> encode_aeb_monitor_status(const AebMonitorStatus& in, ea::ByteSpan payload) noexcept;

// ---- RadarObject ----
inline constexpr std::size_t kRadarObjectPayloadLength = 8U;
inline constexpr std::uint32_t kRadarObjectCanId = 0x200U;

// Decodes the payload (after transport header, including the E2E header if any).
// Fails with kLengthMismatch or kOutOfRange (physical range violated).
ea::Result<RadarObject> decode_radar_object(ea::ConstByteSpan payload) noexcept;

// Encodes signals into `payload`; E2E and transport headers are not touched.
// Fails with kLengthMismatch or kOutOfRange (value outside the physical range).
ea::Result<void> encode_radar_object(const RadarObject& in, ea::ByteSpan payload) noexcept;

}  // namespace adas::aeb_monitor
