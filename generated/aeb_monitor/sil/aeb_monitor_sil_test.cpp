// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb_monitor (pattern: can_function)
// Inputs:    sha256:4c2c50398135b7072ac31e6b5ee98281a3ac11962a726a5375f5b4a9eb38232c
// Regenerate: python3 -m tools.gen

#include <gtest/gtest.h>

#include "sil/aeb_monitor_sil.hpp"

// Closed-loop SIL tests generated from the Gherkin scenarios; each step expands from the step
// catalogues sil/steps/can_function.yaml and sil/steps/features/aeb_monitor.yaml.
// Test names are referenced by trace.json.

namespace adas::aeb_monitor {
namespace {

class AebMonitorSilTest : public ::testing::Test {
protected:
    AebMonitorSil sil_;
};

// Scenario: A justified braking cascade is approved until the vehicle has stopped
// Source:   examples/aeb_monitor/features/justification.feature:9
// Verifies: REQ-AEBM-001, REQ-AEBM-002, REQ-AEBM-010
TEST_F(AebMonitorSilTest, S01_AJustifiedBrakingCascadeIsApprovedUntilTheVehicleHasStopped) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 22 m ahead  [FN-S19]
    sil_.plant().set_lead(22.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the permit is granted  [MON-S02]
    EXPECT_EQ(sil_.actuator().monitor().permit, true);
    // When the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the actuator applies -4.0 m/s2  [MON-S07]
    ASSERT_TRUE(sil_.actuator().applying()); EXPECT_NEAR(sil_.actuator().applied_mps2(), -4.0, 1e-6);
    // When the AEB controller is in FULL_BRAKE with 8 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kFullBrake), 8.0);
    // And 3 s pass  [FN-S10]
    sil_.run(3.0);
    // Then the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the actuator applied every command  [MON-S06]
    EXPECT_GT(sil_.actuator().metrics().applied_commands, 0U); EXPECT_EQ(sil_.actuator().metrics().rejected_commands, 0U);
    // And the actuator applies -8.0 m/s2  [MON-S07]
    ASSERT_TRUE(sil_.actuator().applying()); EXPECT_NEAR(sil_.actuator().applied_mps2(), -8.0, 1e-6);
    // And no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the ego vehicle has stopped  [FN-S47]
    EXPECT_LT(sil_.plant().ego_speed_kph(), 0.5);
    // And every monitor frame passed the E2E check  [MON-S14]
    EXPECT_GT(sil_.actuator().metrics().monitor_frames, 0U); EXPECT_EQ(sil_.actuator().metrics().monitor_e2e_errors, 0U);
    // And one monitor frame was sent per 20 ms  [MON-S15]
    EXPECT_EQ(sil_.actuator().metrics().monitor_frames, sil_.plant().metrics().ticks);
}

// Scenario: Braking onset needs an in-lane target within TTC 2.1 s #1
// Source:   examples/aeb_monitor/features/justification.feature:30
// Verifies: REQ-AEBM-005
// Example:  d=27, lat=0.0, state=MONITORING, reason=NONE
TEST_F(AebMonitorSilTest, S02_BrakingOnsetNeedsAnInLaneTargetWithinTTC21S_Ex1) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a vehicle drives 27 m ahead at 0 km/h with 0.0 m lateral offset  [FN-S12]
    sil_.plant().set_lead(27.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor reports the fault reason NONE  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kNone));
}

// Scenario: Braking onset needs an in-lane target within TTC 2.1 s #2
// Source:   examples/aeb_monitor/features/justification.feature:30
// Verifies: REQ-AEBM-005
// Example:  d=32, lat=0.0, state=SAFE_STATE, reason=UNJUSTIFIED_BRAKING
TEST_F(AebMonitorSilTest, S03_BrakingOnsetNeedsAnInLaneTargetWithinTTC21S_Ex2) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a vehicle drives 32 m ahead at 0 km/h with 0.0 m lateral offset  [FN-S12]
    sil_.plant().set_lead(32.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason UNJUSTIFIED_BRAKING  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kUnjustifiedBraking));
}

// Scenario: Braking onset needs an in-lane target within TTC 2.1 s #3
// Source:   examples/aeb_monitor/features/justification.feature:30
// Verifies: REQ-AEBM-005
// Example:  d=27, lat=2.0, state=MONITORING, reason=NONE
TEST_F(AebMonitorSilTest, S04_BrakingOnsetNeedsAnInLaneTargetWithinTTC21S_Ex3) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a vehicle drives 27 m ahead at 0 km/h with 2.0 m lateral offset  [FN-S12]
    sil_.plant().set_lead(27.0, 0.0, 2.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor reports the fault reason NONE  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kNone));
}

