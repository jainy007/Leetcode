// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc_monitor (pattern: can_function)
// Inputs:    sha256:aaed2b82deaf8ac2890ea08cf061dfa671e7be14789da61b08e9a29f91792b3a
// Regenerate: python3 -m tools.gen

#include <gtest/gtest.h>

#include "sil/acc_monitor_sil.hpp"

// Closed-loop SIL tests generated from the Gherkin scenarios; each step expands from the step
// catalogues sil/steps/can_function.yaml and sil/steps/features/acc_monitor.yaml.
// Test names are referenced by trace.json.

namespace adas::acc_monitor {
namespace {

class AccMonitorSilTest : public ::testing::Test {
protected:
    AccMonitorSil sil_;
};

// Scenario: Commands inside the envelope are approved and applied
// Source:   examples/acc_monitor/features/envelope.feature:8
// Verifies: REQ-ACCM-001, REQ-ACCM-002, REQ-ACCM-010
TEST_F(AccMonitorSilTest, S01_CommandsInsideTheEnvelopeAreApprovedAndApplied) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the permit is granted  [MON-S02]
    EXPECT_EQ(sil_.actuator().monitor().permit, true);
    // When the controller ramps the acceleration request to 2.0 m/s2 at 2.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 2.0, 2.5);
    // And the controller ramps the deceleration request to 3.5 m/s2 at 5.0 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), -3.5, 5.0);
    // And 1 s pass  [FN-S10]
    sil_.run(1.0);
    // Then the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the actuator applied every command  [MON-S06]
    EXPECT_GT(sil_.actuator().metrics().applied_commands, 0U); EXPECT_EQ(sil_.actuator().metrics().rejected_commands, 0U);
    // And the actuator applies -3.5 m/s2  [MON-S07]
    ASSERT_TRUE(sil_.actuator().applying()); EXPECT_NEAR(sil_.actuator().applied_mps2(), -3.5, 1e-6);
    // And the actuator does not request a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), false);
    // And every monitor frame passed the E2E check  [MON-S14]
    EXPECT_GT(sil_.actuator().metrics().monitor_frames, 0U); EXPECT_EQ(sil_.actuator().metrics().monitor_e2e_errors, 0U);
    // And one monitor frame was sent per 20 ms  [MON-S15]
    EXPECT_EQ(sil_.actuator().metrics().monitor_frames, sil_.plant().metrics().ticks);
}

// Scenario: Requests beyond the limits plus tolerance latch SAFE_STATE #1
// Source:   examples/acc_monitor/features/envelope.feature:25
// Verifies: REQ-ACCM-003
// Example:  dir=acceleration, a=2.1, state=MONITORING, reason=NONE
TEST_F(AccMonitorSilTest, S02_RequestsBeyondTheLimitsPlusToleranceLatchSAFESTATE_Ex1) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the acceleration request to 2.1 m/s2 at 2.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 2.1, 2.5);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor reports the fault reason NONE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kNone));
}

// Scenario: Requests beyond the limits plus tolerance latch SAFE_STATE #2
// Source:   examples/acc_monitor/features/envelope.feature:25
// Verifies: REQ-ACCM-003
// Example:  dir=acceleration, a=2.2, state=SAFE_STATE, reason=REQUEST_LIMIT
TEST_F(AccMonitorSilTest, S03_RequestsBeyondTheLimitsPlusToleranceLatchSAFESTATE_Ex2) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the acceleration request to 2.2 m/s2 at 2.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 2.2, 2.5);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_LIMIT  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kRequestLimit));
}

// Scenario: Requests beyond the limits plus tolerance latch SAFE_STATE #3
// Source:   examples/acc_monitor/features/envelope.feature:25
// Verifies: REQ-ACCM-003
// Example:  dir=deceleration, a=3.6, state=MONITORING, reason=NONE
TEST_F(AccMonitorSilTest, S04_RequestsBeyondTheLimitsPlusToleranceLatchSAFESTATE_Ex3) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the deceleration request to 3.6 m/s2 at 2.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), -3.6, 2.5);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor reports the fault reason NONE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kNone));
}

