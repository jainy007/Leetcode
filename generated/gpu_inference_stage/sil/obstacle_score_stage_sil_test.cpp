// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   gpu_inference_stage (pattern: gpu_stage)
// Inputs:    sha256:8f100abd5d48e45bbdc692f79a474a0077f34d8c008e44abb1f2adfcc38e0c85
// Regenerate: python3 -m tools.gen

#include <gtest/gtest.h>

#include <limits>

#include "sil/obstacle_score_stage_sil.hpp"

// SIL tests generated from the Gherkin scenarios; each step expands from the step catalogue
// sil/steps/gpu_stage.yaml. Test names are referenced by trace.json.

namespace adas::obstacle_score {
namespace {

class ObstacleScoreStageSilTest : public ::testing::Test {
protected:
    ObstacleScoreStageSil sil_;
};

// Scenario: Scores match the reference model #1
// Source:   examples/gpu_inference_stage/features/stage.feature:5
// Verifies: REQ-OBS-003
// Example:  input=ramp, car=0.279046, pedestrian=0.240318, cyclist=0.240318, background=0.240318
TEST_F(ObstacleScoreStageSilTest, S01_ScoresMatchTheReferenceModel_Ex1) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // When the stage processes a ramp input  [GPU-S06]
    sil_.process_ramp(1);
    // Then the output status is VALID  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kValid);
    // And score car equals 0.279046 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassCar], 0.279046, 0.0001);
    // And score pedestrian equals 0.240318 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassPedestrian], 0.240318, 0.0001);
    // And score cyclist equals 0.240318 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassCyclist], 0.240318, 0.0001);
    // And score background equals 0.240318 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassBackground], 0.240318, 0.0001);
}

// Scenario: Scores match the reference model #2
// Source:   examples/gpu_inference_stage/features/stage.feature:5
// Verifies: REQ-OBS-003
// Example:  input=zero, car=0.265894, pedestrian=0.240590, cyclist=0.252926, background=0.240590
TEST_F(ObstacleScoreStageSilTest, S02_ScoresMatchTheReferenceModel_Ex2) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // When the stage processes a zero input  [GPU-S08]
    sil_.process_constant(0U);
    // Then the output status is VALID  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kValid);
    // And score car equals 0.265894 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassCar], 0.265894, 0.0001);
    // And score pedestrian equals 0.240590 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassPedestrian], 0.240590, 0.0001);
    // And score cyclist equals 0.252926 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassCyclist], 0.252926, 0.0001);
    // And score background equals 0.240590 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassBackground], 0.240590, 0.0001);
}

// Scenario: Scores match the reference model #3
// Source:   examples/gpu_inference_stage/features/stage.feature:5
// Verifies: REQ-OBS-003
// Example:  input=constant 128, car=0.254661, pedestrian=0.248446, cyclist=0.248446, background=0.248446
TEST_F(ObstacleScoreStageSilTest, S03_ScoresMatchTheReferenceModel_Ex3) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // When the stage processes a constant 128 input  [GPU-S09]
    sil_.process_constant(128U);
    // Then the output status is VALID  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kValid);
    // And score car equals 0.254661 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassCar], 0.254661, 0.0001);
    // And score pedestrian equals 0.248446 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassPedestrian], 0.248446, 0.0001);
    // And score cyclist equals 0.248446 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassCyclist], 0.248446, 0.0001);
    // And score background equals 0.248446 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassBackground], 0.248446, 0.0001);
}

// Scenario: Scores match the reference model #4
// Source:   examples/gpu_inference_stage/features/stage.feature:5
// Verifies: REQ-OBS-003
// Example:  input=constant 255, car=0.250000, pedestrian=0.250000, cyclist=0.250000, background=0.250000
TEST_F(ObstacleScoreStageSilTest, S04_ScoresMatchTheReferenceModel_Ex4) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // When the stage processes a constant 255 input  [GPU-S09]
    sil_.process_constant(255U);
    // Then the output status is VALID  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kValid);
    // And score car equals 0.250000 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassCar], 0.250000, 0.0001);
    // And score pedestrian equals 0.250000 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassPedestrian], 0.250000, 0.0001);
    // And score cyclist equals 0.250000 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassCyclist], 0.250000, 0.0001);
    // And score background equals 0.250000 within 0.0001  [GPU-S11]
    EXPECT_NEAR(sil_.last_output().scores[kClassBackground], 0.250000, 0.0001);
}

