// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb_system (pattern: can_system)
// Inputs:    sha256:178716b09b759745eca874eeb1b20fa3a3e080bfff379eaf556512fc8cfefc60
// Regenerate: python3 -m tools.gen

#include <gtest/gtest.h>

#include "sil/aeb_system_sil.hpp"

// System SIL tests generated from the Gherkin scenarios; each step expands from the step
// catalogues (sil/steps/can_function.yaml, monitored_actuator.yaml, can_system.yaml and the
// feature catalogues of both nodes). Test names are referenced by trace.json.

namespace adas::aeb_system {
namespace {

namespace cmd = ea::plant::can::aeb_command;  // command signal layout for override faults

class AebSystemSilTest : public ::testing::Test {
protected:
    AebSystemSil sil_;
};

// Scenario: Braking requested on a free road is never applied #1
// Source:   examples/aeb_system/features/safety_goals.feature:7
// Verifies: REQ-AEBS-001
// Example:  a=4.0
TEST_F(AebSystemSilTest, S01_BrakingRequestedOnAFreeRoadIsNeverApplied_Ex1) {
    // Given the ego vehicle drives at 80 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(80.0);
    // When 1 s pass  [FN-S10]
    sil_.run(1.0);
    // And the controller sends AebBrakeRequest = 4.0 with a valid CRC for 100 frames  [SYS-S01]
    sil_.override_command(cmd::kAebBrakeRequest, 4.0, 100U);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_OUTSIDE_BRAKING  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kRequestOutsideBraking));
    // And the actuator entered its safe state within the fault detection budget of REQ-AEBS-001  [SYS-S02]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 100.0 + 1e-6);
    // And the applied deceleration never exceeded 0 m/s2  [MON-S12]
    EXPECT_LE(sil_.actuator().metrics().max_applied_decel_mps2, 0.0 + 1e-9);
    // And the actual deceleration of the vehicle never exceeded 0.1 m/s2  [MON-S16]
    EXPECT_LE(sil_.plant().metrics().max_decel_mps2, 0.1 + 1e-9);
    // And the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
}

// Scenario: Braking requested on a free road is never applied #2
// Source:   examples/aeb_system/features/safety_goals.feature:7
// Verifies: REQ-AEBS-001
// Example:  a=8.0
TEST_F(AebSystemSilTest, S02_BrakingRequestedOnAFreeRoadIsNeverApplied_Ex2) {
    // Given the ego vehicle drives at 80 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(80.0);
    // When 1 s pass  [FN-S10]
    sil_.run(1.0);
    // And the controller sends AebBrakeRequest = 8.0 with a valid CRC for 100 frames  [SYS-S01]
    sil_.override_command(cmd::kAebBrakeRequest, 8.0, 100U);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_OUTSIDE_BRAKING  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kRequestOutsideBraking));
    // And the actuator entered its safe state within the fault detection budget of REQ-AEBS-001  [SYS-S02]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 100.0 + 1e-6);
    // And the applied deceleration never exceeded 0 m/s2  [MON-S12]
    EXPECT_LE(sil_.actuator().metrics().max_applied_decel_mps2, 0.0 + 1e-9);
    // And the actual deceleration of the vehicle never exceeded 0.1 m/s2  [MON-S16]
    EXPECT_LE(sil_.plant().metrics().max_decel_mps2, 0.1 + 1e-9);
    // And the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
}

// Scenario: Full braking requested during a warning is never applied
// Source:   examples/aeb_system/features/safety_goals.feature:25
// Verifies: REQ-AEBS-001
TEST_F(AebSystemSilTest, S03_FullBrakingRequestedDuringAWarningIsNeverApplied) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 60 m ahead  [FN-S19]
    sil_.plant().set_lead(60.0, 0.0, 0.0);
    // When 1.6 s pass  [FN-S10]
    sil_.run(1.6);
    // Then the AEB state is WARNING  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kWarning);
    // When the controller sends AebBrakeRequest = 8.0 with a valid CRC for 5 frames  [SYS-S01]
    sil_.override_command(cmd::kAebBrakeRequest, 8.0, 5U);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the actuator entered its safe state within the fault detection budget of REQ-AEBS-001  [SYS-S02]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 100.0 + 1e-6);
    // And the applied deceleration never exceeded 0 m/s2  [MON-S12]
    EXPECT_LE(sil_.actuator().metrics().max_applied_decel_mps2, 0.0 + 1e-9);
}