// Scenario: Requests beyond the limits plus tolerance latch SAFE_STATE #4
// Source:   examples/acc_monitor/features/envelope.feature:25
// Verifies: REQ-ACCM-003
// Example:  dir=deceleration, a=3.7, state=SAFE_STATE, reason=REQUEST_LIMIT
TEST_F(AccMonitorSilTest, S05_RequestsBeyondTheLimitsPlusToleranceLatchSAFESTATE_Ex4) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the deceleration request to 3.7 m/s2 at 2.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), -3.7, 2.5);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_LIMIT  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kRequestLimit));
}

// Scenario: A request above the limit is never applied and SAFE_STATE stays latched
// Source:   examples/acc_monitor/features/envelope.feature:42
// Verifies: REQ-ACCM-003, REQ-ACCM-009
TEST_F(AccMonitorSilTest, S06_ARequestAboveTheLimitIsNeverAppliedAndSAFESTATEStaysLatched) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the acceleration request to 2.5 m/s2 at 2.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 2.5, 2.5);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the permit is withdrawn  [MON-S02]
    EXPECT_EQ(sil_.actuator().monitor().permit, false);
    // And the applied acceleration never exceeded 2.1 m/s2  [MON-S11]
    EXPECT_LE(sil_.actuator().metrics().max_applied_accel_mps2, 2.1 + 1e-9);
    // And the actuator applies no request  [MON-S08]
    EXPECT_FALSE(sil_.actuator().applying());
    // And the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
    // When the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_LIMIT  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kRequestLimit));
    // And the permit is withdrawn  [MON-S02]
    EXPECT_EQ(sil_.actuator().monitor().permit, false);
}

// Scenario: Acceleration and deceleration requested together
// Source:   examples/acc_monitor/features/envelope.feature:60
// Verifies: REQ-ACCM-004
TEST_F(AccMonitorSilTest, S07_AccelerationAndDecelerationRequestedTogether) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller requests 0.05 m/s2 acceleration and 0.05 m/s2 deceleration  [ACCM-S04]
    sil_.command_both(0.05, 0.05);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason ACCEL_AND_BRAKE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kAccelAndBrake));
    // And the applied acceleration never exceeded 0 m/s2  [MON-S11]
    EXPECT_LE(sil_.actuator().metrics().max_applied_accel_mps2, 0.0 + 1e-9);
    // And the applied deceleration never exceeded 0 m/s2  [MON-S12]
    EXPECT_LE(sil_.actuator().metrics().max_applied_decel_mps2, 0.0 + 1e-9);
}

// Scenario: Requests are allowed only while the controller is ACTIVE #1
// Source:   examples/acc_monitor/features/envelope.feature:72
// Verifies: REQ-ACCM-005
// Example:  s=ACTIVE, dir=acceleration, state=MONITORING, reason=NONE
TEST_F(AccMonitorSilTest, S08_RequestsAreAllowedOnlyWhileTheControllerIsACTIVE_Ex1) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller requests 0.05 m/s2 acceleration in state ACTIVE  [ACCM-S03]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.05);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor reports the fault reason NONE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kNone));
}

// Scenario: Requests are allowed only while the controller is ACTIVE #2
// Source:   examples/acc_monitor/features/envelope.feature:72
// Verifies: REQ-ACCM-005
// Example:  s=ACTIVE, dir=deceleration, state=MONITORING, reason=NONE
TEST_F(AccMonitorSilTest, S09_RequestsAreAllowedOnlyWhileTheControllerIsACTIVE_Ex2) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller requests 0.05 m/s2 deceleration in state ACTIVE  [ACCM-S03]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), -0.05);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor reports the fault reason NONE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kNone));
}

// Scenario: Requests are allowed only while the controller is ACTIVE #3
// Source:   examples/acc_monitor/features/envelope.feature:72
// Verifies: REQ-ACCM-005
// Example:  s=STANDBY, dir=acceleration, state=SAFE_STATE, reason=REQUEST_OUTSIDE_ACTIVE
TEST_F(AccMonitorSilTest, S10_RequestsAreAllowedOnlyWhileTheControllerIsACTIVE_Ex3) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is STANDBY without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kStandby), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller requests 0.05 m/s2 acceleration in state STANDBY  [ACCM-S03]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kStandby), 0.05);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_OUTSIDE_ACTIVE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kRequestOutsideActive));
}

