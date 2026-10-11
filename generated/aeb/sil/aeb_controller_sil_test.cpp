// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   aeb (pattern: can_function)
// Inputs:    sha256:19c3eada15aa0f9fc5976b576a1ef298ce3992a8bf775fb325a84334388d59ab
// Regenerate: python3 -m tools.gen

#include <gtest/gtest.h>

#include "sil/aeb_controller_sil.hpp"

// Closed-loop SIL tests generated from the Gherkin scenarios; each step expands from the step
// catalogues sil/steps/can_function.yaml and sil/steps/features/aeb.yaml.
// Test names are referenced by trace.json.

namespace adas::aeb {
namespace {

class AebControllerSilTest : public ::testing::Test {
protected:
    AebControllerSil sil_;
};

// Scenario: Approach to a stationary vehicle runs through the whole cascade
// Source:   examples/aeb/features/cascade.feature:7
// Verifies: REQ-AEB-001, REQ-AEB-002, REQ-AEB-003, REQ-AEB-005, REQ-AEB-014
TEST_F(AebControllerSilTest, S01_ApproachToAStationaryVehicleRunsThroughTheWholeCascade) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 60 m ahead  [FN-S19]
    sil_.plant().set_lead(60.0, 0.0, 0.0);
    // When 8 s pass  [FN-S10]
    sil_.run(8.0);
    // Then a forward collision warning was issued at a time to collision between 2.7 s and 2.9 s  [AEB-S02]
    ASSERT_TRUE(sil_.feature().metrics().warned); EXPECT_GE(sil_.feature().metrics().ttc_at_warning_s, 2.7); EXPECT_LE(sil_.feature().metrics().ttc_at_warning_s, 2.9);
    // And the warning preceded braking by at least 0.8 s  [AEB-S04]
    ASSERT_TRUE(sil_.feature().metrics().braked); EXPECT_GE(sil_.feature().metrics().first_brake_s - sil_.feature().metrics().first_warning_s, 0.8 - 1e-9);
    // And emergency braking started at a time to collision between 1.7 s and 1.9 s  [AEB-S05]
    ASSERT_TRUE(sil_.feature().metrics().braked); EXPECT_GE(sil_.feature().metrics().ttc_at_brake_s, 1.7); EXPECT_LE(sil_.feature().metrics().ttc_at_brake_s, 1.9);
    // And emergency braking started with 4 m/s2  [AEB-S13]
    ASSERT_TRUE(sil_.feature().metrics().braked); EXPECT_EQ(sil_.feature().metrics().first_brake_raw, ea::plant::Plant::accel_raw(4.0));
    // And full braking started at a time to collision between 1.1 s and 1.3 s  [AEB-S14]
    ASSERT_TRUE(sil_.feature().metrics().full_braked); EXPECT_GE(sil_.feature().metrics().ttc_at_full_brake_s, 1.1); EXPECT_LE(sil_.feature().metrics().ttc_at_full_brake_s, 1.3);
    // And the requested deceleration reached 8 m/s2  [AEB-S08]
    EXPECT_EQ(sil_.feature().metrics().max_brake_raw, ea::plant::Plant::accel_raw(8.0));
    // And no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the ego vehicle has stopped  [FN-S47]
    EXPECT_LT(sil_.plant().ego_speed_kph(), 0.5);
    // And the AEB state is STANDBY  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kStandby);
    // And every command frame passed the E2E check  [AEB-S10]
    EXPECT_GT(sil_.feature().metrics().command_frames, 0U); EXPECT_EQ(sil_.feature().metrics().command_e2e_errors, 0U);
    // And one command frame was sent per 20 ms  [AEB-S11]
    EXPECT_EQ(sil_.feature().metrics().command_frames, sil_.plant().metrics().ticks);
}

// Scenario: A late cut-in triggers full braking without the minimum warning time
// Source:   examples/aeb/features/cascade.feature:24
// Verifies: REQ-AEB-003
TEST_F(AebControllerSilTest, S02_ALateCutInTriggersFullBrakingWithoutTheMinimumWarningTime) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // When a vehicle cuts in 9 m ahead at 20 km/h  [FN-S14]
    sil_.plant().cut_in(9.0, 20.0);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the AEB state is FULL_BRAKE  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kFullBrake);
    // And a deceleration of 8 m/s2 is requested  [AEB-S07]
    EXPECT_EQ(sil_.feature().last_command().brake_raw, ea::plant::Plant::accel_raw(8.0));
    // And the warning is on  [AEB-S09]
    EXPECT_EQ(sil_.feature().last_command().warning, true);
}

