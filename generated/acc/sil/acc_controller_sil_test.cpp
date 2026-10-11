// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   acc (pattern: can_function)
// Inputs:    sha256:df14c530c502f140150ce4bf860cdd1e8018cd3a627d306d9073740480ad2f3b
// Regenerate: python3 -m tools.gen

#include <gtest/gtest.h>

#include "sil/acc_controller_sil.hpp"

// Closed-loop SIL tests generated from the Gherkin scenarios; each step expands from the step
// catalogues sil/steps/can_function.yaml and sil/steps/features/acc.yaml.
// Test names are referenced by trace.json.

namespace adas::acc {
namespace {

class AccControllerSilTest : public ::testing::Test {
protected:
    AccControllerSil sil_;
};

// Scenario: Main switch on enters STANDBY without requesting acceleration or braking
// Source:   examples/acc/features/modes.feature:6
// Verifies: REQ-ACC-001, REQ-ACC-019
TEST_F(AccControllerSilTest, S01_MainSwitchOnEntersSTANDBYWithoutRequestingAccelerationOrBrak) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // When the driver switches ACC on  [FN-S02]
    sil_.switch_main(true);
    // Then the ACC state is STANDBY  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kStandby);
    // And neither acceleration nor braking is requested  [ACC-S21]
    EXPECT_EQ(sil_.feature().last_command().accel_raw, 0); EXPECT_EQ(sil_.feature().last_command().brake_raw, 0);
}

// Scenario: Set engages at the current speed rounded to the nearest km/h
// Source:   examples/acc/features/modes.feature:13
// Verifies: REQ-ACC-002, REQ-ACC-020
TEST_F(AccControllerSilTest, S02_SetEngagesAtTheCurrentSpeedRoundedToTheNearestKmH) {
    // Given the ego vehicle drives at 87.4 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(87.4);
    // And the driver switches ACC on  [FN-S02]
    sil_.switch_main(true);
    // When the driver presses the set button  [FN-S04]
    sil_.press(ea::plant::Button::kSet, 1);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
    // And the set speed is 87 km/h  [ACC-S22]
    EXPECT_DOUBLE_EQ(sil_.feature().last_command().set_speed_kph, 87.0);
    // And the reported time gap setting is 2  [ACC-S23]
    EXPECT_EQ(sil_.feature().last_command().time_gap_setting, 2U);
}

// Scenario: Set is ignored while the brake pedal is pressed
// Source:   examples/acc/features/modes.feature:22
// Verifies: REQ-ACC-002
TEST_F(AccControllerSilTest, S03_SetIsIgnoredWhileTheBrakePedalIsPressed) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the driver switches ACC on  [FN-S02]
    sil_.switch_main(true);
    // And the driver presses the brake pedal  [FN-S06]
    sil_.brake(true);
    // When the driver presses the set button  [FN-S04]
    sil_.press(ea::plant::Button::kSet, 1);
    // Then the ACC state is STANDBY  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kStandby);
}

// Scenario: Releasing the brake while Set is still held does not engage
// Source:   examples/acc/features/modes.feature:30
// Verifies: REQ-ACC-002
TEST_F(AccControllerSilTest, S04_ReleasingTheBrakeWhileSetIsStillHeldDoesNotEngage) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And the driver switches ACC on  [FN-S02]
    sil_.switch_main(true);
    // And the driver presses the brake pedal  [FN-S06]
    sil_.brake(true);
    // When the driver releases the brake pedal while holding the set button  [ACC-S19]
    sil_.release_brake_while_holding_set();
    // Then the ACC state is STANDBY  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kStandby);
}

// Scenario: Engagement is refused below 30 km/h #1
// Source:   examples/acc/features/modes.feature:38
// Verifies: REQ-ACC-003
// Example:  button=set
TEST_F(AccControllerSilTest, S05_EngagementIsRefusedBelow30KmH_Ex1) {
    // Given the ego vehicle drives at 60 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(60.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // And the driver presses the cancel button  [FN-S04]
    sil_.press(ea::plant::Button::kCancel, 1);
    // And the ego vehicle drives at 29 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(29.0);
    // When the driver presses the set button  [FN-S04]
    sil_.press(ea::plant::Button::kSet, 1);
    // Then the ACC state is STANDBY  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kStandby);
}

// Scenario: Engagement is refused below 30 km/h #2
// Source:   examples/acc/features/modes.feature:38
// Verifies: REQ-ACC-003
// Example:  button=resume
TEST_F(AccControllerSilTest, S06_EngagementIsRefusedBelow30KmH_Ex2) {
    // Given the ego vehicle drives at 60 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(60.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // And the driver presses the cancel button  [FN-S04]
    sil_.press(ea::plant::Button::kCancel, 1);
    // And the ego vehicle drives at 29 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(29.0);
    // When the driver presses the resume button  [FN-S04]
    sil_.press(ea::plant::Button::kResume, 1);
    // Then the ACC state is STANDBY  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kStandby);
}