// Scenario: Requests are allowed only while the controller is ACTIVE #4
// Source:   examples/acc_monitor/features/envelope.feature:72
// Verifies: REQ-ACCM-005
// Example:  s=OVERRIDE, dir=deceleration, state=SAFE_STATE, reason=REQUEST_OUTSIDE_ACTIVE
TEST_F(AccMonitorSilTest, S11_RequestsAreAllowedOnlyWhileTheControllerIsACTIVE_Ex4) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is OVERRIDE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kOverride), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller requests 0.05 m/s2 deceleration in state OVERRIDE  [ACCM-S03]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kOverride), -0.05);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_OUTSIDE_ACTIVE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kRequestOutsideActive));
}

// Scenario: Requests are allowed only while the controller is ACTIVE #5
// Source:   examples/acc_monitor/features/envelope.feature:72
// Verifies: REQ-ACCM-005
// Example:  s=FAULT, dir=deceleration, state=SAFE_STATE, reason=REQUEST_OUTSIDE_ACTIVE
TEST_F(AccMonitorSilTest, S12_RequestsAreAllowedOnlyWhileTheControllerIsACTIVE_Ex5) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is FAULT without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kFault), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller requests 0.05 m/s2 deceleration in state FAULT  [ACCM-S03]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kFault), -0.05);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_OUTSIDE_ACTIVE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kRequestOutsideActive));
}

// Scenario: Requests are allowed only while the controller is ACTIVE #6
// Source:   examples/acc_monitor/features/envelope.feature:72
// Verifies: REQ-ACCM-005
// Example:  s=OFF, dir=acceleration, state=SAFE_STATE, reason=REQUEST_OUTSIDE_ACTIVE
TEST_F(AccMonitorSilTest, S13_RequestsAreAllowedOnlyWhileTheControllerIsACTIVE_Ex6) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is OFF without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kOff), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller requests 0.05 m/s2 acceleration in state OFF  [ACCM-S03]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kOff), 0.05);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_OUTSIDE_ACTIVE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kRequestOutsideActive));
}

// Scenario: Request rate limits #1
// Source:   examples/acc_monitor/features/envelope.feature:91
// Verifies: REQ-ACCM-006
// Example:  dir=acceleration, j=3.5, state=MONITORING, reason=NONE
TEST_F(AccMonitorSilTest, S14_RequestRateLimits_Ex1) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the acceleration request to 1.0 m/s2 at 3.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 1.0, 3.5);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor reports the fault reason NONE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kNone));
}

// Scenario: Request rate limits #2
// Source:   examples/acc_monitor/features/envelope.feature:91
// Verifies: REQ-ACCM-006
// Example:  dir=acceleration, j=4.0, state=SAFE_STATE, reason=REQUEST_RATE
TEST_F(AccMonitorSilTest, S15_RequestRateLimits_Ex2) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the acceleration request to 1.0 m/s2 at 4.0 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 1.0, 4.0);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_RATE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kRequestRate));
}

// Scenario: Request rate limits #3
// Source:   examples/acc_monitor/features/envelope.feature:91
// Verifies: REQ-ACCM-006
// Example:  dir=deceleration, j=6.0, state=MONITORING, reason=NONE
TEST_F(AccMonitorSilTest, S16_RequestRateLimits_Ex3) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the deceleration request to 1.0 m/s2 at 6.0 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), -1.0, 6.0);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor reports the fault reason NONE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kNone));
}

// Scenario: Request rate limits #4
// Source:   examples/acc_monitor/features/envelope.feature:91
// Verifies: REQ-ACCM-006
// Example:  dir=deceleration, j=6.5, state=SAFE_STATE, reason=REQUEST_RATE
TEST_F(AccMonitorSilTest, S17_RequestRateLimits_Ex4) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the deceleration request to 1.0 m/s2 at 6.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), -1.0, 6.5);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_RATE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kRequestRate));
}

// Scenario: The rate limit scales with the time since the last valid command
// Source:   examples/acc_monitor/features/envelope.feature:108
// Verifies: REQ-ACCM-006
TEST_F(AccMonitorSilTest, S18_TheRateLimitScalesWithTheTimeSinceTheLastValidCommand) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the next 2 command frames are lost  [MON-S21]
    sil_.command_fault(ea::plant::NodeFault::kLose, 2U);
    // And the controller ramps the acceleration request to 1.0 m/s2 at 3.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 1.0, 3.5);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
}