// Scenario: A hard-braking lead vehicle still gets the minimum warning time
// Source:   examples/aeb/features/cascade.feature:33
// Verifies: REQ-AEB-002
TEST_F(AebControllerSilTest, S03_AHardBrakingLeadVehicleStillGetsTheMinimumWarningTime) {
    // Given the ego vehicle drives at 80 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(80.0);
    // And a lead vehicle drives 40 m ahead at 80 km/h  [FN-S11]
    sil_.plant().set_lead(40.0, 80.0, 0.0);
    // When the lead vehicle changes speed to 0 km/h at 6 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(0.0, 6.0);
    // And 10 s pass  [FN-S10]
    sil_.run(10.0);
    // Then the warning preceded braking by at least 0.8 s  [AEB-S04]
    ASSERT_TRUE(sil_.feature().metrics().braked); EXPECT_GE(sil_.feature().metrics().first_brake_s - sil_.feature().metrics().first_warning_s, 0.8 - 1e-9);
}

// Scenario: The warning ends when the time to collision rises above 3.3 s
// Source:   examples/aeb/features/cascade.feature:41
// Verifies: REQ-AEB-004
TEST_F(AebControllerSilTest, S04_TheWarningEndsWhenTheTimeToCollisionRisesAbove33S) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a lead vehicle drives 25 m ahead at 30 km/h  [FN-S11]
    sil_.plant().set_lead(25.0, 30.0, 0.0);
    // When 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the AEB state is WARNING  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kWarning);
    // When the lead vehicle changes speed to 80 km/h at 2 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(80.0, 2.0);
    // And 3 s pass  [FN-S10]
    sil_.run(3.0);
    // Then the warning ended at a time to collision between 3.3 s and 3.5 s  [AEB-S15]
    EXPECT_GE(sil_.feature().metrics().ttc_at_warning_end_s, 3.3); EXPECT_LE(sil_.feature().metrics().ttc_at_warning_end_s, 3.5);
    // And emergency braking was never requested  [AEB-S06]
    EXPECT_FALSE(sil_.feature().metrics().braked);
}

// Scenario: The warning ends when the lead vehicle pulls away
// Source:   examples/aeb/features/cascade.feature:52
// Verifies: REQ-AEB-004
TEST_F(AebControllerSilTest, S05_TheWarningEndsWhenTheLeadVehiclePullsAway) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a lead vehicle drives 25 m ahead at 30 km/h  [FN-S11]
    sil_.plant().set_lead(25.0, 30.0, 0.0);
    // When 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the AEB state is WARNING  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kWarning);
    // When the lead vehicle changes speed to 80 km/h at 4 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(80.0, 4.0);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then the AEB state is STANDBY  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kStandby);
    // And the warning is off  [AEB-S09]
    EXPECT_EQ(sil_.feature().last_command().warning, false);
    // And emergency braking was never requested  [AEB-S06]
    EXPECT_FALSE(sil_.feature().metrics().braked);
}

// Scenario: Braking ends once the ego vehicle no longer approaches
// Source:   examples/aeb/features/cascade.feature:64
// Verifies: REQ-AEB-005
TEST_F(AebControllerSilTest, S06_BrakingEndsOnceTheEgoVehicleNoLongerApproaches) {
    // Given the ego vehicle drives at 60 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(60.0);
    // And a lead vehicle drives 40 m ahead at 20 km/h  [FN-S11]
    sil_.plant().set_lead(40.0, 20.0, 0.0);
    // When 10 s pass  [FN-S10]
    sil_.run(10.0);
    // Then the AEB state is STANDBY  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kStandby);
    // And no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the ego vehicle is still moving  [FN-S48]
    EXPECT_GT(sil_.plant().ego_speed_kph(), 1.0);
}