// Scenario: Braking onset needs an in-lane target within TTC 2.1 s #4
// Source:   examples/aeb_monitor/features/justification.feature:30
// Verifies: REQ-AEBM-005
// Example:  d=27, lat=2.2, state=SAFE_STATE, reason=UNJUSTIFIED_BRAKING
TEST_F(AebMonitorSilTest, S05_BrakingOnsetNeedsAnInLaneTargetWithinTTC21S_Ex4) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a vehicle drives 27 m ahead at 0 km/h with 2.2 m lateral offset  [FN-S12]
    sil_.plant().set_lead(27.0, 0.0, 2.2);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason UNJUSTIFIED_BRAKING  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kUnjustifiedBraking));
}

// Scenario: Braking without any target is never applied
// Source:   examples/aeb_monitor/features/justification.feature:48
// Verifies: REQ-AEBM-005, REQ-AEBM-009
TEST_F(AebMonitorSilTest, S06_BrakingWithoutAnyTargetIsNeverApplied) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in FULL_BRAKE with 8 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kFullBrake), 8.0);
    // And 0.5 s pass  [FN-S10]
    sil_.run(0.5);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason UNJUSTIFIED_BRAKING  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kUnjustifiedBraking));
    // And the permit is withdrawn  [MON-S02]
    EXPECT_EQ(sil_.actuator().monitor().permit, false);
    // And the applied deceleration never exceeded 0 m/s2  [MON-S12]
    EXPECT_LE(sil_.actuator().metrics().max_applied_decel_mps2, 0.0 + 1e-9);
    // And the actual deceleration of the vehicle never exceeded 0.1 m/s2  [MON-S16]
    EXPECT_LE(sil_.plant().metrics().max_decel_mps2, 0.1 + 1e-9);
    // And the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
}

// Scenario: A target receding is no reason to brake
// Source:   examples/aeb_monitor/features/justification.feature:62
// Verifies: REQ-AEBM-005
TEST_F(AebMonitorSilTest, S07_ATargetRecedingIsNoReasonToBrake) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a lead vehicle drives 15 m ahead at 60 km/h  [FN-S11]
    sil_.plant().set_lead(15.0, 60.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason UNJUSTIFIED_BRAKING  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kUnjustifiedBraking));
}

// Scenario: Full braking onset needs TTC 1.5 s #1
// Source:   examples/aeb_monitor/features/justification.feature:73
// Verifies: REQ-AEBM-006
// Example:  d=20, state=MONITORING, reason=NONE
TEST_F(AebMonitorSilTest, S08_FullBrakingOnsetNeedsTTC15S_Ex1) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 20 m ahead  [FN-S19]
    sil_.plant().set_lead(20.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in FULL_BRAKE with 8 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kFullBrake), 8.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor reports the fault reason NONE  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kNone));
}

// Scenario: Full braking onset needs TTC 1.5 s #2
// Source:   examples/aeb_monitor/features/justification.feature:73
// Verifies: REQ-AEBM-006
// Example:  d=25, state=SAFE_STATE, reason=UNJUSTIFIED_FULL_BRAKING
TEST_F(AebMonitorSilTest, S09_FullBrakingOnsetNeedsTTC15S_Ex2) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 25 m ahead  [FN-S19]
    sil_.plant().set_lead(25.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in FULL_BRAKE with 8 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kFullBrake), 8.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason UNJUSTIFIED_FULL_BRAKING  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kUnjustifiedFullBraking));
}

// Scenario: Deceleration limits per braking state #1
// Source:   examples/aeb_monitor/features/justification.feature:91
// Verifies: REQ-AEBM-003
// Example:  s=FULL_BRAKE, a=8.1, state=MONITORING, reason=NONE
TEST_F(AebMonitorSilTest, S10_DecelerationLimitsPerBrakingState_Ex1) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 15 m ahead  [FN-S19]
    sil_.plant().set_lead(15.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in FULL_BRAKE with 8.1 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kFullBrake), 8.1);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor reports the fault reason NONE  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kNone));
}

// Scenario: Deceleration limits per braking state #2
// Source:   examples/aeb_monitor/features/justification.feature:91
// Verifies: REQ-AEBM-003
// Example:  s=FULL_BRAKE, a=8.2, state=SAFE_STATE, reason=REQUEST_LIMIT
TEST_F(AebMonitorSilTest, S11_DecelerationLimitsPerBrakingState_Ex2) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 15 m ahead  [FN-S19]
    sil_.plant().set_lead(15.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in FULL_BRAKE with 8.2 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kFullBrake), 8.2);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_LIMIT  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kRequestLimit));
}