// Scenario: No allocation while running
// Source:   examples/gpu_inference_stage/features/stage.feature:22
// Verifies: REQ-OBS-001
TEST_F(ObstacleScoreStageSilTest, S05_NoAllocationWhileRunning) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // When the stage processes 100 ramp inputs  [GPU-S07]
    sil_.process_ramp(100);
    // Then no memory was allocated after initialization  [GPU-S12]
    EXPECT_EQ(sil_.allocations_since_init(), 0U);
    // And the output status is VALID  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kValid);
}

// Scenario: Tampered weights are rejected #1
// Source:   examples/gpu_inference_stage/features/stage.feature:29
// Verifies: REQ-OBS-002
// Example:  tensor=fc1_w
TEST_F(ObstacleScoreStageSilTest, S06_TamperedWeightsAreRejected_Ex1) {
    // Given the weight file fc1_w is corrupted  [GPU-S02]
    sil_.corrupt_weight_file(Weight::kFc1W);
    // When the stage is initialized  [GPU-S03]
    const ea::Result<void> init_result = sil_.init();
    // Then initialization fails with IntegrityError  [GPU-S04]
    EXPECT_EQ(init_result.error(), ea::ErrorCode::kIntegrityError);
}

// Scenario: Tampered weights are rejected #2
// Source:   examples/gpu_inference_stage/features/stage.feature:29
// Verifies: REQ-OBS-002
// Example:  tensor=fc1_b
TEST_F(ObstacleScoreStageSilTest, S07_TamperedWeightsAreRejected_Ex2) {
    // Given the weight file fc1_b is corrupted  [GPU-S02]
    sil_.corrupt_weight_file(Weight::kFc1B);
    // When the stage is initialized  [GPU-S03]
    const ea::Result<void> init_result = sil_.init();
    // Then initialization fails with IntegrityError  [GPU-S04]
    EXPECT_EQ(init_result.error(), ea::ErrorCode::kIntegrityError);
}

// Scenario: Untampered weights are accepted
// Source:   examples/gpu_inference_stage/features/stage.feature:40
// Verifies: REQ-OBS-002
TEST_F(ObstacleScoreStageSilTest, S08_UntamperedWeightsAreAccepted) {
    // When the stage is initialized  [GPU-S03]
    const ea::Result<void> init_result = sil_.init();
    // Then initialization succeeds  [GPU-S05]
    EXPECT_TRUE(init_result.ok());
}

// Scenario: A slow backend produces DEADLINE_MISS
// Source:   examples/gpu_inference_stage/features/stage.feature:45
// Verifies: REQ-OBS-004
TEST_F(ObstacleScoreStageSilTest, S09_ASlowBackendProducesDEADLINEMISS) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // And the backend takes 34 ms per execution  [GPU-S13]
    sil_.backend().set_execution_delay_ms(34);
    // When the stage processes a ramp input  [GPU-S06]
    sil_.process_ramp(1);
    // Then the output status is DEADLINE_MISS  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kDeadlineMiss);
}

// Scenario: A backend just inside the deadline is accepted
// Source:   examples/gpu_inference_stage/features/stage.feature:52
// Verifies: REQ-OBS-004
TEST_F(ObstacleScoreStageSilTest, S10_ABackendJustInsideTheDeadlineIsAccepted) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // And the backend takes 33 ms per execution  [GPU-S13]
    sil_.backend().set_execution_delay_ms(33);
    // When the stage processes a ramp input  [GPU-S06]
    sil_.process_ramp(1);
    // Then the output status is VALID  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kValid);
}