// Scenario: Kickdown cancels the intervention, a lighter pedal does not #1
// Source:   examples/aeb/features/cascade.feature:73
// Verifies: REQ-AEB-006
// Example:  pedal=85, result=the AEB state is STANDBY
TEST_F(AebControllerSilTest, S07_KickdownCancelsTheInterventionALighterPedalDoesNot_Ex1) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 60 m ahead  [FN-S19]
    sil_.plant().set_lead(60.0, 0.0, 0.0);
    // When 3 s pass  [FN-S10]
    sil_.run(3.0);
    // Then the warning is on  [AEB-S09]
    EXPECT_EQ(sil_.feature().last_command().warning, true);
    // When the driver holds the accelerator pedal at 85 %  [FN-S20]
    sil_.plant().set_accel_pedal(85.0);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the AEB state is STANDBY  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kStandby);
}

// Scenario: Kickdown cancels the intervention, a lighter pedal does not #2
// Source:   examples/aeb/features/cascade.feature:73
// Verifies: REQ-AEB-006
// Example:  pedal=75, result=emergency braking is requested
TEST_F(AebControllerSilTest, S08_KickdownCancelsTheInterventionALighterPedalDoesNot_Ex2) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 60 m ahead  [FN-S19]
    sil_.plant().set_lead(60.0, 0.0, 0.0);
    // When 3 s pass  [FN-S10]
    sil_.run(3.0);
    // Then the warning is on  [AEB-S09]
    EXPECT_EQ(sil_.feature().last_command().warning, true);
    // When the driver holds the accelerator pedal at 75 %  [FN-S20]
    sil_.plant().set_accel_pedal(75.0);
    // And 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then emergency braking is requested  [AEB-S12]
    EXPECT_GT(sil_.feature().last_command().brake_raw, 0);
}

// Scenario: Kickdown also cancels full braking
// Source:   examples/aeb/features/cascade.feature:88
// Verifies: REQ-AEB-006
TEST_F(AebControllerSilTest, S09_KickdownAlsoCancelsFullBraking) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // When a vehicle cuts in 9 m ahead at 20 km/h  [FN-S14]
    sil_.plant().cut_in(9.0, 20.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the AEB state is FULL_BRAKE  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kFullBrake);
    // When the driver holds the accelerator pedal at 85 %  [FN-S20]
    sil_.plant().set_accel_pedal(85.0);
    // And 0.1 s pass  [FN-S10]
    sil_.run(0.1);
    // Then the AEB state is STANDBY  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kStandby);
}

// Scenario: The driver's brake pedal does not cancel emergency braking #1
// Source:   examples/aeb/features/cascade.feature:98
// Verifies: REQ-AEB-007
// Example:  t=3, stage=PARTIAL_BRAKE
TEST_F(AebControllerSilTest, S10_TheDriverSBrakePedalDoesNotCancelEmergencyBraking_Ex1) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 60 m ahead  [FN-S19]
    sil_.plant().set_lead(60.0, 0.0, 0.0);
    // When 3 s pass  [FN-S10]
    sil_.run(3.0);
    // Then the AEB state is PARTIAL_BRAKE  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kPartialBrake);
    // When the driver holds the brake pedal  [FN-S21]
    sil_.plant().set_brake_pedal(true);
    // And 0.04 s pass  [FN-S10]
    sil_.run(0.04);
    // Then the AEB state is PARTIAL_BRAKE  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kPartialBrake);
    // When 5 s pass  [FN-S10]
    sil_.run(5.0);
    // Then no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the requested deceleration reached 8 m/s2  [AEB-S08]
    EXPECT_EQ(sil_.feature().metrics().max_brake_raw, ea::plant::Plant::accel_raw(8.0));
    // And the ego vehicle has stopped  [FN-S47]
    EXPECT_LT(sil_.plant().ego_speed_kph(), 0.5);
}

