// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   adas_input_gate (pattern: input_gate)
// Inputs:    sha256:7b41fca38b878d513901b7e22e311a9def27a1eb78f5d184b5be554512adbf01
// Regenerate: python3 -m tools.gen

#include "adas_input_gate/adas_input_gate.hpp"

#include "ea/codec/someip.hpp"
#include "ea/core/assert.hpp"
#include "ea/core/saturating.hpp"

namespace adas::input_gate {

const char* to_string(State s) noexcept {
    switch (s) {
        case State::kInit:
            return "INIT";
        case State::kRun:
            return "RUN";
        case State::kDegraded:
            return "DEGRADED";
        case State::kSafe:
            return "SAFE";
    }
    return "UNKNOWN";
}

AdasInputGate::AdasInputGate() noexcept
    : checker_(kObjectSummaryE2e), state_(State::kInit) {
    out_.state = state_;
    out_.qualifier = Qualifier::kInvalid;
    out_.condition = CycleCondition::kMissing;
    out_.e2e_status = ea::e2e::P4Status::kNoNewData;
}

CycleCondition AdasInputGate::classify(ea::ConstByteSpan frame, ObjectSummary& decoded) noexcept {
    if (frame.empty()) {
        out_.e2e_status = ea::e2e::P4Status::kNoNewData;
        return CycleCondition::kMissing;
    }
    const ea::Result<ea::ConstByteSpan> payload = ea::someip::parse(frame, kObjectSummarySomeIp);
    if (!payload.ok()) {
        out_.e2e_status = ea::e2e::P4Status::kError;
        return CycleCondition::kE2eError;  // malformed transport is treated as a communication fault
    }
    out_.e2e_status = checker_.check(*payload);
    if (!ea::e2e::is_usable(out_.e2e_status)) {
        return CycleCondition::kE2eError;
    }
    const ea::Result<ObjectSummary> d = decode_object_summary(*payload);
    if (!d.ok()) {
        return (d.error() == ea::ErrorCode::kOutOfRange) ? CycleCondition::kRangeError : CycleCondition::kE2eError;
    }
    decoded = *d;
    return CycleCondition::kValid;
}

void AdasInputGate::update_debounce(CycleCondition c) noexcept {
    // first_valid: consecutive valid, threshold 1  [REQ-GATE-001]
    counters_[kFirstValid] = (c == CycleCondition::kValid)
        ? ea::sat_inc(counters_[kFirstValid])
        : static_cast<std::uint16_t>(0U);
    // e2e_fault: consecutive e2e_error, threshold 3  [REQ-GATE-003]
    counters_[kE2eFault] = (c == CycleCondition::kE2eError)
        ? ea::sat_inc(counters_[kE2eFault])
        : static_cast<std::uint16_t>(0U);
    // range_fault: consecutive range_error, threshold 1  [REQ-GATE-004]
    counters_[kRangeFault] = (c == CycleCondition::kRangeError)
        ? ea::sat_inc(counters_[kRangeFault])
        : static_cast<std::uint16_t>(0U);
    // timeout: consecutive missing, threshold 5  [REQ-GATE-005]
    counters_[kTimeout] = (c == CycleCondition::kMissing)
        ? ea::sat_inc(counters_[kTimeout])
        : static_cast<std::uint16_t>(0U);
    // recovered: consecutive valid, threshold 10  [REQ-GATE-006]
    counters_[kRecovered] = (c == CycleCondition::kValid)
        ? ea::sat_inc(counters_[kRecovered])
        : static_cast<std::uint16_t>(0U);
}

void AdasInputGate::transition() noexcept {
    switch (state_) {
        case State::kInit:
            if (counters_[kFirstValid] >= 1U) {
                state_ = State::kRun;  // REQ-GATE-001
            } else if (counters_[kTimeout] >= 5U) {
                state_ = State::kSafe;  // REQ-GATE-005
            } else {
                // no transition
            }
            break;
        case State::kRun:
            if (counters_[kTimeout] >= 5U) {
                state_ = State::kSafe;  // REQ-GATE-005
            } else if (counters_[kE2eFault] >= 3U) {
                state_ = State::kDegraded;  // REQ-GATE-003
            } else if (counters_[kRangeFault] >= 1U) {
                state_ = State::kDegraded;  // REQ-GATE-004
            } else {
                // no transition
            }
            break;
        case State::kDegraded:
            if (counters_[kTimeout] >= 5U) {
                state_ = State::kSafe;  // REQ-GATE-005
            } else if (counters_[kRecovered] >= 10U) {
                state_ = State::kRun;  // REQ-GATE-006
            } else {
                // no transition
            }
            break;
        case State::kSafe:
            // latched: no exit until restart
            break;
    }
}

void AdasInputGate::publish_status() noexcept {
    GateStatus msg{};
    msg.state = static_cast<double>(static_cast<std::uint8_t>(state_));
    msg.qualifier = static_cast<double>(static_cast<std::uint8_t>(out_.qualifier));
    alive_counter_ = (alive_counter_ + 1U) % 8U;  // protocol counter, wraps by definition
    msg.alive_counter = static_cast<double>(alive_counter_);
    msg.nearest_distance = out_.data.nearest_distance;
    msg.e2e_error_count = static_cast<double>((e2e_error_count_ < 255U) ? e2e_error_count_ : 255U);
    // Ranges are proven compatible by the generator (GEN-046); a failure is a contract violation.
    const ea::Result<void> r = encode_gate_status(msg, ea::ByteSpan(status_.data(), status_.size()));
    EA_ASSERT(r.ok());
}

void AdasInputGate::step(ea::ConstByteSpan frame) noexcept {
    ObjectSummary decoded{};
    const CycleCondition c = classify(frame, decoded);
    if (c == CycleCondition::kE2eError) {
        e2e_error_count_ = ea::sat_inc(e2e_error_count_);
    }
    update_debounce(c);
    transition();

    out_.state = state_;
    out_.condition = c;
    const bool valid_state = (state_ == State::kRun);
    if (valid_state && c == CycleCondition::kValid) {
        out_.qualifier = Qualifier::kValid;
        out_.data = decoded;
    } else {
        out_.qualifier = Qualifier::kInvalid;  // last valid data is kept, never overwritten by unchecked data
    }
    publish_status();
    heartbeat_.beat();
}

}  // namespace adas::input_gate
