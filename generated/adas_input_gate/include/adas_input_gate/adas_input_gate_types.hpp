// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   adas_input_gate (pattern: input_gate)
// Inputs:    sha256:e0c4923740db7d928336d8abc9859ca05e37446222ba5a081ee7c47bc53cdec6
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstddef>
#include <cstdint>

namespace adas::input_gate {

// Gate states. The numeric codes are part of the status message interface.
enum class State : std::uint8_t {
    kInit = 0U,
    kRun = 1U,
    kDegraded = 2U,
    kSafe = 3U,
};
inline constexpr std::size_t kStateCount = 4U;
const char* to_string(State s) noexcept;

enum class Qualifier : std::uint8_t {
    kInvalid = 0U,
    kValid = 1U,
};

// Classification of one cycle; exactly one applies.
enum class CycleCondition : std::uint8_t {
    kValid,
    kE2eError,
    kRangeError,
    kMissing,
};

// Signals of the input message ObjectSummary.
enum class Signal : std::uint8_t {
    kObjectCount,
    kNearestDistance,
    kNearestRelSpeed,
    kNearestConfidence,
};

// Signals of the status message GateStatus.
enum class StatusSignal : std::uint8_t {
    kState,
    kQualifier,
    kAliveCounter,
    kNearestDistance,
    kE2eErrorCount,
};

// Message GateStatus (can, CAN id 0x18FU). Source: examples/adas_input_gate/interfaces/gate_status.dbc:9
struct GateStatus {
    double state;  // [0.0, 3.0] 
    double qualifier;  // [0.0, 1.0] 
    double alive_counter;  // [0.0, 7.0] 
    double nearest_distance;  // [0.0, 250.0] m
    double e2e_error_count;  // [0.0, 255.0] 
};

// Message ObjectSummary (someip, service 0x1234U, event 0x8001U). Source: examples/adas_input_gate/model.yaml
struct ObjectSummary {
    double object_count;  // [0.0, 64.0] 1
    double nearest_distance;  // [0.0, 250.0] m
    double nearest_rel_speed;  // [-80.0, 80.0] m/s
    double nearest_confidence;  // [0.0, 100.0] %
};

}  // namespace adas::input_gate