// Scenario: Implausible scores are flagged by the stage #1
// Source:   examples/gpu_inference_stage/features/stage.feature:59
// Verifies: REQ-OBS-005
// Example:  class=car, value=NaN
TEST_F(ObstacleScoreStageSilTest, S11_ImplausibleScoresAreFlaggedByTheStage_Ex1) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // And the backend overwrites score car with NaN  [GPU-S14]
    sil_.backend().overwrite_output(kClassCar, std::numeric_limits<float>::quiet_NaN());
    // When the stage processes a ramp input  [GPU-S06]
    sil_.process_ramp(1);
    // Then the output status is IMPLAUSIBLE  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kImplausible);
}

// Scenario: Implausible scores are flagged by the stage #2
// Source:   examples/gpu_inference_stage/features/stage.feature:59
// Verifies: REQ-OBS-005
// Example:  class=pedestrian, value=1.5
TEST_F(ObstacleScoreStageSilTest, S12_ImplausibleScoresAreFlaggedByTheStage_Ex2) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // And the backend overwrites score pedestrian with 1.5  [GPU-S14]
    sil_.backend().overwrite_output(kClassPedestrian, 1.5F);
    // When the stage processes a ramp input  [GPU-S06]
    sil_.process_ramp(1);
    // Then the output status is IMPLAUSIBLE  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kImplausible);
}

// Scenario: Implausible scores are flagged by the stage #3
// Source:   examples/gpu_inference_stage/features/stage.feature:59
// Verifies: REQ-OBS-005
// Example:  class=cyclist, value=-0.1
TEST_F(ObstacleScoreStageSilTest, S13_ImplausibleScoresAreFlaggedByTheStage_Ex3) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // And the backend overwrites score cyclist with -0.1  [GPU-S14]
    sil_.backend().overwrite_output(kClassCyclist, -0.1F);
    // When the stage processes a ramp input  [GPU-S06]
    sil_.process_ramp(1);
    // Then the output status is IMPLAUSIBLE  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kImplausible);
}

// Scenario: Implausible scores are flagged by the stage #4
// Source:   examples/gpu_inference_stage/features/stage.feature:59
// Verifies: REQ-OBS-005
// Example:  class=background, value=0.5
TEST_F(ObstacleScoreStageSilTest, S14_ImplausibleScoresAreFlaggedByTheStage_Ex4) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // And the backend overwrites score background with 0.5  [GPU-S14]
    sil_.backend().overwrite_output(kClassBackground, 0.5F);
    // When the stage processes a ramp input  [GPU-S06]
    sil_.process_ramp(1);
    // Then the output status is IMPLAUSIBLE  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kImplausible);
}

// Scenario: Monitor reports NO_DATA before the first output
// Source:   examples/gpu_inference_stage/features/stage.feature:73
// Verifies: REQ-OBS-008
TEST_F(ObstacleScoreStageSilTest, S15_MonitorReportsNODATABeforeTheFirstOutput) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // When the monitor checks the latest output  [GPU-S16]
    sil_.monitor_check(0);
    // Then the monitor reports NO_DATA  [GPU-S18]
    EXPECT_EQ(sil_.last_verdict(), MonitorVerdict::kNoData);
}

// Scenario: Monitor detects a stalled stage
// Source:   examples/gpu_inference_stage/features/stage.feature:79
// Verifies: REQ-OBS-006
TEST_F(ObstacleScoreStageSilTest, S16_MonitorDetectsAStalledStage) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // When the stage processes a ramp input  [GPU-S06]
    sil_.process_ramp(1);
    // And the monitor checks the latest output after 66 ms  [GPU-S17]
    sil_.monitor_check(66);
    // Then the monitor reports OK  [GPU-S18]
    EXPECT_EQ(sil_.last_verdict(), MonitorVerdict::kOk);
    // When the monitor checks the latest output after 1 ms  [GPU-S17]
    sil_.monitor_check(1);
    // Then the monitor reports TIMEOUT  [GPU-S18]
    EXPECT_EQ(sil_.last_verdict(), MonitorVerdict::kTimeout);
}