// Scenario: Acceleration toward a close in-lane vehicle latches SAFE_STATE after 0.5 s
// Source:   examples/acc_monitor/features/envelope.feature:119
// Verifies: REQ-ACCM-007
TEST_F(AccMonitorSilTest, S19_AccelerationTowardACloseInLaneVehicleLatchesSAFESTATEAfter05) {
    // Given the ego vehicle drives at 60 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(60.0);
    // And a stationary vehicle stands 30 m ahead  [FN-S19]
    sil_.plant().set_lead(30.0, 0.0, 0.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the acceleration request to 0.5 m/s2 at 2.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 0.5, 2.5);
    // And 1 s pass  [FN-S10]
    sil_.run(1.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason UNINTENDED_ACCEL  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kUnintendedAccel));
    // And the monitor entered SAFE_STATE between 0.68 s and 0.72 s  [MON-S04]
    EXPECT_GE(sil_.actuator().metrics().monitor_safe_state_s, 0.68 - 1e-9); EXPECT_LE(sil_.actuator().metrics().monitor_safe_state_s, 0.72 + 1e-9);
    // And the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
}

// Scenario: Acceleration is allowed when no in-lane vehicle is closer than 2 s #1
// Source:   examples/acc_monitor/features/envelope.feature:132
// Verifies: REQ-ACCM-007
// Example:  d=80, lat=0.0
TEST_F(AccMonitorSilTest, S20_AccelerationIsAllowedWhenNoInLaneVehicleIsCloserThan2S_Ex1) {
    // Given the ego vehicle drives at 60 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(60.0);
    // And a vehicle drives 80 m ahead at 0 km/h with 0.0 m lateral offset  [FN-S12]
    sil_.plant().set_lead(80.0, 0.0, 0.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the acceleration request to 0.5 m/s2 at 2.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 0.5, 2.5);
    // And 1 s pass  [FN-S10]
    sil_.run(1.0);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
}

// Scenario: Acceleration is allowed when no in-lane vehicle is closer than 2 s #2
// Source:   examples/acc_monitor/features/envelope.feature:132
// Verifies: REQ-ACCM-007
// Example:  d=30, lat=2.0
TEST_F(AccMonitorSilTest, S21_AccelerationIsAllowedWhenNoInLaneVehicleIsCloserThan2S_Ex2) {
    // Given the ego vehicle drives at 60 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(60.0);
    // And a vehicle drives 30 m ahead at 0 km/h with 2.0 m lateral offset  [FN-S12]
    sil_.plant().set_lead(30.0, 0.0, 2.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the acceleration request to 0.5 m/s2 at 2.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 0.5, 2.5);
    // And 1 s pass  [FN-S10]
    sil_.run(1.0);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
}

// Scenario: Acceleration without a valid radar latches SAFE_STATE
// Source:   examples/acc_monitor/features/envelope.feature:148
// Verifies: REQ-ACCM-007
TEST_F(AccMonitorSilTest, S22_AccelerationWithoutAValidRadarLatchesSAFESTATE) {
    // Given the ego vehicle drives at 60 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(60.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the radar sender pauses for 50 frames  [FN-S18]
    sil_.plant().skip_frames(ea::plant::Stream::kRadar, 50U);
    // And the controller ramps the acceleration request to 0.5 m/s2 at 2.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 0.5, 2.5);
    // And 1 s pass  [FN-S10]
    sil_.run(1.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason UNINTENDED_ACCEL  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kUnintendedAccel));
}

// Scenario: Short acceleration bursts toward a close vehicle do not add up
// Source:   examples/acc_monitor/features/envelope.feature:159
// Verifies: REQ-ACCM-007
TEST_F(AccMonitorSilTest, S23_ShortAccelerationBurstsTowardACloseVehicleDoNotAddUp) {
    // Given the ego vehicle drives at 60 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(60.0);
    // And a stationary vehicle stands 30 m ahead  [FN-S19]
    sil_.plant().set_lead(30.0, 0.0, 0.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the acceleration request to 0.1 m/s2 at 2.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 0.1, 2.5);
    // And 0.3 s pass  [FN-S10]
    sil_.run(0.3);
    // And the controller ramps the acceleration request to 0.0 m/s2 at 5.0 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 0.0, 5.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // And the controller ramps the acceleration request to 0.1 m/s2 at 2.5 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 0.1, 2.5);
    // And 0.3 s pass  [FN-S10]
    sil_.run(0.3);
    // And the controller ramps the acceleration request to 0.0 m/s2 at 5.0 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), 0.0, 5.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the monitor state is MONITORING  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kMonitoring);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
}

