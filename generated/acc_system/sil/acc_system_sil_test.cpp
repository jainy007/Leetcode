// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc_system (pattern: can_system)
// Inputs:    sha256:bcf75ddb317884072eb7fb43b43c3450882edb36edd84b7a99e749e56ad81f26
// Regenerate: python3 -m tools.gen

#include <gtest/gtest.h>

#include "sil/acc_system_sil.hpp"

// System SIL tests generated from the Gherkin scenarios; each step expands from the step
// catalogues (sil/steps/can_function.yaml, monitored_actuator.yaml, can_system.yaml and the
// feature catalogues of both nodes). Test names are referenced by trace.json.

namespace adas::acc_system {
namespace {

namespace cmd = ea::plant::can::acc_command;  // command signal layout for override faults

class AccSystemSilTest : public ::testing::Test {
protected:
    AccSystemSil sil_;
};

// Scenario: An acceleration request above the limit is never applied
// Source:   examples/acc_system/features/safety_goals.feature:8
// Verifies: REQ-ACCS-001
TEST_F(AccSystemSilTest, S01_AnAccelerationRequestAboveTheLimitIsNeverApplied) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 2 s pass  [FN-S10]
    sil_.run(2.0);
    // And the controller sends AccelRequest = 3.0 with a valid CRC for 100 frames  [SYS-S01]
    sil_.override_command(cmd::kAccelRequest, 3.0, 100U);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_LIMIT  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kRequestLimit));
    // And the actuator entered its safe state within the fault detection budget of REQ-ACCS-001  [SYS-S02]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 100.0 + 1e-6);
    // And the applied acceleration never exceeded 2.1 m/s2  [MON-S11]
    EXPECT_LE(sil_.actuator().metrics().max_applied_accel_mps2, 2.1 + 1e-9);
    // And the actual acceleration of the vehicle never exceeded 2.1 m/s2  [MON-S16]
    EXPECT_LE(sil_.plant().metrics().max_accel_mps2, 2.1 + 1e-9);
    // And the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
}

// Scenario: An acceleration step within the limit but above the jerk envelope is never applied
// Source:   examples/acc_system/features/safety_goals.feature:22
// Verifies: REQ-ACCS-001
TEST_F(AccSystemSilTest, S02_AnAccelerationStepWithinTheLimitButAboveTheJerkEnvelopeIsNev) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 2 s pass  [FN-S10]
    sil_.run(2.0);
    // And the controller sends AccelRequest = 1.0 with a valid CRC for 100 frames  [SYS-S01]
    sil_.override_command(cmd::kAccelRequest, 1.0, 100U);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_RATE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kRequestRate));
    // And the actuator entered its safe state within the fault detection budget of REQ-ACCS-001  [SYS-S02]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 100.0 + 1e-6);
    // And the applied acceleration never exceeded 0.1 m/s2  [MON-S11]
    EXPECT_LE(sil_.actuator().metrics().max_applied_accel_mps2, 0.1 + 1e-9);
}

// Scenario: A deceleration request above the limit is never applied
// Source:   examples/acc_system/features/safety_goals.feature:34
// Verifies: REQ-ACCS-002
TEST_F(AccSystemSilTest, S03_ADecelerationRequestAboveTheLimitIsNeverApplied) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 2 s pass  [FN-S10]
    sil_.run(2.0);
    // And the controller sends BrakeRequest = 6.0 with a valid CRC for 100 frames  [SYS-S01]
    sil_.override_command(cmd::kBrakeRequest, 6.0, 100U);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_LIMIT  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kRequestLimit));
    // And the actuator entered its safe state within the fault detection budget of REQ-ACCS-002  [SYS-S02]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 100.0 + 1e-6);
    // And the applied deceleration never exceeded 3.6 m/s2  [MON-S12]
    EXPECT_LE(sil_.actuator().metrics().max_applied_decel_mps2, 3.6 + 1e-9);
    // And the actual deceleration of the vehicle never exceeded 3.6 m/s2  [MON-S16]
    EXPECT_LE(sil_.plant().metrics().max_decel_mps2, 3.6 + 1e-9);
}

// Scenario: Acceleration kept up while the controller brakes for a cut-in is caught
// Source:   examples/acc_system/features/safety_goals.feature:47
// Verifies: REQ-ACCS-003
TEST_F(AccSystemSilTest, S04_AccelerationKeptUpWhileTheControllerBrakesForACutInIsCaught) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 2 s pass  [FN-S10]
    sil_.run(2.0);
    // And the controller sends AccelRequest = 0.06 with a valid CRC for 2000 frames  [SYS-S01]
    sil_.override_command(cmd::kAccelRequest, 0.06, 2000U);
    // And a vehicle cuts in 25 m ahead at 70 km/h  [FN-S14]
    sil_.plant().cut_in(25.0, 70.0);
    // And 3 s pass  [FN-S10]
    sil_.run(3.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the actuator entered its safe state within the fault detection budget of REQ-ACCS-003  [SYS-S02]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 900.0 + 1e-6);
    // And the applied acceleration never exceeded 0.06 m/s2  [MON-S11]
    EXPECT_LE(sil_.actuator().metrics().max_applied_accel_mps2, 0.06 + 1e-9);
    // And the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
}

