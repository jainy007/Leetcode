// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   adas_input_gate (pattern: input_gate)
// Inputs:    sha256:7b41fca38b878d513901b7e22e311a9def27a1eb78f5d184b5be554512adbf01
// Regenerate: python3 -m tools.gen

#pragma once

#include <array>
#include <cstdint>

#include "ea/core/span.hpp"
#include "ea/e2e/p4.hpp"
#include "ea/supervision/supervision.hpp"
#include "adas_input_gate/adas_input_gate_codec.hpp"
#include "adas_input_gate/adas_input_gate_types.hpp"

namespace adas::input_gate {

// AdasInputGate: interface gate for ObjectSummary (ASIL B, profile asil).
// One call to step() per 20 ms cycle:
//   1. classify the cycle: valid | e2e_error | range_error | missing
//   2. update the debounce counters
//   3. take at most one state transition (declared order)
//   4. publish output and status message, report alive
class AdasInputGate {
public:
    struct Output {
        State state;
        Qualifier qualifier;
        ObjectSummary data;  // last value that passed all checks
        CycleCondition condition;
        ea::e2e::P4Status e2e_status;
    };

    AdasInputGate() noexcept;

    // `frame` is the complete SOME/IP frame received in this cycle; empty if none arrived.
    void step(ea::ConstByteSpan frame) noexcept;

    const Output& output() const noexcept { return out_; }
    State state() const noexcept { return state_; }
    ea::ConstByteSpan status_payload() const noexcept { return ea::ConstByteSpan(status_.data(), status_.size()); }
    const ea::sup::Heartbeat& heartbeat() const noexcept { return heartbeat_; }

private:
    enum DebounceId : std::uint8_t {
        kFirstValid,
        kE2eFault,
        kRangeFault,
        kTimeout,
        kRecovered,
        kDebounceCount
    };

    CycleCondition classify(ea::ConstByteSpan frame, ObjectSummary& decoded) noexcept;
    void update_debounce(CycleCondition c) noexcept;
    void transition() noexcept;
    void publish_status() noexcept;

    ea::e2e::P4Checker checker_;
    std::array<std::uint16_t, kDebounceCount> counters_{};
    State state_;
    Output out_{};
    std::array<std::uint8_t, kGateStatusPayloadLength> status_{};
    std::uint32_t alive_counter_ = 0U;
    std::uint32_t e2e_error_count_ = 0U;
    ea::sup::Heartbeat heartbeat_;
};

}  // namespace adas::input_gate