// Scenario: A stopped or frozen command sender latches SAFE_STATE within the timeout #1
// Source:   examples/acc_monitor/features/faults.feature:7
// Verifies: REQ-ACCM-008, REQ-ACCM-009
// Example:  f=stops
TEST_F(AccMonitorSilTest, S24_AStoppedOrFrozenCommandSenderLatchesSAFESTATEWithinTheTimeou_Ex1) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the command sender stops  [MON-S20]
    sil_.command_fault(ea::plant::NodeFault::kLose, 1000000U);
    // And 0.5 s pass  [FN-S10]
    sil_.run(0.5);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason COMMAND_TIMEOUT  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kCommandTimeout));
    // And the monitor entered SAFE_STATE within 40 ms of the first fault  [MON-S03]
    ASSERT_GE(sil_.actuator().metrics().monitor_safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().monitor_safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 40 + 1e-6);
    // And the permit is withdrawn  [MON-S02]
    EXPECT_EQ(sil_.actuator().monitor().permit, false);
}

// Scenario: A stopped or frozen command sender latches SAFE_STATE within the timeout #2
// Source:   examples/acc_monitor/features/faults.feature:7
// Verifies: REQ-ACCM-008, REQ-ACCM-009
// Example:  f=freezes
TEST_F(AccMonitorSilTest, S25_AStoppedOrFrozenCommandSenderLatchesSAFESTATEWithinTheTimeou_Ex2) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the command sender freezes  [MON-S20]
    sil_.command_fault(ea::plant::NodeFault::kFreeze, 1000000U);
    // And 0.5 s pass  [FN-S10]
    sil_.run(0.5);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason COMMAND_TIMEOUT  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kCommandTimeout));
    // And the monitor entered SAFE_STATE within 40 ms of the first fault  [MON-S03]
    ASSERT_GE(sil_.actuator().metrics().monitor_safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().monitor_safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 40 + 1e-6);
    // And the permit is withdrawn  [MON-S02]
    EXPECT_EQ(sil_.actuator().monitor().permit, false);
}

// Scenario: Up to two lost or corrupted command frames are tolerated #1
// Source:   examples/acc_monitor/features/faults.feature:24
// Verifies: REQ-ACCM-008
// Example:  f=lost
TEST_F(AccMonitorSilTest, S26_UpToTwoLostOrCorruptedCommandFramesAreTolerated_Ex1) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
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
// Source:   examples/acc_monitor/features/faults.feature:24
// Verifies: REQ-ACCM-008
// Example:  f=corrupted
TEST_F(AccMonitorSilTest, S27_UpToTwoLostOrCorruptedCommandFramesAreTolerated_Ex2) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
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
// Source:   examples/acc_monitor/features/faults.feature:39
// Verifies: REQ-ACCM-001, REQ-ACCM-008
TEST_F(AccMonitorSilTest, S28_WithoutAnyCommandTheMonitorStaysInINITAndGrantsNoPermit) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
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
    // And the actuator applies no request  [MON-S08]
    EXPECT_FALSE(sil_.actuator().applying());
}

// Scenario: A lost monitor makes the actuator ramp out braking and request a takeover
// Source:   examples/acc_monitor/features/faults.feature:49
// Verifies: REQ-ACCM-010
TEST_F(AccMonitorSilTest, S29_ALostMonitorMakesTheActuatorRampOutBrakingAndRequestATakeove) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the controller is ACTIVE without request  [ACCM-S01]
    sil_.command(static_cast<double>(ea::plant::AccStatus::kActive), 0.0);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // And the controller ramps the deceleration request to 2.0 m/s2 at 5.0 m/s3  [ACCM-S02]
    sil_.ramp_request(static_cast<double>(ea::plant::AccStatus::kActive), -2.0, 5.0);
    // And the monitor sender stops  [MON-S22]
    sil_.monitor_fault(ea::plant::NodeFault::kLose, 1000000U);
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
}  // namespace adas::acc_monitor