// Scenario: Braking above the full braking level is never applied
// Source:   examples/aeb_system/features/safety_goals.feature:37
// Verifies: REQ-AEBS-001
TEST_F(AebSystemSilTest, S04_BrakingAboveTheFullBrakingLevelIsNeverApplied) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 60 m ahead  [FN-S19]
    sil_.plant().set_lead(60.0, 0.0, 0.0);
    // When 4 s pass  [FN-S10]
    sil_.run(4.0);
    // Then the AEB state is FULL_BRAKE  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kFullBrake);
    // When the controller sends AebBrakeRequest = 9.5 with a valid CRC for 100 frames  [SYS-S01]
    sil_.override_command(cmd::kAebBrakeRequest, 9.5, 100U);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason REQUEST_LIMIT  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kRequestLimit));
    // And the actuator entered its safe state within the fault detection budget of REQ-AEBS-001  [SYS-S02]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 100.0 + 1e-6);
    // And the applied deceleration never exceeded 8.1 m/s2  [MON-S12]
    EXPECT_LE(sil_.actuator().metrics().max_applied_decel_mps2, 8.1 + 1e-9);
}

// Scenario: A stopped or frozen controller during full braking is ramped out #1
// Source:   examples/aeb_system/features/safety_goals.feature:50
// Verifies: REQ-AEBS-002, REQ-AEBS-003
// Example:  f=stops
TEST_F(AebSystemSilTest, S05_AStoppedOrFrozenControllerDuringFullBrakingIsRampedOut_Ex1) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 60 m ahead  [FN-S19]
    sil_.plant().set_lead(60.0, 0.0, 0.0);
    // When 4 s pass  [FN-S10]
    sil_.run(4.0);
    // Then the AEB state is FULL_BRAKE  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kFullBrake);
    // When the command sender stops  [MON-S20]
    sil_.command_fault(ea::plant::NodeFault::kLose, 1000000U);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason COMMAND_TIMEOUT  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kCommandTimeout));
    // And the actuator entered its safe state within the fault detection budget of REQ-AEBS-002  [SYS-S02]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 100.0 + 1e-6);
    // And the request was ramped out within 900 ms of the first fault  [MON-S13]
    ASSERT_GE(sil_.actuator().metrics().ramp_out_done_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().ramp_out_done_s - sil_.plant().metrics().first_fault_s) * 1000.0, 900 + 1e-6);
    // And the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
}

// Scenario: A stopped or frozen controller during full braking is ramped out #2
// Source:   examples/aeb_system/features/safety_goals.feature:50
// Verifies: REQ-AEBS-002, REQ-AEBS-003
// Example:  f=freezes
TEST_F(AebSystemSilTest, S06_AStoppedOrFrozenControllerDuringFullBrakingIsRampedOut_Ex2) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 60 m ahead  [FN-S19]
    sil_.plant().set_lead(60.0, 0.0, 0.0);
    // When 4 s pass  [FN-S10]
    sil_.run(4.0);
    // Then the AEB state is FULL_BRAKE  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kFullBrake);
    // When the command sender freezes  [MON-S20]
    sil_.command_fault(ea::plant::NodeFault::kFreeze, 1000000U);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the monitor state is SAFE_STATE  [MON-S01]
    EXPECT_EQ(sil_.actuator().monitor().status, ea::plant::MonitorStatus::kSafeState);
    // And the monitor reports the fault reason COMMAND_TIMEOUT  [AEBM-S02]
    EXPECT_EQ(sil_.actuator().monitor().reason, static_cast<std::uint32_t>(ea::plant::can::AebMonitorReason::kCommandTimeout));
    // And the actuator entered its safe state within the fault detection budget of REQ-AEBS-002  [SYS-S02]
    ASSERT_GE(sil_.actuator().metrics().safe_state_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().safe_state_s - sil_.plant().metrics().first_fault_s) * 1000.0, 100.0 + 1e-6);
    // And the request was ramped out within 900 ms of the first fault  [MON-S13]
    ASSERT_GE(sil_.actuator().metrics().ramp_out_done_s, 0.0); ASSERT_GE(sil_.plant().metrics().first_fault_s, 0.0); EXPECT_LE((sil_.actuator().metrics().ramp_out_done_s - sil_.plant().metrics().first_fault_s) * 1000.0, 900 + 1e-6);
    // And the actuator requests a driver takeover  [MON-S09]
    EXPECT_EQ(sil_.actuator().takeover_request(), true);
}