// Scenario: Deceleration limits per braking state #3
// Source:   examples/aeb_monitor/features/justification.feature:91
// Verifies: REQ-AEBM-003
// Example:  s=PARTIAL_BRAKE, a=4.1, state=MONITORING, reason=NONE
TEST_F(AebMonitorSilTest, S12_DecelerationLimitsPerBrakingState_Ex3) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 15 m ahead  [FN-S19]
    sil_.plant().set_lead(15.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in PARTIAL_BRAKE with 4.1 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.1);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor reports the fault reason NONE  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kNone));
}

// Scenario: Deceleration limits per braking state #4
// Source:   examples/aeb_monitor/features/justification.feature:91
// Verifies: REQ-AEBM-003
// Example:  s=PARTIAL_BRAKE, a=4.2, state=SAFE_STATE, reason=REQUEST_LIMIT
TEST_F(AebMonitorSilTest, S13_DecelerationLimitsPerBrakingState_Ex4) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 15 m ahead  [FN-S19]
    sil_.plant().set_lead(15.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in PARTIAL_BRAKE with 4.2 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.2);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_LIMIT  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kRequestLimit));
}

// Scenario: Braking is allowed only in a braking state #1
// Source:   examples/aeb_monitor/features/justification.feature:109
// Verifies: REQ-AEBM-004
// Example:  s=STANDBY
TEST_F(AebMonitorSilTest, S14_BrakingIsAllowedOnlyInABrakingState_Ex1) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 15 m ahead  [FN-S19]
    sil_.plant().set_lead(15.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in STANDBY with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_OUTSIDE_BRAKING  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kRequestOutsideBraking));
}

// Scenario: Braking is allowed only in a braking state #2
// Source:   examples/aeb_monitor/features/justification.feature:109
// Verifies: REQ-AEBM-004
// Example:  s=WARNING
TEST_F(AebMonitorSilTest, S15_BrakingIsAllowedOnlyInABrakingState_Ex2) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 15 m ahead  [FN-S19]
    sil_.plant().set_lead(15.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in WARNING with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kWarning), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_OUTSIDE_BRAKING  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kRequestOutsideBraking));
}

// Scenario: Braking is allowed only in a braking state #3
// Source:   examples/aeb_monitor/features/justification.feature:109
// Verifies: REQ-AEBM-004
// Example:  s=FAULT
TEST_F(AebMonitorSilTest, S16_BrakingIsAllowedOnlyInABrakingState_Ex3) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 15 m ahead  [FN-S19]
    sil_.plant().set_lead(15.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in FAULT with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kFault), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_OUTSIDE_BRAKING  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kRequestOutsideBraking));
}

// Scenario: Braking that continues after the target left the lane latches SAFE_STATE
// Source:   examples/aeb_monitor/features/justification.feature:126
// Verifies: REQ-AEBM-007
TEST_F(AebMonitorSilTest, S17_BrakingThatContinuesAfterTheTargetLeftTheLaneLatchesSAFESTAT) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 22 m ahead  [FN-S19]
    sil_.plant().set_lead(22.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the lead vehicle leaves the lane  [FN-S15]
    sil_.plant().remove_lead();
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason UNJUSTIFIED_BRAKING  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kUnjustifiedBraking));
    // And the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
}

// Scenario: A new braking onset is justified again after braking ended
// Source:   examples/aeb_monitor/features/justification.feature:140
// Verifies: REQ-AEBM-005
TEST_F(AebMonitorSilTest, S18_ANewBrakingOnsetIsJustifiedAgainAfterBrakingEnded) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a lead vehicle drives 15 m ahead at 20 km/h  [FN-S11]
    sil_.plant().set_lead(15.0, 20.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // And the lead vehicle changes speed to 80 km/h at 5 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(80.0, 5.0);
    // And 3 s pass  [FN-S10]
    sil_.run(3.0);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // When the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason UNJUSTIFIED_BRAKING  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kUnjustifiedBraking));
}

// Scenario: A stopped or frozen command sender latches SAFE_STATE within the timeout #1
// Source:   examples/aeb_monitor/features/faults.feature:6
// Verifies: REQ-AEBM-008, REQ-AEBM-009
// Example:  f=stops
TEST_F(AebMonitorSilTest, S19_AStoppedOrFrozenCommandSenderLatchesSAFESTATEWithinTheTimeou_Ex1) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the command sender stops  [MON-S20]
    sil_.command_fault(ea::plant::NodeFault::kLose, 1000000U);
    // And 0.5 s pass  [FN-S10]
    sil_.run(0.5);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason COMMAND_TIMEOUT  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kCommandTimeout));
    // And the monitor entered SAFE_STATE within 40 ms of the first fault  [MON-S03]
    ASSERT_GE(sil_.actuator().metrics().monitor_safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().monitor_safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 40 + 1e-6);
    // And the permit is withdrawn  [MON-S02]
    EXPECT_EQ(sil_.actuator().monitor().permit, false);
}

