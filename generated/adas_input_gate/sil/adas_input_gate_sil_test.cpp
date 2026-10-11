// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   adas_input_gate (pattern: input_gate)
// Inputs:    sha256:83d39aa4e7e734b248f9bb99cf241f4c8a723350803ed4fbdd50a1b76fd9d353
// Regenerate: python3 -m tools.gen

#include <gtest/gtest.h>

#include <limits>

#include "sil/adas_input_gate_sil.hpp"

// SIL tests generated from the Gherkin scenarios; each step expands from the step catalogue
// sil/steps/input_gate.yaml. Test names are referenced by trace.json.

namespace adas::input_gate {
namespace {

class AdasInputGateSilTest : public ::testing::Test {
protected:
    AdasInputGateSil sil_;
};

// Scenario: First valid frame brings the gate into RUN with valid output
// Source:   examples/adas_input_gate/features/gate.feature:6
// Verifies: REQ-GATE-001, REQ-GATE-002
TEST_F(AdasInputGateSilTest, S01_FirstValidFrameBringsTheGateIntoRUNWithValidOutput) {
    // Given a freshly started component  [GATE-S01]
    // freshly constructed component (test fixture)
    // Then the state is INIT  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kInit);
    // When the component receives 1 valid frame  [GATE-S02]
    sil_.receive_valid(1);
    // Then the state is RUN  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kRun);
    // And the output qualifier is VALID  [GATE-S08]
    EXPECT_EQ(sil_.qualifier(), Qualifier::kValid);
    // And the output signal nearest_distance equals 42.5 within 0.01  [GATE-S09]
    EXPECT_NEAR(sil_.output_signal(Signal::kNearestDistance), 42.5, 0.01);
    // And the output signal nearest_rel_speed equals -3.25 within 0.01  [GATE-S09]
    EXPECT_NEAR(sil_.output_signal(Signal::kNearestRelSpeed), -3.25, 0.01);
}

// Scenario: Corrupted frames do not start the gate
// Source:   examples/adas_input_gate/features/gate.feature:16
// Verifies: REQ-GATE-001
TEST_F(AdasInputGateSilTest, S02_CorruptedFramesDoNotStartTheGate) {
    // Given a freshly started component  [GATE-S01]
    // freshly constructed component (test fixture)
    // When the component receives 2 frames with corrupted CRC  [GATE-S03]
    sil_.receive_corrupted_crc(2);
    // Then the state is INIT  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kInit);
    // And the output qualifier is INVALID  [GATE-S08]
    EXPECT_EQ(sil_.qualifier(), Qualifier::kInvalid);
}

// Scenario: Three consecutive E2E errors degrade the gate
// Source:   examples/adas_input_gate/features/gate.feature:23
// Verifies: REQ-GATE-003
TEST_F(AdasInputGateSilTest, S03_ThreeConsecutiveE2EErrorsDegradeTheGate) {
    // Given the component has received 5 valid frames  [GATE-S02]
    sil_.receive_valid(5);
    // When the component receives 2 frames with corrupted CRC  [GATE-S03]
    sil_.receive_corrupted_crc(2);
    // Then the state is RUN  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kRun);
    // When the component receives 1 frame with corrupted CRC  [GATE-S03]
    sil_.receive_corrupted_crc(1);
    // Then the state is DEGRADED  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kDegraded);
    // And the output qualifier is INVALID  [GATE-S08]
    EXPECT_EQ(sil_.qualifier(), Qualifier::kInvalid);
}

// Scenario: Repeated counters count as E2E errors
// Source:   examples/adas_input_gate/features/gate.feature:32
// Verifies: REQ-GATE-003
TEST_F(AdasInputGateSilTest, S04_RepeatedCountersCountAsE2EErrors) {
    // Given the component has received 5 valid frames  [GATE-S02]
    sil_.receive_valid(5);
    // When the component receives 3 frames with repeated counter  [GATE-S04]
    sil_.receive_repeated_counter(3);
    // Then the state is DEGRADED  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kDegraded);
}

// Scenario: Non-consecutive E2E errors are tolerated
// Source:   examples/adas_input_gate/features/gate.feature:38
// Verifies: REQ-GATE-003
TEST_F(AdasInputGateSilTest, S05_NonConsecutiveE2EErrorsAreTolerated) {
    // Given the component has received 5 valid frames  [GATE-S02]
    sil_.receive_valid(5);
    // When the component receives 2 frames with corrupted CRC  [GATE-S03]
    sil_.receive_corrupted_crc(2);
    // And the component receives 1 valid frame  [GATE-S02]
    sil_.receive_valid(1);
    // And the component receives 2 frames with corrupted CRC  [GATE-S03]
    sil_.receive_corrupted_crc(2);
    // Then the state is RUN  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kRun);
}

// Scenario: Out-of-range signal values are not forwarded #1
// Source:   examples/adas_input_gate/features/gate.feature:46
// Verifies: REQ-GATE-004
// Example:  signal=object_count, value=65
TEST_F(AdasInputGateSilTest, S06_OutOfRangeSignalValuesAreNotForwarded_Ex1) {
    // Given the component has received 3 valid frames  [GATE-S02]
    sil_.receive_valid(3);
    // When the component receives a frame with object_count set to 65  [GATE-S05]
    sil_.receive_with(Signal::kObjectCount, 65.0);
    // Then the state is DEGRADED  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kDegraded);
    // And the output qualifier is INVALID  [GATE-S08]
    EXPECT_EQ(sil_.qualifier(), Qualifier::kInvalid);
    // And the output signal nearest_distance equals 42.5 within 0.01  [GATE-S09]
    EXPECT_NEAR(sil_.output_signal(Signal::kNearestDistance), 42.5, 0.01);
}

// Scenario: Out-of-range signal values are not forwarded #2
// Source:   examples/adas_input_gate/features/gate.feature:46
// Verifies: REQ-GATE-004
// Example:  signal=nearest_distance, value=250.5
TEST_F(AdasInputGateSilTest, S07_OutOfRangeSignalValuesAreNotForwarded_Ex2) {
    // Given the component has received 3 valid frames  [GATE-S02]
    sil_.receive_valid(3);
    // When the component receives a frame with nearest_distance set to 250.5  [GATE-S05]
    sil_.receive_with(Signal::kNearestDistance, 250.5);
    // Then the state is DEGRADED  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kDegraded);
    // And the output qualifier is INVALID  [GATE-S08]
    EXPECT_EQ(sil_.qualifier(), Qualifier::kInvalid);
    // And the output signal nearest_distance equals 42.5 within 0.01  [GATE-S09]
    EXPECT_NEAR(sil_.output_signal(Signal::kNearestDistance), 42.5, 0.01);
}

// Scenario: Out-of-range signal values are not forwarded #3
// Source:   examples/adas_input_gate/features/gate.feature:46
// Verifies: REQ-GATE-004
// Example:  signal=nearest_rel_speed, value=-80.5
TEST_F(AdasInputGateSilTest, S08_OutOfRangeSignalValuesAreNotForwarded_Ex3) {
    // Given the component has received 3 valid frames  [GATE-S02]
    sil_.receive_valid(3);
    // When the component receives a frame with nearest_rel_speed set to -80.5  [GATE-S05]
    sil_.receive_with(Signal::kNearestRelSpeed, -80.5);
    // Then the state is DEGRADED  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kDegraded);
    // And the output qualifier is INVALID  [GATE-S08]
    EXPECT_EQ(sil_.qualifier(), Qualifier::kInvalid);
    // And the output signal nearest_distance equals 42.5 within 0.01  [GATE-S09]
    EXPECT_NEAR(sil_.output_signal(Signal::kNearestDistance), 42.5, 0.01);
}

// Scenario: Out-of-range signal values are not forwarded #4
// Source:   examples/adas_input_gate/features/gate.feature:46
// Verifies: REQ-GATE-004
// Example:  signal=nearest_confidence, value=100.5
TEST_F(AdasInputGateSilTest, S09_OutOfRangeSignalValuesAreNotForwarded_Ex4) {
    // Given the component has received 3 valid frames  [GATE-S02]
    sil_.receive_valid(3);
    // When the component receives a frame with nearest_confidence set to 100.5  [GATE-S05]
    sil_.receive_with(Signal::kNearestConfidence, 100.5);
    // Then the state is DEGRADED  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kDegraded);
    // And the output qualifier is INVALID  [GATE-S08]
    EXPECT_EQ(sil_.qualifier(), Qualifier::kInvalid);
    // And the output signal nearest_distance equals 42.5 within 0.01  [GATE-S09]
    EXPECT_NEAR(sil_.output_signal(Signal::kNearestDistance), 42.5, 0.01);
}

// Scenario: Boundary values are accepted
// Source:   examples/adas_input_gate/features/gate.feature:61
// Verifies: REQ-GATE-004
TEST_F(AdasInputGateSilTest, S10_BoundaryValuesAreAccepted) {
    // Given the component has received 1 valid frame  [GATE-S02]
    sil_.receive_valid(1);
    // When the component receives a frame with nearest_distance set to 250  [GATE-S05]
    sil_.receive_with(Signal::kNearestDistance, 250.0);
    // Then the state is RUN  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kRun);
    // And the output signal nearest_distance equals 250 within 0.01  [GATE-S09]
    EXPECT_NEAR(sil_.output_signal(Signal::kNearestDistance), 250.0, 0.01);
    // When the component receives a frame with nearest_rel_speed set to -80  [GATE-S05]
    sil_.receive_with(Signal::kNearestRelSpeed, -80.0);
    // Then the state is RUN  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kRun);
}

// Scenario: Five missing cycles in RUN lead to SAFE
// Source:   examples/adas_input_gate/features/gate.feature:70
// Verifies: REQ-GATE-005
TEST_F(AdasInputGateSilTest, S11_FiveMissingCyclesInRUNLeadToSAFE) {
    // Given the component has received 2 valid frames  [GATE-S02]
    sil_.receive_valid(2);
    // When 4 cycles pass without a frame  [GATE-S06]
    sil_.miss(4);
    // Then the state is RUN  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kRun);
    // And the output qualifier is INVALID  [GATE-S08]
    EXPECT_EQ(sil_.qualifier(), Qualifier::kInvalid);
    // When 1 cycle passes without a frame  [GATE-S06]
    sil_.miss(1);
    // Then the state is SAFE  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kSafe);
}

// Scenario: No frame at all after start leads to SAFE
// Source:   examples/adas_input_gate/features/gate.feature:79
// Verifies: REQ-GATE-005
TEST_F(AdasInputGateSilTest, S12_NoFrameAtAllAfterStartLeadsToSAFE) {
    // Given a freshly started component  [GATE-S01]
    // freshly constructed component (test fixture)
    // When 5 cycles pass without a frame  [GATE-S06]
    sil_.miss(5);
    // Then the state is SAFE  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kSafe);
}

// Scenario: Ten consecutive valid frames recover from DEGRADED
// Source:   examples/adas_input_gate/features/gate.feature:85
// Verifies: REQ-GATE-006
TEST_F(AdasInputGateSilTest, S13_TenConsecutiveValidFramesRecoverFromDEGRADED) {
    // Given the component has received 3 valid frames  [GATE-S02]
    sil_.receive_valid(3);
    // And the component receives a frame with nearest_distance set to 300  [GATE-S05]
    sil_.receive_with(Signal::kNearestDistance, 300.0);
    // Then the state is DEGRADED  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kDegraded);
    // When the component receives 9 valid frames  [GATE-S02]
    sil_.receive_valid(9);
    // Then the state is DEGRADED  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kDegraded);
    // And the output qualifier is INVALID  [GATE-S08]
    EXPECT_EQ(sil_.qualifier(), Qualifier::kInvalid);
    // When the component receives 1 valid frame  [GATE-S02]
    sil_.receive_valid(1);
    // Then the state is RUN  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kRun);
    // And the output qualifier is VALID  [GATE-S08]
    EXPECT_EQ(sil_.qualifier(), Qualifier::kValid);
}

// Scenario: SAFE is latched
// Source:   examples/adas_input_gate/features/gate.feature:97
// Verifies: REQ-GATE-007
TEST_F(AdasInputGateSilTest, S14_SAFEIsLatched) {
    // Given a freshly started component  [GATE-S01]
    // freshly constructed component (test fixture)
    // When 5 cycles pass without a frame  [GATE-S06]
    sil_.miss(5);
    // And the component receives 20 valid frames  [GATE-S02]
    sil_.receive_valid(20);
    // Then the state is SAFE  [GATE-S07]
    EXPECT_EQ(sil_.state(), State::kSafe);
    // And the output qualifier is INVALID  [GATE-S08]
    EXPECT_EQ(sil_.qualifier(), Qualifier::kInvalid);
}

// Scenario: Status message reflects the gate every cycle
// Source:   examples/adas_input_gate/features/gate.feature:105
// Verifies: REQ-GATE-008
TEST_F(AdasInputGateSilTest, S15_StatusMessageReflectsTheGateEveryCycle) {
    // Given the component has received 1 valid frame  [GATE-S02]
    sil_.receive_valid(1);
    // Then the status message reports state RUN  [GATE-S11]
    EXPECT_EQ(sil_.status_state(), State::kRun);
    // And the status message signal Qualifier is 1  [GATE-S12]
    EXPECT_NEAR(sil_.status_signal(StatusSignal::kQualifier), 1.0, 1e-6);
    // And the status message signal NearestDistance is 42.5  [GATE-S12]
    EXPECT_NEAR(sil_.status_signal(StatusSignal::kNearestDistance), 42.5, 1e-6);
    // When the component receives 3 frames with corrupted CRC  [GATE-S03]
    sil_.receive_corrupted_crc(3);
    // Then the status message reports state DEGRADED  [GATE-S11]
    EXPECT_EQ(sil_.status_state(), State::kDegraded);
    // And the status message signal Qualifier is 0  [GATE-S12]
    EXPECT_NEAR(sil_.status_signal(StatusSignal::kQualifier), 0.0, 1e-6);
    // And the status message signal E2eErrorCount is 3  [GATE-S12]
    EXPECT_NEAR(sil_.status_signal(StatusSignal::kE2eErrorCount), 3.0, 1e-6);
}

// Scenario: Alive counter and heartbeat advance once per cycle
// Source:   examples/adas_input_gate/features/gate.feature:116
// Verifies: REQ-GATE-008, REQ-GATE-009
TEST_F(AdasInputGateSilTest, S16_AliveCounterAndHeartbeatAdvanceOncePerCycle) {
    // Given a freshly started component  [GATE-S01]
    // freshly constructed component (test fixture)
    // When the component receives 9 valid frames  [GATE-S02]
    sil_.receive_valid(9);
    // Then the heartbeat count is 9  [GATE-S10]
    EXPECT_EQ(sil_.heartbeat(), 9U);
    // And the status message signal AliveCounter is 1  [GATE-S12]
    EXPECT_NEAR(sil_.status_signal(StatusSignal::kAliveCounter), 1.0, 1e-6);
}

}  // namespace
}  // namespace adas::input_gate