// Scenario: Monitor repeats plausibility checks independently #1
// Source:   examples/gpu_inference_stage/features/stage.feature:88
// Verifies: REQ-OBS-007
// Example:  class=car, value=NaN
TEST_F(ObstacleScoreStageSilTest, S17_MonitorRepeatsPlausibilityChecksIndependently_Ex1) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // And the channel overwrites score car with NaN  [GPU-S15]
    sil_.corrupt_channel(kClassCar, std::numeric_limits<float>::quiet_NaN());
    // When the stage processes a ramp input  [GPU-S06]
    sil_.process_ramp(1);
    // Then the output status is VALID  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kValid);
    // When the monitor checks the latest output  [GPU-S16]
    sil_.monitor_check(0);
    // Then the monitor reports IMPLAUSIBLE  [GPU-S18]
    EXPECT_EQ(sil_.last_verdict(), MonitorVerdict::kImplausible);
}

// Scenario: Monitor repeats plausibility checks independently #2
// Source:   examples/gpu_inference_stage/features/stage.feature:88
// Verifies: REQ-OBS-007
// Example:  class=pedestrian, value=2.0
TEST_F(ObstacleScoreStageSilTest, S18_MonitorRepeatsPlausibilityChecksIndependently_Ex2) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // And the channel overwrites score pedestrian with 2.0  [GPU-S15]
    sil_.corrupt_channel(kClassPedestrian, 2.0F);
    // When the stage processes a ramp input  [GPU-S06]
    sil_.process_ramp(1);
    // Then the output status is VALID  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kValid);
    // When the monitor checks the latest output  [GPU-S16]
    sil_.monitor_check(0);
    // Then the monitor reports IMPLAUSIBLE  [GPU-S18]
    EXPECT_EQ(sil_.last_verdict(), MonitorVerdict::kImplausible);
}

// Scenario: Monitor repeats plausibility checks independently #3
// Source:   examples/gpu_inference_stage/features/stage.feature:88
// Verifies: REQ-OBS-007
// Example:  class=background, value=0.9
TEST_F(ObstacleScoreStageSilTest, S19_MonitorRepeatsPlausibilityChecksIndependently_Ex3) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // And the channel overwrites score background with 0.9  [GPU-S15]
    sil_.corrupt_channel(kClassBackground, 0.9F);
    // When the stage processes a ramp input  [GPU-S06]
    sil_.process_ramp(1);
    // Then the output status is VALID  [GPU-S10]
    EXPECT_EQ(sil_.last_output().status, OutputStatus::kValid);
    // When the monitor checks the latest output  [GPU-S16]
    sil_.monitor_check(0);
    // Then the monitor reports IMPLAUSIBLE  [GPU-S18]
    EXPECT_EQ(sil_.last_verdict(), MonitorVerdict::kImplausible);
}

// Scenario: Monitor propagates a stage-detected fault
// Source:   examples/gpu_inference_stage/features/stage.feature:103
// Verifies: REQ-OBS-007, REQ-OBS-005
TEST_F(ObstacleScoreStageSilTest, S20_MonitorPropagatesAStageDetectedFault) {
    // Given an initialized stage  [GPU-S01]
    ASSERT_TRUE(sil_.init().ok());
    // And the backend overwrites score car with NaN  [GPU-S14]
    sil_.backend().overwrite_output(kClassCar, std::numeric_limits<float>::quiet_NaN());
    // When the stage processes a ramp input  [GPU-S06]
    sil_.process_ramp(1);
    // And the monitor checks the latest output  [GPU-S16]
    sil_.monitor_check(0);
    // Then the monitor reports IMPLAUSIBLE  [GPU-S18]
    EXPECT_EQ(sil_.last_verdict(), MonitorVerdict::kImplausible);
}

}  // namespace
}  // namespace adas::obstacle_score