// Scenario: A stopped or frozen controller while braking behind a slower vehicle #1
// Source:   examples/acc_system/features/safety_goals.feature:60
// Verifies: REQ-ACCS-004, REQ-ACCS-005
// Example:  f=stops
TEST_F(AccSystemSilTest, S05_AStoppedOrFrozenControllerWhileBrakingBehindASlowerVehicle_Ex1) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And a lead vehicle drives 50 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(50.0, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 3 s pass  [FN-S10]
    sil_.run(3.0);
    // And the lead vehicle changes speed to 70 km/h at 1.5 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(70.0, 1.5);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then braking is requested  [ACC-S25]
    EXPECT_GT(sil_.feature().last_command().brake_raw, 0);
    // When the command sender stops  [MON-S20]
    sil_.command_fault(ea::plant::NodeFault::kLose, 1000000U);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason COMMAND_TIMEOUT  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kCommandTimeout));
    // And the actuator entered its safe state within the fault detection budget of REQ-ACCS-004  [SYS-S02]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 100.0 + 1e-6);
    // And the request was ramped out within 800 ms of the first fault  [MON-S13]
    ASSERT_GE(sil_.actuator().metrics().ramp_out_done_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().ramp_out_done_s - sil_.plant().metrics().first_fault_s) * 1000.0, 800 + 1e-6);
    // And the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
}

// Scenario: A stopped or frozen controller while braking behind a slower vehicle #2
// Source:   examples/acc_system/features/safety_goals.feature:60
// Verifies: REQ-ACCS-004, REQ-ACCS-005
// Example:  f=freezes
TEST_F(AccSystemSilTest, S06_AStoppedOrFrozenControllerWhileBrakingBehindASlowerVehicle_Ex2) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And a lead vehicle drives 50 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(50.0, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 3 s pass  [FN-S10]
    sil_.run(3.0);
    // And the lead vehicle changes speed to 70 km/h at 1.5 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(70.0, 1.5);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then braking is requested  [ACC-S25]
    EXPECT_GT(sil_.feature().last_command().brake_raw, 0);
    // When the command sender freezes  [MON-S20]
    sil_.command_fault(ea::plant::NodeFault::kFreeze, 1000000U);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason COMMAND_TIMEOUT  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kCommandTimeout));
    // And the actuator entered its safe state within the fault detection budget of REQ-ACCS-004  [SYS-S02]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 100.0 + 1e-6);
    // And the request was ramped out within 800 ms of the first fault  [MON-S13]
    ASSERT_GE(sil_.actuator().metrics().ramp_out_done_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().ramp_out_done_s - sil_.plant().metrics().first_fault_s) * 1000.0, 800 + 1e-6);
    // And the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
}

// Scenario: A controller reporting STANDBY while requesting braking is caught
// Source:   examples/acc_system/features/safety_goals.feature:82
// Verifies: REQ-ACCS-004
TEST_F(AccSystemSilTest, S07_AControllerReportingSTANDBYWhileRequestingBrakingIsCaught) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And a lead vehicle drives 50 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(50.0, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 3 s pass  [FN-S10]
    sil_.run(3.0);
    // And the lead vehicle changes speed to 70 km/h at 1.5 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(70.0, 1.5);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // And the controller sends AccStatus = 1 with a valid CRC for 100 frames  [SYS-S01]
    sil_.override_command(cmd::kAccStatus, 1.0, 100U);
    // And 1 s pass  [FN-S10]
    sil_.run(1.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_OUTSIDE_ACTIVE  [ACCM-S05]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AccMonitorReason::kRequestOutsideActive));
    // And the actuator entered its safe state within the fault detection budget of REQ-ACCS-004  [SYS-S02]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 100.0 + 1e-6);
}

// Scenario: A lost monitor makes the actuator ramp out and request a takeover
// Source:   examples/acc_system/features/safety_goals.feature:96
// Verifies: REQ-ACCS-005
TEST_F(AccSystemSilTest, S08_ALostMonitorMakesTheActuatorRampOutAndRequestATakeover) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And a lead vehicle drives 50 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(50.0, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 3 s pass  [FN-S10]
    sil_.run(3.0);
    // And the lead vehicle changes speed to 70 km/h at 1.5 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(70.0, 1.5);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // And the monitor sender stops  [MON-S22]
    sil_.monitor_fault(ea::plant::NodeFault::kLose, 1000000U);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
    // And the actuator entered its safe state within 40 ms of the first fault  [MON-S10]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 40 + 1e-6);
    // And the actuator applies no request  [MON-S08]
    EXPECT_FALSE(sil_.actuator().applying());
}

