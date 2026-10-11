# Closed-loop SIL scenarios for FCW + AEB. Steps come from sil/steps/can_function.yaml and
# sil/steps/features/aeb.yaml only. TTC is measured by the plant from its own world state;
# timing tolerance 0.1 s = radar period (40 ms) + cycle (20 ms) + range quantization.
Feature: AEB warning and braking cascade

  @REQ-AEB-001 @REQ-AEB-002 @REQ-AEB-003 @REQ-AEB-005 @REQ-AEB-014
  Scenario: Approach to a stationary vehicle runs through the whole cascade
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 60 m ahead
    When 8 s pass
    Then a forward collision warning was issued at a time to collision between 2.7 s and 2.9 s
    And the warning preceded braking by at least 0.8 s
    And emergency braking started at a time to collision between 1.7 s and 1.9 s
    And emergency braking started with 4 m/s2
    And full braking started at a time to collision between 1.1 s and 1.3 s
    And the requested deceleration reached 8 m/s2
    And no collision occurred
    And the ego vehicle has stopped
    And the AEB state is STANDBY
    And every command frame passed the E2E check
    And one command frame was sent per 20 ms

  @REQ-AEB-003
  Scenario: A late cut-in triggers full braking without the minimum warning time
    Given the ego vehicle drives at 50 km/h
    When a vehicle cuts in 9 m ahead at 20 km/h
    And 0.2 s pass
    Then the AEB state is FULL_BRAKE
    And a deceleration of 8 m/s2 is requested
    And the warning is on

  @REQ-AEB-002
  Scenario: A hard-braking lead vehicle still gets the minimum warning time
    Given the ego vehicle drives at 80 km/h
    And a lead vehicle drives 40 m ahead at 80 km/h
    When the lead vehicle changes speed to 0 km/h at 6 m/s2
    And 10 s pass
    Then the warning preceded braking by at least 0.8 s

  @REQ-AEB-004
  Scenario: The warning ends when the time to collision rises above 3.3 s
    Given the ego vehicle drives at 50 km/h
    And a lead vehicle drives 25 m ahead at 30 km/h
    When 2 s pass
    Then the AEB state is WARNING
    When the lead vehicle changes speed to 80 km/h at 2 m/s2
    And 3 s pass
    Then the warning ended at a time to collision between 3.3 s and 3.5 s
    And emergency braking was never requested

  @REQ-AEB-004
  Scenario: The warning ends when the lead vehicle pulls away
    Given the ego vehicle drives at 50 km/h
    And a lead vehicle drives 25 m ahead at 30 km/h
    When 2 s pass
    Then the AEB state is WARNING
    When the lead vehicle changes speed to 80 km/h at 4 m/s2
    And 2 s pass
    Then the AEB state is STANDBY
    And the warning is off
    And emergency braking was never requested

  @REQ-AEB-005
  Scenario: Braking ends once the ego vehicle no longer approaches
    Given the ego vehicle drives at 60 km/h
    And a lead vehicle drives 40 m ahead at 20 km/h
    When 10 s pass
    Then the AEB state is STANDBY
    And no collision occurred
    And the ego vehicle is still moving

  @REQ-AEB-006
  Scenario Outline: Kickdown cancels the intervention, a lighter pedal does not
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 60 m ahead
    When 3 s pass
    Then the warning is on
    When the driver holds the accelerator pedal at <pedal> %
    And 0.2 s pass
    Then <result>

    Examples:
      | pedal | result                              |
      | 85    | the AEB state is STANDBY            |
      | 75    | emergency braking is requested      |

  @REQ-AEB-006
  Scenario: Kickdown also cancels full braking
    Given the ego vehicle drives at 50 km/h
    When a vehicle cuts in 9 m ahead at 20 km/h
    And 0.1 s pass
    Then the AEB state is FULL_BRAKE
    When the driver holds the accelerator pedal at 85 %
    And 0.1 s pass
    Then the AEB state is STANDBY

  @REQ-AEB-007
  Scenario Outline: The driver's brake pedal does not cancel emergency braking
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 60 m ahead
    When <t> s pass
    Then the AEB state is <stage>
    When the driver holds the brake pedal
    And 0.04 s pass
    Then the AEB state is <stage>
    When 5 s pass
    Then no collision occurred
    And the requested deceleration reached 8 m/s2
    And the ego vehicle has stopped

    Examples:
      | t   | stage         |
      | 3   | PARTIAL_BRAKE |
      | 3.6 | FULL_BRAKE    |
