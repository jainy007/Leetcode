// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc_monitor (pattern: can_function)
// Inputs:    sha256:aaed2b82deaf8ac2890ea08cf061dfa671e7be14789da61b08e9a29f91792b3a
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstddef>
#include <cstdint>

#include "ea/codec/someip.hpp"
#include "ea/core/result.hpp"
#include "ea/core/span.hpp"
#include "ea/e2e/p4.hpp"
#include "acc_monitor/acc_monitor_types.hpp"

namespace adas::acc_monitor {

// ---- AccCommand ----
inline constexpr std::size_t kAccCommandPayloadLength = 8U;
inline constexpr std::uint32_t kAccCommandCanId = 0x300U;

// Decodes the payload (after transport header, including the E2E header if any).
// Fails with kLengthMismatch or kOutOfRange (physical range violated).
ea::Result<AccCommand> decode_acc_command(ea::ConstByteSpan payload) noexcept;

// Encodes signals into `payload`; E2E and transport headers are not touched.
// Fails with kLengthMismatch or kOutOfRange (value outside the physical range).
ea::Result<void> encode_acc_command(const AccCommand& in, ea::ByteSpan payload) noexcept;

// ---- AccMonitorStatus ----
inline constexpr std::size_t kAccMonitorStatusPayloadLength = 4U;
inline constexpr std::uint32_t kAccMonitorStatusCanId = 0x301U;

// Decodes the payload (after transport header, including the E2E header if any).
// Fails with kLengthMismatch or kOutOfRange (physical range violated).
ea::Result<AccMonitorStatus> decode_acc_monitor_status(ea::ConstByteSpan payload) noexcept;

// Encodes signals into `payload`; E2E and transport headers are not touched.
// Fails with kLengthMismatch or kOutOfRange (value outside the physical range).
ea::Result<void> encode_acc_monitor_status(const AccMonitorStatus& in, ea::ByteSpan payload) noexcept;

// ---- RadarObject ----
inline constexpr std::size_t kRadarObjectPayloadLength = 8U;
inline constexpr std::uint32_t kRadarObjectCanId = 0x200U;

// Decodes the payload (after transport header, including the E2E header if any).
// Fails with kLengthMismatch or kOutOfRange (physical range violated).
ea::Result<RadarObject> decode_radar_object(ea::ConstByteSpan payload) noexcept;

// Encodes signals into `payload`; E2E and transport headers are not touched.
// Fails with kLengthMismatch or kOutOfRange (value outside the physical range).
ea::Result<void> encode_radar_object(const RadarObject& in, ea::ByteSpan payload) noexcept;

}  // namespace adas::acc_monitor