// Scenario: Cancel keeps the set speed and Resume restores it
// Source:   examples/acc/features/modes.feature:52
// Verifies: REQ-ACC-004, REQ-ACC-006
TEST_F(AccControllerSilTest, S07_CancelKeepsTheSetSpeedAndResumeRestoresIt) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver presses the cancel button  [FN-S04]
    sil_.press(ea::plant::Button::kCancel, 1);
    // Then the ACC state is STANDBY  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kStandby);
    // And the set speed is 100 km/h  [ACC-S22]
    EXPECT_DOUBLE_EQ(sil_.feature().last_command().set_speed_kph, 100.0);
    // When the ego vehicle drives at 80 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(80.0);
    // And the driver presses the resume button  [FN-S04]
    sil_.press(ea::plant::Button::kResume, 1);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
    // And the set speed is 100 km/h  [ACC-S22]
    EXPECT_DOUBLE_EQ(sil_.feature().last_command().set_speed_kph, 100.0);
}

// Scenario: Speed Up and Speed Down change the set speed in 5 km/h steps
// Source:   examples/acc/features/modes.feature:64
// Verifies: REQ-ACC-005
TEST_F(AccControllerSilTest, S08_SpeedUpAndSpeedDownChangeTheSetSpeedIn5KmHSteps) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver presses the speed up button  [FN-S04]
    sil_.press(ea::plant::Button::kSpeedUp, 1);
    // Then the set speed is 105 km/h  [ACC-S22]
    EXPECT_DOUBLE_EQ(sil_.feature().last_command().set_speed_kph, 105.0);
    // When the driver presses the speed down button  [FN-S04]
    sil_.press(ea::plant::Button::kSpeedDown, 1);
    // And the driver presses the speed down button  [FN-S04]
    sil_.press(ea::plant::Button::kSpeedDown, 1);
    // Then the set speed is 95 km/h  [ACC-S22]
    EXPECT_DOUBLE_EQ(sil_.feature().last_command().set_speed_kph, 95.0);
}

// Scenario: The set speed stays within 30 km/h to 180 km/h #1
// Source:   examples/acc/features/modes.feature:74
// Verifies: REQ-ACC-005
// Example:  speed=178, button=speed up, result=180
TEST_F(AccControllerSilTest, S09_TheSetSpeedStaysWithin30KmHTo180KmH_Ex1) {
    // Given the ego vehicle drives at 178 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(178.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver presses the speed up button  [FN-S04]
    sil_.press(ea::plant::Button::kSpeedUp, 1);
    // Then the set speed is 180 km/h  [ACC-S22]
    EXPECT_DOUBLE_EQ(sil_.feature().last_command().set_speed_kph, 180.0);
}

// Scenario: The set speed stays within 30 km/h to 180 km/h #2
// Source:   examples/acc/features/modes.feature:74
// Verifies: REQ-ACC-005
// Example:  speed=32, button=speed down, result=30
TEST_F(AccControllerSilTest, S10_TheSetSpeedStaysWithin30KmHTo180KmH_Ex2) {
    // Given the ego vehicle drives at 32 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(32.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver presses the speed down button  [FN-S04]
    sil_.press(ea::plant::Button::kSpeedDown, 1);
    // Then the set speed is 30 km/h  [ACC-S22]
    EXPECT_DOUBLE_EQ(sil_.feature().last_command().set_speed_kph, 30.0);
}

// Scenario: The brake pedal disengages within one cycle
// Source:   examples/acc/features/modes.feature:86
// Verifies: REQ-ACC-006, REQ-ACC-019
TEST_F(AccControllerSilTest, S11_TheBrakePedalDisengagesWithinOneCycle) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver presses the brake pedal  [FN-S06]
    sil_.brake(true);
    // Then the ACC state is STANDBY  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kStandby);
    // And the ACC state changed within 20 ms of the driver action  [FN-S24]
    EXPECT_GE(sil_.plant().status_change_latency_ms(), 0.0); EXPECT_LE(sil_.plant().status_change_latency_ms(), 20.0);
    // And the set speed is 100 km/h  [ACC-S22]
    EXPECT_DOUBLE_EQ(sil_.feature().last_command().set_speed_kph, 100.0);
    // And neither acceleration nor braking is requested  [ACC-S21]
    EXPECT_EQ(sil_.feature().last_command().accel_raw, 0); EXPECT_EQ(sil_.feature().last_command().brake_raw, 0);
}

// Scenario: Accelerator pedal override suspends control
// Source:   examples/acc/features/modes.feature:96
// Verifies: REQ-ACC-007
TEST_F(AccControllerSilTest, S12_AcceleratorPedalOverrideSuspendsControl) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver presses the accelerator pedal to 30 %  [FN-S07]
    sil_.accelerator(30.0);
    // Then the ACC state is OVERRIDE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kOverride);
    // And neither acceleration nor braking is requested  [ACC-S21]
    EXPECT_EQ(sil_.feature().last_command().accel_raw, 0); EXPECT_EQ(sil_.feature().last_command().brake_raw, 0);
    // When 2 s pass  [FN-S10]
    sil_.run(2.0);
    // And the driver releases the accelerator pedal  [FN-S08]
    sil_.accelerator(0.0);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
}

