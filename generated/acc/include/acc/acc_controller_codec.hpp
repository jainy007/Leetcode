// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc (pattern: can_function)
// Inputs:    sha256:fbfce52a7d6bd218de02fbc84f6ae65f4d57f340161623b43290af8f8ea21a6d
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstddef>
#include <cstdint>

#include "ea/codec/someip.hpp"
#include "ea/core/result.hpp"
#include "ea/core/span.hpp"
#include "ea/e2e/p4.hpp"
#include "acc/acc_controller_types.hpp"

namespace adas::acc {

// ---- AccCommand ----
inline constexpr std::size_t kAccCommandPayloadLength = 8U;
inline constexpr std::uint32_t kAccCommandCanId = 0x300U;

// Decodes the payload (after transport header, including the E2E header if any).
// Fails with kLengthMismatch or kOutOfRange (physical range violated).
ea::Result<AccCommand> decode_acc_command(ea::ConstByteSpan payload) noexcept;

// Encodes signals into `payload`; E2E and transport headers are not touched.
// Fails with kLengthMismatch or kOutOfRange (value outside the physical range).
ea::Result<void> encode_acc_command(const AccCommand& in, ea::ByteSpan payload) noexcept;

// ---- DriverInput ----
inline constexpr std::size_t kDriverInputPayloadLength = 8U;
inline constexpr std::uint32_t kDriverInputCanId = 0x220U;

// Decodes the payload (after transport header, including the E2E header if any).
// Fails with kLengthMismatch or kOutOfRange (physical range violated).
ea::Result<DriverInput> decode_driver_input(ea::ConstByteSpan payload) noexcept;

// Encodes signals into `payload`; E2E and transport headers are not touched.
// Fails with kLengthMismatch or kOutOfRange (value outside the physical range).
ea::Result<void> encode_driver_input(const DriverInput& in, ea::ByteSpan payload) noexcept;

// ---- EgoState ----
inline constexpr std::size_t kEgoStatePayloadLength = 8U;
inline constexpr std::uint32_t kEgoStateCanId = 0x210U;

// Decodes the payload (after transport header, including the E2E header if any).
// Fails with kLengthMismatch or kOutOfRange (physical range violated).
ea::Result<EgoState> decode_ego_state(ea::ConstByteSpan payload) noexcept;

// Encodes signals into `payload`; E2E and transport headers are not touched.
// Fails with kLengthMismatch or kOutOfRange (value outside the physical range).
ea::Result<void> encode_ego_state(const EgoState& in, ea::ByteSpan payload) noexcept;

// ---- RadarObject ----
inline constexpr std::size_t kRadarObjectPayloadLength = 8U;
inline constexpr std::uint32_t kRadarObjectCanId = 0x200U;

// Decodes the payload (after transport header, including the E2E header if any).
// Fails with kLengthMismatch or kOutOfRange (physical range violated).
ea::Result<RadarObject> decode_radar_object(ea::ConstByteSpan payload) noexcept;

// Encodes signals into `payload`; E2E and transport headers are not touched.
// Fails with kLengthMismatch or kOutOfRange (value outside the physical range).
ea::Result<void> encode_radar_object(const RadarObject& in, ea::ByteSpan payload) noexcept;

}  // namespace adas::acc
