# Scenarios may only use steps from sil/steps/gpu_stage.yaml.
Feature: ObstacleScoreStage inference with independent ObstacleScoreMonitor

  @REQ-OBS-003
  Scenario Outline: Scores match the reference model
    Given an initialized stage
    When the stage processes a <input> input
    Then the output status is VALID
    And score car equals <car> within 0.0001
    And score pedestrian equals <pedestrian> within 0.0001
    And score cyclist equals <cyclist> within 0.0001
    And score background equals <background> within 0.0001

    Examples:
      | input        | car      | pedestrian | cyclist  | background |
      | ramp         | 0.279046 | 0.240318   | 0.240318 | 0.240318   |
      | zero         | 0.265894 | 0.240590   | 0.252926 | 0.240590   |
      | constant 128 | 0.254661 | 0.248446   | 0.248446 | 0.248446   |
      | constant 255 | 0.250000 | 0.250000   | 0.250000 | 0.250000   |

  @REQ-OBS-001
  Scenario: No allocation while running
    Given an initialized stage
    When the stage processes 100 ramp inputs
    Then no memory was allocated after initialization
    And the output status is VALID

  @REQ-OBS-002
  Scenario Outline: Tampered weights are rejected
    Given the weight file <tensor> is corrupted
    When the stage is initialized
    Then initialization fails with IntegrityError

    Examples:
      | tensor |
      | fc1_w  |
      | fc1_b  |

  @REQ-OBS-002
  Scenario: Untampered weights are accepted
    When the stage is initialized
    Then initialization succeeds

  @REQ-OBS-004
  Scenario: A slow backend produces DEADLINE_MISS
    Given an initialized stage
    And the backend takes 34 ms per execution
    When the stage processes a ramp input
    Then the output status is DEADLINE_MISS

  @REQ-OBS-004
  Scenario: A backend just inside the deadline is accepted
    Given an initialized stage
    And the backend takes 33 ms per execution
    When the stage processes a ramp input
    Then the output status is VALID

  @REQ-OBS-005
  Scenario Outline: Implausible scores are flagged by the stage
    Given an initialized stage
    And the backend overwrites score <class> with <value>
    When the stage processes a ramp input
    Then the output status is IMPLAUSIBLE

    Examples:
      | class      | value |
      | car        | NaN   |
      | pedestrian | 1.5   |
      | cyclist    | -0.1  |
      | background | 0.5   |

  @REQ-OBS-008
  Scenario: Monitor reports NO_DATA before the first output
    Given an initialized stage
    When the monitor checks the latest output
    Then the monitor reports NO_DATA

  @REQ-OBS-006
  Scenario: Monitor detects a stalled stage
    Given an initialized stage
    When the stage processes a ramp input
    And the monitor checks the latest output after 66 ms
    Then the monitor reports OK
    When the monitor checks the latest output after 1 ms
    Then the monitor reports TIMEOUT

  @REQ-OBS-007
  Scenario Outline: Monitor repeats plausibility checks independently
    Given an initialized stage
    And the channel overwrites score <class> with <value>
    When the stage processes a ramp input
    Then the output status is VALID
    When the monitor checks the latest output
    Then the monitor reports IMPLAUSIBLE

    Examples:
      | class      | value |
      | car        | NaN   |
      | pedestrian | 2.0   |
      | background | 0.9   |

  @REQ-OBS-007 @REQ-OBS-005
  Scenario: Monitor propagates a stage-detected fault
    Given an initialized stage
    And the backend overwrites score car with NaN
    When the stage processes a ramp input
    And the monitor checks the latest output
    Then the monitor reports IMPLAUSIBLE