// Scenario: The brake pedal also ends OVERRIDE
// Source:   examples/acc/features/modes.feature:107
// Verifies: REQ-ACC-006
TEST_F(AccControllerSilTest, S13_TheBrakePedalAlsoEndsOVERRIDE) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver presses the accelerator pedal to 30 %  [FN-S07]
    sil_.accelerator(30.0);
    // Then the ACC state is OVERRIDE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kOverride);
    // When the driver presses the brake pedal  [FN-S06]
    sil_.brake(true);
    // Then the ACC state is STANDBY  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kStandby);
}

// Scenario: A light touch of the accelerator pedal does not override
// Source:   examples/acc/features/modes.feature:116
// Verifies: REQ-ACC-007
TEST_F(AccControllerSilTest, S14_ALightTouchOfTheAcceleratorPedalDoesNotOverride) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver presses the accelerator pedal to 4 %  [FN-S07]
    sil_.accelerator(4.0);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
}

// Scenario: Main switch off clears the set speed
// Source:   examples/acc/features/modes.feature:123
// Verifies: REQ-ACC-008, REQ-ACC-019
TEST_F(AccControllerSilTest, S15_MainSwitchOffClearsTheSetSpeed) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver switches ACC off  [FN-S02]
    sil_.switch_main(false);
    // Then the ACC state is OFF  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kOff);
    // And the set speed is 0 km/h  [ACC-S22]
    EXPECT_DOUBLE_EQ(sil_.feature().last_command().set_speed_kph, 0.0);
    // And neither acceleration nor braking is requested  [ACC-S21]
    EXPECT_EQ(sil_.feature().last_command().accel_raw, 0); EXPECT_EQ(sil_.feature().last_command().brake_raw, 0);
    // When the driver switches ACC on  [FN-S02]
    sil_.switch_main(true);
    // And the driver presses the resume button  [FN-S04]
    sil_.press(ea::plant::Button::kResume, 1);
    // Then the ACC state is STANDBY  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kStandby);
}