// Scenario: The driver's brake pedal does not cancel emergency braking #2
// Source:   examples/aeb/features/cascade.feature:98
// Verifies: REQ-AEB-007
// Example:  t=3.6, stage=FULL_BRAKE
TEST_F(AebControllerSilTest, S11_TheDriverSBrakePedalDoesNotCancelEmergencyBraking_Ex2) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 60 m ahead  [FN-S19]
    sil_.plant().set_lead(60.0, 0.0, 0.0);
    // When 3.6 s pass  [FN-S10]
    sil_.run(3.6);
    // Then the AEB state is FULL_BRAKE  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kFullBrake);
    // When the driver holds the brake pedal  [FN-S21]
    sil_.plant().set_brake_pedal(true);
    // And 0.04 s pass  [FN-S10]
    sil_.run(0.04);
    // Then the AEB state is FULL_BRAKE  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kFullBrake);
    // When 5 s pass  [FN-S10]
    sil_.run(5.0);
    // Then no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the requested deceleration reached 8 m/s2  [AEB-S08]
    EXPECT_EQ(sil_.feature().metrics().max_brake_raw, ea::plant::Plant::accel_raw(8.0));
    // And the ego vehicle has stopped  [FN-S47]
    EXPECT_LT(sil_.plant().ego_speed_kph(), 0.5);
}

// Scenario: A stationary vehicle is avoided from 20 km/h to 60 km/h #1
// Source:   examples/aeb/features/performance.feature:4
// Verifies: REQ-AEB-010
// Example:  speed=20
TEST_F(AebControllerSilTest, S12_AStationaryVehicleIsAvoidedFrom20KmHTo60KmH_Ex1) {
    // Given the ego vehicle drives at 20 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(20.0);
    // And a stationary vehicle stands 100 m ahead  [FN-S19]
    sil_.plant().set_lead(100.0, 0.0, 0.0);
    // When 20 s pass  [FN-S10]
    sil_.run(20.0);
    // Then no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the ego vehicle has stopped  [FN-S47]
    EXPECT_LT(sil_.plant().ego_speed_kph(), 0.5);
}

// Scenario: A stationary vehicle is avoided from 20 km/h to 60 km/h #2
// Source:   examples/aeb/features/performance.feature:4
// Verifies: REQ-AEB-010
// Example:  speed=40
TEST_F(AebControllerSilTest, S13_AStationaryVehicleIsAvoidedFrom20KmHTo60KmH_Ex2) {
    // Given the ego vehicle drives at 40 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(40.0);
    // And a stationary vehicle stands 100 m ahead  [FN-S19]
    sil_.plant().set_lead(100.0, 0.0, 0.0);
    // When 20 s pass  [FN-S10]
    sil_.run(20.0);
    // Then no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the ego vehicle has stopped  [FN-S47]
    EXPECT_LT(sil_.plant().ego_speed_kph(), 0.5);
}

// Scenario: A stationary vehicle is avoided from 20 km/h to 60 km/h #3
// Source:   examples/aeb/features/performance.feature:4
// Verifies: REQ-AEB-010
// Example:  speed=60
TEST_F(AebControllerSilTest, S14_AStationaryVehicleIsAvoidedFrom20KmHTo60KmH_Ex3) {
    // Given the ego vehicle drives at 60 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(60.0);
    // And a stationary vehicle stands 100 m ahead  [FN-S19]
    sil_.plant().set_lead(100.0, 0.0, 0.0);
    // When 20 s pass  [FN-S10]
    sil_.run(20.0);
    // Then no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the ego vehicle has stopped  [FN-S47]
    EXPECT_LT(sil_.plant().ego_speed_kph(), 0.5);
}