// Scenario: A lost monitor during braking makes the actuator ramp out
// Source:   examples/aeb_system/features/safety_goals.feature:69
// Verifies: REQ-AEBS-003
TEST_F(AebSystemSilTest, S07_ALostMonitorDuringBrakingMakesTheActuatorRampOut) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 60 m ahead  [FN-S19]
    sil_.plant().set_lead(60.0, 0.0, 0.0);
    // When 4 s pass  [FN-S10]
    sil_.run(4.0);
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

// Scenario: A stationary vehicle is still avoided #1
// Source:   examples/aeb_system/features/availability.feature:6
// Verifies: REQ-AEBS-004
// Example:  v=20, d=40
TEST_F(AebSystemSilTest, S08_AStationaryVehicleIsStillAvoided_Ex1) {
    // Given the ego vehicle drives at 20 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(20.0);
    // And a stationary vehicle stands 40 m ahead  [FN-S19]
    sil_.plant().set_lead(40.0, 0.0, 0.0);
    // When 15 s pass  [FN-S10]
    sil_.run(15.0);
    // Then no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the ego vehicle has stopped  [FN-S47]
    EXPECT_LT(sil_.plant().ego_speed_kph(), 0.5);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the actuator applied every command  [MON-S06]
    EXPECT_GT(sil_.actuator().metrics().applied_commands, 0U); EXPECT_EQ(sil_.actuator().metrics().rejected_commands, 0U);
}

// Scenario: A stationary vehicle is still avoided #2
// Source:   examples/aeb_system/features/availability.feature:6
// Verifies: REQ-AEBS-004
// Example:  v=40, d=60
TEST_F(AebSystemSilTest, S09_AStationaryVehicleIsStillAvoided_Ex2) {
    // Given the ego vehicle drives at 40 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(40.0);
    // And a stationary vehicle stands 60 m ahead  [FN-S19]
    sil_.plant().set_lead(60.0, 0.0, 0.0);
    // When 15 s pass  [FN-S10]
    sil_.run(15.0);
    // Then no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the ego vehicle has stopped  [FN-S47]
    EXPECT_LT(sil_.plant().ego_speed_kph(), 0.5);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the actuator applied every command  [MON-S06]
    EXPECT_GT(sil_.actuator().metrics().applied_commands, 0U); EXPECT_EQ(sil_.actuator().metrics().rejected_commands, 0U);
}

// Scenario: A stationary vehicle is still avoided #3
// Source:   examples/aeb_system/features/availability.feature:6
// Verifies: REQ-AEBS-004
// Example:  v=60, d=80
TEST_F(AebSystemSilTest, S10_AStationaryVehicleIsStillAvoided_Ex3) {
    // Given the ego vehicle drives at 60 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(60.0);
    // And a stationary vehicle stands 80 m ahead  [FN-S19]
    sil_.plant().set_lead(80.0, 0.0, 0.0);
    // When 15 s pass  [FN-S10]
    sil_.run(15.0);
    // Then no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the ego vehicle has stopped  [FN-S47]
    EXPECT_LT(sil_.plant().ego_speed_kph(), 0.5);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the actuator applied every command  [MON-S06]
    EXPECT_GT(sil_.actuator().metrics().applied_commands, 0U); EXPECT_EQ(sil_.actuator().metrics().rejected_commands, 0U);
}