// Scenario: ACC hands over below 25 km/h and only then #1
// Source:   examples/acc/features/modes.feature:135
// Verifies: REQ-ACC-009
// Example:  lead=22, state=STANDBY
TEST_F(AccControllerSilTest, S16_ACCHandsOverBelow25KmHAndOnlyThen_Ex1) {
    // Given the ego vehicle drives at 60 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(60.0);
    // And a lead vehicle drives 30 m ahead at 60 km/h  [FN-S11]
    sil_.plant().set_lead(30.0, 60.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the lead vehicle changes speed to 22 km/h at 2 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(22.0, 2.0);
    // And 40 s pass  [FN-S10]
    sil_.run(40.0);
    // Then the ACC state is STANDBY  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kStandby);
}

// Scenario: ACC hands over below 25 km/h and only then #2
// Source:   examples/acc/features/modes.feature:135
// Verifies: REQ-ACC-009
// Example:  lead=27, state=ACTIVE
TEST_F(AccControllerSilTest, S17_ACCHandsOverBelow25KmHAndOnlyThen_Ex2) {
    // Given the ego vehicle drives at 60 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(60.0);
    // And a lead vehicle drives 30 m ahead at 60 km/h  [FN-S11]
    sil_.plant().set_lead(30.0, 60.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the lead vehicle changes speed to 27 km/h at 2 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(27.0, 2.0);
    // And 40 s pass  [FN-S10]
    sil_.run(40.0);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
}

// Scenario: Accelerate to a higher set speed within the comfort limits
// Source:   examples/acc/features/control.feature:4
// Verifies: REQ-ACC-010, REQ-ACC-014, REQ-ACC-015, REQ-ACC-017, REQ-ACC-018
TEST_F(AccControllerSilTest, S18_AccelerateToAHigherSetSpeedWithinTheComfortLimits) {
    // Given the ego vehicle drives at 80 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(80.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver presses the speed up button 8 times  [FN-S05]
    sil_.press(ea::plant::Button::kSpeedUp, 8);
    // And 40 s pass  [FN-S10]
    sil_.run(40.0);
    // Then the set speed is 120 km/h  [ACC-S22]
    EXPECT_DOUBLE_EQ(sil_.feature().last_command().set_speed_kph, 120.0);
    // And the ego speed is 120 km/h within 1 km/h  [FN-S40]
    EXPECT_NEAR(sil_.plant().ego_speed_kph(), 120.0, 1.0);
    // And the requested acceleration never exceeded 2.0 m/s2  [ACC-S27]
    EXPECT_LE(sil_.feature().metrics().max_accel_raw, ea::plant::Plant::accel_raw(2.0));
    // And the requested jerk never exceeded 2.5 m/s3 increasing and 5.0 m/s3 decreasing  [ACC-S29]
    EXPECT_LE(sil_.feature().metrics().max_jerk_up_raw, ea::plant::Plant::jerk_step_raw(2.5)); EXPECT_LE(sil_.feature().metrics().max_jerk_down_raw, ea::plant::Plant::jerk_step_raw(5.0));
    // And acceleration and braking were never requested together  [ACC-S30]
    EXPECT_EQ(sil_.feature().metrics().both_requested, 0U);
    // And the steering request was always 0 deg  [ACC-S31]
    EXPECT_EQ(sil_.feature().metrics().steering_nonzero, 0U);
}

// Scenario: Follow a slower vehicle at the selected time gap #1
// Source:   examples/acc/features/control.feature:17
// Verifies: REQ-ACC-011, REQ-ACC-012
// Example:  setting=1, distance=32.8
TEST_F(AccControllerSilTest, S19_FollowASlowerVehicleAtTheSelectedTimeGap_Ex1) {
    // Given the ego vehicle drives at 120 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(120.0);
    // And the time gap setting is 1  [FN-S09]
    sil_.plant().set_gap_setting(1U);
    // And a lead vehicle drives 80 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(80.0, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 60 s pass  [FN-S10]
    sil_.run(60.0);
    // Then the distance to the lead vehicle is 32.8 m within 2 m  [FN-S43]
    ASSERT_TRUE(sil_.plant().lead_present()); EXPECT_NEAR(sil_.plant().distance_m(), 32.8, 2.0);
    // And the ego speed is 100 km/h within 1 km/h  [FN-S40]
    EXPECT_NEAR(sil_.plant().ego_speed_kph(), 100.0, 1.0);
    // And the reported time gap setting is 1  [ACC-S23]
    EXPECT_EQ(sil_.feature().last_command().time_gap_setting, 1U);
    // And the ego speed never exceeded 120 km/h  [FN-S42]
    EXPECT_LE(sil_.plant().metrics().max_ego_speed_kph, 120.0 + 1e-6);  // m/s <-> km/h round-off
}

// Scenario: Follow a slower vehicle at the selected time gap #2
// Source:   examples/acc/features/control.feature:17
// Verifies: REQ-ACC-011, REQ-ACC-012
// Example:  setting=2, distance=46.7
TEST_F(AccControllerSilTest, S20_FollowASlowerVehicleAtTheSelectedTimeGap_Ex2) {
    // Given the ego vehicle drives at 120 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(120.0);
    // And the time gap setting is 2  [FN-S09]
    sil_.plant().set_gap_setting(2U);
    // And a lead vehicle drives 80 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(80.0, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 60 s pass  [FN-S10]
    sil_.run(60.0);
    // Then the distance to the lead vehicle is 46.7 m within 2 m  [FN-S43]
    ASSERT_TRUE(sil_.plant().lead_present()); EXPECT_NEAR(sil_.plant().distance_m(), 46.7, 2.0);
    // And the ego speed is 100 km/h within 1 km/h  [FN-S40]
    EXPECT_NEAR(sil_.plant().ego_speed_kph(), 100.0, 1.0);
    // And the reported time gap setting is 2  [ACC-S23]
    EXPECT_EQ(sil_.feature().last_command().time_gap_setting, 2U);
    // And the ego speed never exceeded 120 km/h  [FN-S42]
    EXPECT_LE(sil_.plant().metrics().max_ego_speed_kph, 120.0 + 1e-6);  // m/s <-> km/h round-off
}

// Scenario: Follow a slower vehicle at the selected time gap #3
// Source:   examples/acc/features/control.feature:17
// Verifies: REQ-ACC-011, REQ-ACC-012
// Example:  setting=3, distance=60.6
TEST_F(AccControllerSilTest, S21_FollowASlowerVehicleAtTheSelectedTimeGap_Ex3) {
    // Given the ego vehicle drives at 120 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(120.0);
    // And the time gap setting is 3  [FN-S09]
    sil_.plant().set_gap_setting(3U);
    // And a lead vehicle drives 80 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(80.0, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 60 s pass  [FN-S10]
    sil_.run(60.0);
    // Then the distance to the lead vehicle is 60.6 m within 2 m  [FN-S43]
    ASSERT_TRUE(sil_.plant().lead_present()); EXPECT_NEAR(sil_.plant().distance_m(), 60.6, 2.0);
    // And the ego speed is 100 km/h within 1 km/h  [FN-S40]
    EXPECT_NEAR(sil_.plant().ego_speed_kph(), 100.0, 1.0);
    // And the reported time gap setting is 3  [ACC-S23]
    EXPECT_EQ(sil_.feature().last_command().time_gap_setting, 3U);
    // And the ego speed never exceeded 120 km/h  [FN-S42]
    EXPECT_LE(sil_.plant().metrics().max_ego_speed_kph, 120.0 + 1e-6);  // m/s <-> km/h round-off
}

// Scenario: A faster lead vehicle does not pull the ego vehicle above the set speed
// Source:   examples/acc/features/control.feature:35
// Verifies: REQ-ACC-011, REQ-ACC-010
TEST_F(AccControllerSilTest, S22_AFasterLeadVehicleDoesNotPullTheEgoVehicleAboveTheSetSpeed) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And a lead vehicle drives 50 m ahead at 130 km/h  [FN-S11]
    sil_.plant().set_lead(50.0, 130.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 30 s pass  [FN-S10]
    sil_.run(30.0);
    // Then the ego speed is 100 km/h within 1 km/h  [FN-S40]
    EXPECT_NEAR(sil_.plant().ego_speed_kph(), 100.0, 1.0);
    // And the ego speed never exceeded 101 km/h  [FN-S42]
    EXPECT_LE(sil_.plant().metrics().max_ego_speed_kph, 101.0 + 1e-6);  // m/s <-> km/h round-off
}

// Scenario: A vehicle in the adjacent lane is ignored
// Source:   examples/acc/features/control.feature:44
// Verifies: REQ-ACC-013
TEST_F(AccControllerSilTest, S23_AVehicleInTheAdjacentLaneIsIgnored) {
    // Given the ego vehicle drives at 120 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(120.0);
    // And a vehicle drives 50 m ahead at 80 km/h with 1.9 m lateral offset  [FN-S12]
    sil_.plant().set_lead(50.0, 80.0, 1.9);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 20 s pass  [FN-S10]
    sil_.run(20.0);
    // Then the ego speed is 120 km/h within 1 km/h  [FN-S40]
    EXPECT_NEAR(sil_.plant().ego_speed_kph(), 120.0, 1.0);
    // And braking was never requested  [ACC-S26]
    EXPECT_EQ(sil_.feature().metrics().max_brake_raw, 0);
}

// Scenario: A vehicle just inside the lane boundary is followed
// Source:   examples/acc/features/control.feature:53
// Verifies: REQ-ACC-013
TEST_F(AccControllerSilTest, S24_AVehicleJustInsideTheLaneBoundaryIsFollowed) {
    // Given the ego vehicle drives at 120 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(120.0);
    // And a vehicle drives 60 m ahead at 80 km/h with 1.7 m lateral offset  [FN-S12]
    sil_.plant().set_lead(60.0, 80.0, 1.7);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 2 s pass  [FN-S10]
    sil_.run(2.0);
    // Then braking is requested  [ACC-S25]
    EXPECT_GT(sil_.feature().last_command().brake_raw, 0);
}

// Scenario: A vehicle beyond 200 m is not a target even when closing fast
// Source:   examples/acc/features/control.feature:61
// Verifies: REQ-ACC-013
TEST_F(AccControllerSilTest, S25_AVehicleBeyond200MIsNotATargetEvenWhenClosingFast) {
    // Given the ego vehicle drives at 180 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(180.0);
    // And a lead vehicle drives 245 m ahead at 0 km/h  [FN-S11]
    sil_.plant().set_lead(245.0, 0.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 0.4 s pass  [FN-S10]
    sil_.run(0.4);
    // Then braking was never requested  [ACC-S26]
    EXPECT_EQ(sil_.feature().metrics().max_brake_raw, 0);
    // And the distance to the lead vehicle is 205 m within 2 m  [FN-S43]
    ASSERT_TRUE(sil_.plant().lead_present()); EXPECT_NEAR(sil_.plant().distance_m(), 205.0, 2.0);
}

// Scenario: A vehicle just inside 200 m is a target
// Source:   examples/acc/features/control.feature:70
// Verifies: REQ-ACC-013
TEST_F(AccControllerSilTest, S26_AVehicleJustInside200MIsATarget) {
    // Given the ego vehicle drives at 180 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(180.0);
    // And a lead vehicle drives 215 m ahead at 0 km/h  [FN-S11]
    sil_.plant().set_lead(215.0, 0.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 0.2 s pass  [FN-S10]
    sil_.run(0.2);
    // Then braking is requested  [ACC-S25]
    EXPECT_GT(sil_.feature().last_command().brake_raw, 0);
    // And the distance to the lead vehicle is 185 m within 2 m  [FN-S43]
    ASSERT_TRUE(sil_.plant().lead_present()); EXPECT_NEAR(sil_.plant().distance_m(), 185.0, 2.0);
}

// Scenario: The lead vehicle brakes while being followed #1
// Source:   examples/acc/features/control.feature:79
// Verifies: REQ-ACC-016, REQ-ACC-014, REQ-ACC-017
// Example:  target=50, decel=2
TEST_F(AccControllerSilTest, S27_TheLeadVehicleBrakesWhileBeingFollowed_Ex1) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And a lead vehicle drives 46.7 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(46.7, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the lead vehicle changes speed to 50 km/h at 2 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(50.0, 2.0);
    // And 30 s pass  [FN-S10]
    sil_.run(30.0);
    // Then the distance to the lead vehicle never fell below 5 m  [FN-S44]
    EXPECT_GT(sil_.plant().metrics().min_distance_m, 5.0);
    // And no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the requested deceleration never exceeded 3.5 m/s2  [ACC-S28]
    EXPECT_LE(sil_.feature().metrics().max_brake_raw, ea::plant::Plant::accel_raw(3.5));
    // And acceleration and braking were never requested together  [ACC-S30]
    EXPECT_EQ(sil_.feature().metrics().both_requested, 0U);
}

// Scenario: The lead vehicle brakes while being followed #2
// Source:   examples/acc/features/control.feature:79
// Verifies: REQ-ACC-016, REQ-ACC-014, REQ-ACC-017
// Example:  target=50, decel=3
TEST_F(AccControllerSilTest, S28_TheLeadVehicleBrakesWhileBeingFollowed_Ex2) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And a lead vehicle drives 46.7 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(46.7, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the lead vehicle changes speed to 50 km/h at 3 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(50.0, 3.0);
    // And 30 s pass  [FN-S10]
    sil_.run(30.0);
    // Then the distance to the lead vehicle never fell below 5 m  [FN-S44]
    EXPECT_GT(sil_.plant().metrics().min_distance_m, 5.0);
    // And no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the requested deceleration never exceeded 3.5 m/s2  [ACC-S28]
    EXPECT_LE(sil_.feature().metrics().max_brake_raw, ea::plant::Plant::accel_raw(3.5));
    // And acceleration and braking were never requested together  [ACC-S30]
    EXPECT_EQ(sil_.feature().metrics().both_requested, 0U);
}

// Scenario: The lead vehicle brakes while being followed #3
// Source:   examples/acc/features/control.feature:79
// Verifies: REQ-ACC-016, REQ-ACC-014, REQ-ACC-017
// Example:  target=30, decel=3
TEST_F(AccControllerSilTest, S29_TheLeadVehicleBrakesWhileBeingFollowed_Ex3) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And a lead vehicle drives 46.7 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(46.7, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the lead vehicle changes speed to 30 km/h at 3 m/s2  [FN-S13]
    sil_.plant().lead_change_speed(30.0, 3.0);
    // And 30 s pass  [FN-S10]
    sil_.run(30.0);
    // Then the distance to the lead vehicle never fell below 5 m  [FN-S44]
    EXPECT_GT(sil_.plant().metrics().min_distance_m, 5.0);
    // And no collision occurred  [FN-S45]
    EXPECT_FALSE(sil_.plant().metrics().collision);
    // And the requested deceleration never exceeded 3.5 m/s2  [ACC-S28]
    EXPECT_LE(sil_.feature().metrics().max_brake_raw, ea::plant::Plant::accel_raw(3.5));
    // And acceleration and braking were never requested together  [ACC-S30]
    EXPECT_EQ(sil_.feature().metrics().both_requested, 0U);
}

// Scenario: A slower vehicle cuts in 25 m ahead
// Source:   examples/acc/features/control.feature:97
// Verifies: REQ-ACC-016, REQ-ACC-014, REQ-ACC-015
TEST_F(AccControllerSilTest, S30_ASlowerVehicleCutsIn25MAhead) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When a vehicle cuts in 25 m ahead at 80 km/h  [FN-S14]
    sil_.plant().cut_in(25.0, 80.0);
    // And 30 s pass  [FN-S10]
    sil_.run(30.0);
    // Then the distance to the lead vehicle never fell below 5 m  [FN-S44]
    EXPECT_GT(sil_.plant().metrics().min_distance_m, 5.0);
    // And the requested deceleration never exceeded 3.5 m/s2  [ACC-S28]
    EXPECT_LE(sil_.feature().metrics().max_brake_raw, ea::plant::Plant::accel_raw(3.5));
    // And the requested jerk never exceeded 2.5 m/s3 increasing and 5.0 m/s3 decreasing  [ACC-S29]
    EXPECT_LE(sil_.feature().metrics().max_jerk_up_raw, ea::plant::Plant::jerk_step_raw(2.5)); EXPECT_LE(sil_.feature().metrics().max_jerk_down_raw, ea::plant::Plant::jerk_step_raw(5.0));
    // And the distance to the lead vehicle is 38.3 m within 2 m  [FN-S43]
    ASSERT_TRUE(sil_.plant().lead_present()); EXPECT_NEAR(sil_.plant().distance_m(), 38.3, 2.0);
}

// Scenario: The lead vehicle leaves and the ego vehicle resumes the set speed
// Source:   examples/acc/features/control.feature:108
// Verifies: REQ-ACC-011
TEST_F(AccControllerSilTest, S31_TheLeadVehicleLeavesAndTheEgoVehicleResumesTheSetSpeed) {
    // Given the ego vehicle drives at 120 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(120.0);
    // And a lead vehicle drives 46.7 m ahead at 100 km/h  [FN-S11]
    sil_.plant().set_lead(46.7, 100.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 10 s pass  [FN-S10]
    sil_.run(10.0);
    // And the lead vehicle leaves the lane  [FN-S15]
    sil_.plant().remove_lead();
    // And 40 s pass  [FN-S10]
    sil_.run(40.0);
    // Then the ego speed is 120 km/h within 1 km/h  [FN-S40]
    EXPECT_NEAR(sil_.plant().ego_speed_kph(), 120.0, 1.0);
}

// Scenario: Every command frame is E2E protected and sent every cycle
// Source:   examples/acc/features/control.feature:118
// Verifies: REQ-ACC-020, REQ-ACC-018
TEST_F(AccControllerSilTest, S32_EveryCommandFrameIsE2EProtectedAndSentEveryCycle) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And a lead vehicle drives 60 m ahead at 90 km/h  [FN-S11]
    sil_.plant().set_lead(60.0, 90.0, 0.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When 10 s pass  [FN-S10]
    sil_.run(10.0);
    // Then every command frame passed the E2E check  [ACC-S32]
    EXPECT_GT(sil_.feature().metrics().command_frames, 0U); EXPECT_EQ(sil_.feature().metrics().command_e2e_errors, 0U);
    // And one command frame was sent per 20 ms  [ACC-S33]
    EXPECT_EQ(sil_.feature().metrics().command_frames, sil_.plant().metrics().ticks);
    // And the steering request was always 0 deg  [ACC-S31]
    EXPECT_EQ(sil_.feature().metrics().steering_nonzero, 0U);
}

// Scenario: Lost radar frames below the timeout are tolerated #1
// Source:   examples/acc/features/faults.feature:4
// Verifies: REQ-ACC-021
// Example:  n=2, fault=lost
TEST_F(AccControllerSilTest, S33_LostRadarFramesBelowTheTimeoutAreTolerated_Ex1) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the next 2 radar frames are lost  [FN-S16]
    sil_.plant().lose_frames(ea::plant::Stream::kRadar, 2U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
}

// Scenario: Lost radar frames below the timeout are tolerated #2
// Source:   examples/acc/features/faults.feature:4
// Verifies: REQ-ACC-021
// Example:  n=2, fault=corrupted
TEST_F(AccControllerSilTest, S34_LostRadarFramesBelowTheTimeoutAreTolerated_Ex2) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the next 2 radar frames are corrupted  [FN-S16]
    sil_.plant().corrupt_frames(ea::plant::Stream::kRadar, 2U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
}

// Scenario: Radar frames missing for 120 ms lead to FAULT #1
// Source:   examples/acc/features/faults.feature:17
// Verifies: REQ-ACC-021, REQ-ACC-019
// Example:  n=3, fault=lost
TEST_F(AccControllerSilTest, S35_RadarFramesMissingFor120MsLeadToFAULT_Ex1) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the next 3 radar frames are lost  [FN-S16]
    sil_.plant().lose_frames(ea::plant::Stream::kRadar, 3U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is FAULT  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kFault);
    // And neither acceleration nor braking is requested  [ACC-S21]
    EXPECT_EQ(sil_.feature().last_command().accel_raw, 0); EXPECT_EQ(sil_.feature().last_command().brake_raw, 0);
}

// Scenario: Radar frames missing for 120 ms lead to FAULT #2
// Source:   examples/acc/features/faults.feature:17
// Verifies: REQ-ACC-021, REQ-ACC-019
// Example:  n=3, fault=corrupted
TEST_F(AccControllerSilTest, S36_RadarFramesMissingFor120MsLeadToFAULT_Ex2) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the next 3 radar frames are corrupted  [FN-S16]
    sil_.plant().corrupt_frames(ea::plant::Stream::kRadar, 3U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is FAULT  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kFault);
    // And neither acceleration nor braking is requested  [ACC-S21]
    EXPECT_EQ(sil_.feature().last_command().accel_raw, 0); EXPECT_EQ(sil_.feature().last_command().brake_raw, 0);
}

// Scenario: Vehicle frames missing for 60 ms lead to FAULT #1
// Source:   examples/acc/features/faults.feature:31
// Verifies: REQ-ACC-021
// Example:  n=2, message=ego state, state=ACTIVE
TEST_F(AccControllerSilTest, S37_VehicleFramesMissingFor60MsLeadToFAULT_Ex1) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the next 2 ego state frames are lost  [FN-S17]
    sil_.plant().lose_frames(ea::plant::Stream::kEgoState, 2U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
}

// Scenario: Vehicle frames missing for 60 ms lead to FAULT #2
// Source:   examples/acc/features/faults.feature:31
// Verifies: REQ-ACC-021
// Example:  n=3, message=ego state, state=FAULT
TEST_F(AccControllerSilTest, S38_VehicleFramesMissingFor60MsLeadToFAULT_Ex2) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the next 3 ego state frames are lost  [FN-S17]
    sil_.plant().lose_frames(ea::plant::Stream::kEgoState, 3U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is FAULT  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kFault);
}

// Scenario: Vehicle frames missing for 60 ms lead to FAULT #3
// Source:   examples/acc/features/faults.feature:31
// Verifies: REQ-ACC-021
// Example:  n=2, message=driver input, state=ACTIVE
TEST_F(AccControllerSilTest, S39_VehicleFramesMissingFor60MsLeadToFAULT_Ex3) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the next 2 driver input frames are lost  [FN-S17]
    sil_.plant().lose_frames(ea::plant::Stream::kDriverInput, 2U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
}

// Scenario: Vehicle frames missing for 60 ms lead to FAULT #4
// Source:   examples/acc/features/faults.feature:31
// Verifies: REQ-ACC-021
// Example:  n=3, message=driver input, state=FAULT
TEST_F(AccControllerSilTest, S40_VehicleFramesMissingFor60MsLeadToFAULT_Ex4) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the next 3 driver input frames are lost  [FN-S17]
    sil_.plant().lose_frames(ea::plant::Stream::kDriverInput, 3U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is FAULT  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kFault);
}