// Scenario: The impact speed on a stationary vehicle at 80 km/h is at most 40 km/h
// Source:   examples/aeb/features/performance.feature:18
// Verifies: REQ-AEB-011
TEST_F(AebControllerSilTest, S15_TheImpactSpeedOnAStationaryVehicleAt80KmHIsAtMost40KmH) {
    // Given the ego vehicle drives at 80 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(80.0);
    // And a stationary vehicle stands 100 m ahead  [FN-S19]
    sil_.plant().set_lead(100.0, 0.0, 0.0);
    // When 10 s pass  [FN-S10]
    sil_.run(10.0);
    // Then the impact speed was at most 40 km/h  [FN-S46]
    EXPECT_LE(sil_.plant().metrics().impact_speed_kph, 40.0);
}

// Scenario: A vehicle at 20 km/h is avoided from up to 80 km/h #1
// Source:   examples/aeb/features/performance.feature:25
// Verifies: REQ-AEB-012
// Example:  speed=50
TEST_F(AebControllerSilTest, S16_AVehicleAt20KmHIsAvoidedFromUpTo80KmH_Ex1) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a lead vehicle drives 100 m ahead at 20 km/h  [FN-S11]
    sil_.plant().set_lead(100.0, 20.0, 0.0);
    // When 20 s pass  [FN-S10]
    sil_.run(20.0);
    // Then no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
}

// Scenario: A vehicle at 20 km/h is avoided from up to 80 km/h #2
// Source:   examples/aeb/features/performance.feature:25
// Verifies: REQ-AEB-012
// Example:  speed=80
TEST_F(AebControllerSilTest, S17_AVehicleAt20KmHIsAvoidedFromUpTo80KmH_Ex2) {
    // Given the ego vehicle drives at 80 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(80.0);
    // And a lead vehicle drives 100 m ahead at 20 km/h  [FN-S11]
    sil_.plant().set_lead(100.0, 20.0, 0.0);
    // When 20 s pass  [FN-S10]
    sil_.run(20.0);
    // Then no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
}

// Scenario: Only vehicles within 1.8 m of the lane centre are relevant #1
// Source:   examples/aeb/features/robustness.feature:4
// Verifies: REQ-AEB-008
// Example:  offset=2.0, result=no forward collision warning was issued
TEST_F(AebControllerSilTest, S18_OnlyVehiclesWithin18MOfTheLaneCentreAreRelevant_Ex1) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a vehicle drives 60 m ahead at 0 km/h with 2.0 m lateral offset  [FN-S12]
    sil_.plant().set_lead(60.0, 0.0, 2.0);
    // When 4 s pass  [FN-S10]
    sil_.run(4.0);
    // Then no forward collision warning was issued  [AEB-S03]
    EXPECT_FALSE(sil_.feature().metrics().warned);
}

// Scenario: Only vehicles within 1.8 m of the lane centre are relevant #2
// Source:   examples/aeb/features/robustness.feature:4
// Verifies: REQ-AEB-008
// Example:  offset=1.6, result=a forward collision warning was issued at a time to collision between 2.7 s and 2.9 s
TEST_F(AebControllerSilTest, S19_OnlyVehiclesWithin18MOfTheLaneCentreAreRelevant_Ex2) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a vehicle drives 60 m ahead at 0 km/h with 1.6 m lateral offset  [FN-S12]
    sil_.plant().set_lead(60.0, 0.0, 1.6);
    // When 4 s pass  [FN-S10]
    sil_.run(4.0);
    // Then a forward collision warning was issued at a time to collision between 2.7 s and 2.9 s  [AEB-S02]
    ASSERT_TRUE(sil_.feature().metrics().warned); EXPECT_GE(sil_.feature().metrics().ttc_at_warning_s, 2.7); EXPECT_LE(sil_.feature().metrics().ttc_at_warning_s, 2.9);
}

// Scenario: A vehicle followed at the same speed is not a hazard
// Source:   examples/aeb/features/robustness.feature:16
// Verifies: REQ-AEB-008
TEST_F(AebControllerSilTest, S20_AVehicleFollowedAtTheSameSpeedIsNotAHazard) {
    // Given the ego vehicle drives at 80 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(80.0);
    // And a lead vehicle drives 10 m ahead at 80 km/h  [FN-S11]
    sil_.plant().set_lead(10.0, 80.0, 0.0);
    // When 10 s pass  [FN-S10]
    sil_.run(10.0);
    // Then no forward collision warning was issued  [AEB-S03]
    EXPECT_FALSE(sil_.feature().metrics().warned);
    // And emergency braking was never requested  [AEB-S06]
    EXPECT_FALSE(sil_.feature().metrics().braked);
}