// Scenario: The impact speed at 80 km/h stays at 40 km/h or less
// Source:   examples/aeb_system/features/availability.feature:22
// Verifies: REQ-AEBS-004
TEST_F(AebSystemSilTest, S11_TheImpactSpeedAt80KmHStaysAt40KmHOrLess) {
    // Given the ego vehicle drives at 80 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(80.0);
    // And a stationary vehicle stands 100 m ahead  [FN-S19]
    sil_.plant().set_lead(100.0, 0.0, 0.0);
    // When 15 s pass  [FN-S10]
    sil_.run(15.0);
    // Then the impact speed was at most 40 km/h  [FN-S46]
    EXPECT_LE(sil_.plant().metrics().impact_speed_kph, 40.0);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the actuator applied every command  [MON-S06]
    EXPECT_GT(sil_.actuator().metrics().applied_commands, 0U); EXPECT_EQ(sil_.actuator().metrics().rejected_commands, 0U);
}

// Scenario: A vehicle driving at 20 km/h is avoided from 80 km/h
// Source:   examples/aeb_system/features/availability.feature:31
// Verifies: REQ-AEBS-004
TEST_F(AebSystemSilTest, S12_AVehicleDrivingAt20KmHIsAvoidedFrom80KmH) {
    // Given the ego vehicle drives at 80 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(80.0);
    // And a lead vehicle drives 100 m ahead at 20 km/h  [FN-S11]
    sil_.plant().set_lead(100.0, 20.0, 0.0);
    // When 15 s pass  [FN-S10]
    sil_.run(15.0);
    // Then no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the actuator applied every command  [MON-S06]
    EXPECT_GT(sil_.actuator().metrics().applied_commands, 0U); EXPECT_EQ(sil_.actuator().metrics().rejected_commands, 0U);
}

// Scenario: A late cut-in is braked for at once
// Source:   examples/aeb_system/features/availability.feature:40
// Verifies: REQ-AEBS-004
TEST_F(AebSystemSilTest, S13_ALateCutInIsBrakedForAtOnce) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // When a vehicle cuts in 9 m ahead at 20 km/h  [FN-S14]
    sil_.plant().cut_in(9.0, 20.0);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the AEB state is FULL_BRAKE  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kFullBrake);
    // And the actuator applies -8.0 m/s2  [MON-S07]
    ASSERT_TRUE(sil_.actuator().applying()); EXPECT_NEAR(sil_.actuator().applied_mps2(), -8.0, 1e-6);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
}

// Scenario: Kickdown cancels braking without a monitor intervention
// Source:   examples/aeb_system/features/availability.feature:49
// Verifies: REQ-AEBS-004
TEST_F(AebSystemSilTest, S14_KickdownCancelsBrakingWithoutAMonitorIntervention) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 60 m ahead  [FN-S19]
    sil_.plant().set_lead(60.0, 0.0, 0.0);
    // When 4 s pass  [FN-S10]
    sil_.run(4.0);
    // And the driver presses the accelerator pedal to 90 %  [FN-S07]
    sil_.accelerator(90.0);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the AEB state is STANDBY  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kStandby);
    // And the monitor never entered SAFE_STATE  [MON-S05]
    EXPECT_LT(sil_.actuator().metrics().monitor_safe_state_s, 0.0);
    // And the actuator applied every command  [MON-S06]
    EXPECT_GT(sil_.actuator().metrics().applied_commands, 0U); EXPECT_EQ(sil_.actuator().metrics().rejected_commands, 0U);
}

}  // namespace
}  // namespace adas::aeb_system