// Scenario: A paused sender is detected exactly at the timeout #1
// Source:   examples/acc/features/faults.feature:46
// Verifies: REQ-ACC-021
// Example:  message=radar, n=2, state=ACTIVE
TEST_F(AccControllerSilTest, S41_APausedSenderIsDetectedExactlyAtTheTimeout_Ex1) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the radar sender pauses for 2 frames  [FN-S18]
    sil_.plant().skip_frames(ea::plant::Stream::kRadar, 2U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
}

// Scenario: A paused sender is detected exactly at the timeout #2
// Source:   examples/acc/features/faults.feature:46
// Verifies: REQ-ACC-021
// Example:  message=radar, n=3, state=FAULT
TEST_F(AccControllerSilTest, S42_APausedSenderIsDetectedExactlyAtTheTimeout_Ex2) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the radar sender pauses for 3 frames  [FN-S18]
    sil_.plant().skip_frames(ea::plant::Stream::kRadar, 3U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is FAULT  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kFault);
}

// Scenario: A paused sender is detected exactly at the timeout #3
// Source:   examples/acc/features/faults.feature:46
// Verifies: REQ-ACC-021
// Example:  message=ego state, n=2, state=ACTIVE
TEST_F(AccControllerSilTest, S43_APausedSenderIsDetectedExactlyAtTheTimeout_Ex3) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the ego state sender pauses for 2 frames  [FN-S18]
    sil_.plant().skip_frames(ea::plant::Stream::kEgoState, 2U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
}