// Scenario: Warnings start only between 10 km/h and 150 km/h #1
// Source:   examples/aeb/features/robustness.feature:24
// Verifies: REQ-AEB-009
// Example:  speed=9, distance=20, time=5, result=no forward collision warning was issued
TEST_F(AebControllerSilTest, S21_WarningsStartOnlyBetween10KmHAnd150KmH_Ex1) {
    // Given the ego vehicle drives at 9 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(9.0);
    // And a stationary vehicle stands 20 m ahead  [FN-S19]
    sil_.plant().set_lead(20.0, 0.0, 0.0);
    // When 5 s pass  [FN-S10]
    sil_.run(5.0);
    // Then no forward collision warning was issued  [AEB-S03]
    EXPECT_FALSE(sil_.feature().metrics().warned);
}

// Scenario: Warnings start only between 10 km/h and 150 km/h #2
// Source:   examples/aeb/features/robustness.feature:24
// Verifies: REQ-AEB-009
// Example:  speed=11, distance=20, time=5, result=a forward collision warning was issued at a time to collision between 2.7 s and 2.9 s
TEST_F(AebControllerSilTest, S22_WarningsStartOnlyBetween10KmHAnd150KmH_Ex2) {
    // Given the ego vehicle drives at 11 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(11.0);
    // And a stationary vehicle stands 20 m ahead  [FN-S19]
    sil_.plant().set_lead(20.0, 0.0, 0.0);
    // When 5 s pass  [FN-S10]
    sil_.run(5.0);
    // Then a forward collision warning was issued at a time to collision between 2.7 s and 2.9 s  [AEB-S02]
    ASSERT_TRUE(sil_.feature().metrics().warned); EXPECT_GE(sil_.feature().metrics().ttc_at_warning_s, 2.7); EXPECT_LE(sil_.feature().metrics().ttc_at_warning_s, 2.9);
}

// Scenario: Warnings start only between 10 km/h and 150 km/h #3
// Source:   examples/aeb/features/robustness.feature:24
// Verifies: REQ-AEB-009
// Example:  speed=145, distance=140, time=2, result=a forward collision warning was issued at a time to collision between 2.7 s and 2.9 s
TEST_F(AebControllerSilTest, S23_WarningsStartOnlyBetween10KmHAnd150KmH_Ex3) {
    // Given the ego vehicle drives at 145 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(145.0);
    // And a stationary vehicle stands 140 m ahead  [FN-S19]
    sil_.plant().set_lead(140.0, 0.0, 0.0);
    // When 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then a forward collision warning was issued at a time to collision between 2.7 s and 2.9 s  [AEB-S02]
    ASSERT_TRUE(sil_.feature().metrics().warned); EXPECT_GE(sil_.feature().metrics().ttc_at_warning_s, 2.7); EXPECT_LE(sil_.feature().metrics().ttc_at_warning_s, 2.9);
}

// Scenario: Warnings start only between 10 km/h and 150 km/h #4
// Source:   examples/aeb/features/robustness.feature:24
// Verifies: REQ-AEB-009
// Example:  speed=155, distance=140, time=2, result=no forward collision warning was issued
TEST_F(AebControllerSilTest, S24_WarningsStartOnlyBetween10KmHAnd150KmH_Ex4) {
    // Given the ego vehicle drives at 155 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(155.0);
    // And a stationary vehicle stands 140 m ahead  [FN-S19]
    sil_.plant().set_lead(140.0, 0.0, 0.0);
    // When 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then no forward collision warning was issued  [AEB-S03]
    EXPECT_FALSE(sil_.feature().metrics().warned);
}