// Scenario: A stopped or frozen command sender latches SAFE_STATE within the timeout #2
// Source:   examples/aeb_monitor/features/faults.feature:6
// Verifies: REQ-AEBM-008, REQ-AEBM-009
// Example:  f=freezes
TEST_F(AebMonitorSilTest, S20_AStoppedOrFrozenCommandSenderLatchesSAFESTATEWithinTheTimeou_Ex2) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the command sender freezes  [MON-S20]
    sil_.command_fault(ea::plant::NodeFault::kFreeze, 1000000U);
    // And 0.5 s pass  [FN-S10]
    sil_.run(0.5);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason COMMAND_TIMEOUT  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kCommandTimeout));
    // And the monitor entered SAFE_STATE within 40 ms of the first fault  [MON-S03]
    ASSERT_GE(sil_.actuator().metrics().monitor_safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().monitor_safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 40 + 1e-6);
    // And the permit is withdrawn  [MON-S02]
    EXPECT_EQ(sil_.actuator().monitor().permit, false);
}

// Scenario: Up to two lost or corrupted command frames are tolerated #1
// Source:   examples/aeb_monitor/features/faults.feature:23
// Verifies: REQ-AEBM-008
// Example:  f=lost
TEST_F(AebMonitorSilTest, S21_UpToTwoLostOrCorruptedCommandFramesAreTolerated_Ex1) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the next 2 command frames are lost  [MON-S21]
    sil_.command_fault(ea::plant::NodeFault::kLose, 2U);
    // And 0.5 s pass  [FN-S10]
    sil_.run(0.5);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
}

// Scenario: Up to two lost or corrupted command frames are tolerated #2
// Source:   examples/aeb_monitor/features/faults.feature:23
// Verifies: REQ-AEBM-008
// Example:  f=corrupted
TEST_F(AebMonitorSilTest, S22_UpToTwoLostOrCorruptedCommandFramesAreTolerated_Ex2) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the next 2 command frames are corrupted  [MON-S21]
    sil_.command_fault(ea::plant::NodeFault::kCorrupt, 2U);
    // And 0.5 s pass  [FN-S10]
    sil_.run(0.5);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
}

// Scenario: Without any command the monitor stays in INIT and grants no permit
// Source:   examples/aeb_monitor/features/faults.feature:38
// Verifies: REQ-AEBM-001, REQ-AEBM-008
TEST_F(AebMonitorSilTest, S23_WithoutAnyCommandTheMonitorStaysInINITAndGrantsNoPermit) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // When the command sender stops  [MON-S20]
    sil_.command_fault(ea::plant::NodeFault::kLose, 1000000U);
    // And 1 s pass  [FN-S10]
    sil_.run(1.0);
    // Then the monitor state is INIT  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kInit);
    // And the permit is withdrawn  [MON-S02]
    EXPECT_EQ(sil_.actuator().monitor().permit, false);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
}

// Scenario: A frozen monitor makes the actuator ramp out braking and request a takeover
// Source:   examples/aeb_monitor/features/faults.feature:47
// Verifies: REQ-AEBM-010
TEST_F(AebMonitorSilTest, S24_AFrozenMonitorMakesTheActuatorRampOutBrakingAndRequestATakeo) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 22 m ahead  [FN-S19]
    sil_.plant().set_lead(22.0, 0.0, 0.0);
    // And the AEB controller is in STANDBY with 0 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kStandby), 0.0);
    // When 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking  [AEBM-S01]
    sil_.command(static_cast<double>(ea::plant::AebStatus::kPartialBrake), 4.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the monitor sender freezes  [MON-S22]
    sil_.monitor_fault(ea::plant::NodeFault::kFreeze, 1000000U);
    // And 1 s pass  [FN-S10]
    sil_.run(1.0);
    // Then the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
    // And the actuator entered its safe state within 40 ms of the first fault  [MON-S10]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 40 + 1e-6);
    // And the request was ramped out within 460 ms of the first fault  [MON-S13]
    ASSERT_GE(sil_.actuator().metrics().ramp_out_done_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().ramp_out_done_s - sil_.plant().metrics().first_fault_s) * 1000.0, 460 + 1e-6);
    // And the actuator applies no request  [MON-S08]
    EXPECT_FALSE(sil_.actuator().applying());
}

}  // namespace
}  // namespace adas::aeb_monitor