// Scenario: A paused sender is detected exactly at the timeout #4
// Source:   examples/acc/features/faults.feature:46
// Verifies: REQ-ACC-021
// Example:  message=ego state, n=3, state=FAULT
TEST_F(AccControllerSilTest, S44_APausedSenderIsDetectedExactlyAtTheTimeout_Ex4) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the ego state sender pauses for 3 frames  [FN-S18]
    sil_.plant().skip_frames(ea::plant::Stream::kEgoState, 3U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is FAULT  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kFault);
}

// Scenario: A paused sender is detected exactly at the timeout #5
// Source:   examples/acc/features/faults.feature:46
// Verifies: REQ-ACC-021
// Example:  message=driver input, n=2, state=ACTIVE
TEST_F(AccControllerSilTest, S45_APausedSenderIsDetectedExactlyAtTheTimeout_Ex5) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver input sender pauses for 2 frames  [FN-S18]
    sil_.plant().skip_frames(ea::plant::Stream::kDriverInput, 2U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
}

// Scenario: A paused sender is detected exactly at the timeout #6
// Source:   examples/acc/features/faults.feature:46
// Verifies: REQ-ACC-021
// Example:  message=driver input, n=3, state=FAULT
TEST_F(AccControllerSilTest, S46_APausedSenderIsDetectedExactlyAtTheTimeout_Ex6) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the driver input sender pauses for 3 frames  [FN-S18]
    sil_.plant().skip_frames(ea::plant::Stream::kDriverInput, 3U);
    // And 1 s passes  [FN-S10]
    sil_.run(1.0);
    // Then the ACC state is FAULT  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kFault);
}