// Scenario: A paused radar leads to FAULT without intervention, then recovers
// Source:   examples/aeb/features/robustness.feature:38
// Verifies: REQ-AEB-013
TEST_F(AebControllerSilTest, S25_APausedRadarLeadsToFAULTWithoutInterventionThenRecovers) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // And a stationary vehicle stands 100 m ahead  [FN-S19]
    sil_.plant().set_lead(100.0, 0.0, 0.0);
    // When 1 s passes  [FN-S10]
    sil_.run(1.0);
    // And the radar sender pauses for 5 frames  [FN-S18]
    sil_.plant().skip_frames(ea::plant::Stream::kRadar, 5U);
    // And 0.16 s pass  [FN-S10]
    sil_.run(0.16);
    // Then the AEB state is FAULT  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kFault);
    // And the warning is off  [AEB-S09]
    EXPECT_EQ(sil_.feature().last_command().warning, false);
    // And emergency braking was never requested  [AEB-S06]
    EXPECT_FALSE(sil_.feature().metrics().braked);
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then the AEB state is STANDBY  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kStandby);
}

// Scenario: Vehicle input timeouts lead to FAULT #1
// Source:   examples/aeb/features/robustness.feature:51
// Verifies: REQ-AEB-013
// Example:  message=ego state, n=2, state=STANDBY
TEST_F(AebControllerSilTest, S26_VehicleInputTimeoutsLeadToFAULT_Ex1) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // When 1 s passes  [FN-S10]
    sil_.run(1.0);
    // And the ego state sender pauses for 2 frames  [FN-S18]
    sil_.plant().skip_frames(ea::plant::Stream::kEgoState, 2U);
    // And 0.06 s pass  [FN-S10]
    sil_.run(0.06);
    // Then the AEB state is STANDBY  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kStandby);
}

// Scenario: Vehicle input timeouts lead to FAULT #2
// Source:   examples/aeb/features/robustness.feature:51
// Verifies: REQ-AEB-013
// Example:  message=ego state, n=3, state=FAULT
TEST_F(AebControllerSilTest, S27_VehicleInputTimeoutsLeadToFAULT_Ex2) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // When 1 s passes  [FN-S10]
    sil_.run(1.0);
    // And the ego state sender pauses for 3 frames  [FN-S18]
    sil_.plant().skip_frames(ea::plant::Stream::kEgoState, 3U);
    // And 0.06 s pass  [FN-S10]
    sil_.run(0.06);
    // Then the AEB state is FAULT  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kFault);
}

// Scenario: Vehicle input timeouts lead to FAULT #3
// Source:   examples/aeb/features/robustness.feature:51
// Verifies: REQ-AEB-013
// Example:  message=driver input, n=2, state=STANDBY
TEST_F(AebControllerSilTest, S28_VehicleInputTimeoutsLeadToFAULT_Ex3) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // When 1 s passes  [FN-S10]
    sil_.run(1.0);
    // And the driver input sender pauses for 2 frames  [FN-S18]
    sil_.plant().skip_frames(ea::plant::Stream::kDriverInput, 2U);
    // And 0.06 s pass  [FN-S10]
    sil_.run(0.06);
    // Then the AEB state is STANDBY  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kStandby);
}

// Scenario: Vehicle input timeouts lead to FAULT #4
// Source:   examples/aeb/features/robustness.feature:51
// Verifies: REQ-AEB-013
// Example:  message=driver input, n=3, state=FAULT
TEST_F(AebControllerSilTest, S29_VehicleInputTimeoutsLeadToFAULT_Ex4) {
    // Given the ego vehicle drives at 50 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(50.0);
    // When 1 s passes  [FN-S10]
    sil_.run(1.0);
    // And the driver input sender pauses for 3 frames  [FN-S18]
    sil_.plant().skip_frames(ea::plant::Stream::kDriverInput, 3U);
    // And 0.06 s pass  [FN-S10]
    sil_.run(0.06);
    // Then the AEB state is FAULT  [AEB-S01]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AebStatus::kFault);
}

}  // namespace
}  // namespace adas::aeb