// Scenario: Two corrupted command frames neither stop ACC nor trigger the monitor
// Source:   examples/acc_system/features/safety_goals.feature:110
// Verifies: REQ-ACCS-006
TEST_F(AccSystemSilTest, S09_TwoCorruptedCommandFramesNeitherStopACCNorTriggerTheMonitor) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And a lead vehicle drives 50 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(50.0, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 3 s pass  [FN-S10]
    sil_.run(3.0);
    // And the next 2 command frames are corrupted  [MON-S21]
    sil_.command_fault(ea::plant::NodeFault::kCorrupt, 2U);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
    // And the actuator does not request a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), false);
}

// Scenario: Accelerate to a higher set speed
// Source:   examples/acc_system/features/availability.feature:6
// Verifies: REQ-ACCS-006
TEST_F(AccSystemSilTest, S10_AccelerateToAHigherSetSpeed) {
    // Given the ego vehicle drives at 80 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(80.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver presses the speed up button 8 times  [FN-S05]
    sil_.press(ea::plant::Button::kSpeedUp, 8);
    // And 40 s pass  [FN-S10]
    sil_.run(40.0);
    // Then the ego speed is 120 km/h within 1 km/h  [FN-S40]
    EXPECT_NEAR(sil_.plant().ego_speed_kph(), 120.0, 1.0);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the actuator applied every command  [MON-S06]
    EXPECT_GT(sil_.actuator().metrics().applied_commands, 0U); EXPECT_EQ(sil_.actuator().metrics().rejected_commands, 0U);
}

// Scenario: Follow a slower vehicle at the default time gap
// Source:   examples/acc_system/features/availability.feature:16
// Verifies: REQ-ACCS-006
TEST_F(AccSystemSilTest, S11_FollowASlowerVehicleAtTheDefaultTimeGap) {
    // Given the ego vehicle drives at 120 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(120.0);
    // And a lead vehicle drives 80 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(80.0, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 60 s pass  [FN-S10]
    sil_.run(60.0);
    // Then the distance to the lead vehicle is 46.7 m within 2 m  [FN-S43]
    ASSERT_TRUE(sil_.plant().lead_present()); EXPECT_NEAR(sil_.plant().distance_m(), 46.7, 2.0);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the actuator applied every command  [MON-S06]
    EXPECT_GT(sil_.actuator().metrics().applied_commands, 0U); EXPECT_EQ(sil_.actuator().metrics().rejected_commands, 0U);
}

// Scenario: Brake for a vehicle cutting in
// Source:   examples/acc_system/features/availability.feature:26
// Verifies: REQ-ACCS-006
TEST_F(AccSystemSilTest, S12_BrakeForAVehicleCuttingIn) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 2 s pass  [FN-S10]
    sil_.run(2.0);
    // And a vehicle cuts in 25 m ahead at 70 km/h  [FN-S14]
    sil_.plant().cut_in(25.0, 70.0);
    // And 20 s pass  [FN-S10]
    sil_.run(20.0);
    // Then no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the actuator applied every command  [MON-S06]
    EXPECT_GT(sil_.actuator().metrics().applied_commands, 0U); EXPECT_EQ(sil_.actuator().metrics().rejected_commands, 0U);
}

// Scenario: Follow a lead vehicle braking to a lower speed
// Source:   examples/acc_system/features/availability.feature:37
// Verifies: REQ-ACCS-006
TEST_F(AccSystemSilTest, S13_FollowALeadVehicleBrakingToALowerSpeed) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And a lead vehicle drives 50 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(50.0, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 3 s pass  [FN-S10]
    sil_.run(3.0);
    // And the lead vehicle changes speed to 40 km/h at 3 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(40.0, 3.0);
    // And 30 s pass  [FN-S10]
    sil_.run(30.0);
    // Then no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the ego speed is 40 km/h within 1 km/h  [FN-S40]
    EXPECT_NEAR(sil_.plant().ego_speed_kph(), 40.0, 1.0);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the actuator applied every command  [MON-S06]
    EXPECT_GT(sil_.actuator().metrics().applied_commands, 0U); EXPECT_EQ(sil_.actuator().metrics().rejected_commands, 0U);
}

// Scenario: Driver override, cancel and resume
// Source:   examples/acc_system/features/availability.feature:50
// Verifies: REQ-ACCS-006
TEST_F(AccSystemSilTest, S14_DriverOverrideCancelAndResume) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver presses the accelerator pedal to 30 %  [FN-S07]
    sil_.accelerator(30.0);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // And the driver releases the accelerator pedal  [FN-S08]
    sil_.accelerator(0.0);
    // And 5 s pass  [FN-S10]
    sil_.run(5.0);
    // And the driver presses the brake pedal  [FN-S06]
    sil_.brake(true);
    // And the driver releases the brake pedal  [FN-S06]
    sil_.brake(false);
    // And the driver presses the resume button  [FN-S04]
    sil_.press(ea::plant::Button::kResume, 1);
    // And 10 s pass  [FN-S10]
    sil_.run(10.0);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the actuator applied every command  [MON-S06]
    EXPECT_GT(sil_.actuator().metrics().applied_commands, 0U); EXPECT_EQ(sil_.actuator().metrics().rejected_commands, 0U);
}

}  // namespace
}  // namespace adas::acc_system