// Scenario: FAULT is latched until the main switch is turned off
// Source:   examples/acc/features/faults.feature:63
// Verifies: REQ-ACC-022
TEST_F(AccControllerSilTest, S47_FAULTIsLatchedUntilTheMainSwitchIsTurnedOff) {
    // Given the ego vehicle drives at 100 km/h  [FN-S01]
    sil_.plant().set_ego_speed_kph(100.0);
    // And ACC is engaged  [ACC-S03]
    sil_.engage();
    // When the next 5 radar frames are lost  [FN-S16]
    sil_.plant().lose_frames(ea::plant::Stream::kRadar, 5U);
    // And 2 s pass  [FN-S10]
    sil_.run(2.0);
    // And the driver presses the set button  [FN-S04]
    sil_.press(ea::plant::Button::kSet, 1);
    // Then the ACC state is FAULT  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kFault);
    // When the driver switches ACC off  [FN-S02]
    sil_.switch_main(false);
    // Then the ACC state is OFF  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kOff);
    // When the driver switches ACC on  [FN-S02]
    sil_.switch_main(true);
    // And the driver presses the set button  [FN-S04]
    sil_.press(ea::plant::Button::kSet, 1);
    // Then the ACC state is ACTIVE  [ACC-S20]
    EXPECT_EQ(sil_.feature().last_command().status, ea::plant::AccStatus::kActive);
}

}  // namespace
}  // namespace adas::acc
