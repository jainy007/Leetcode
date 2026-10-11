# Fault-free AEB scenarios with the monitor and the actuator gate in the loop: same expected
# values as examples/aeb (REQ-AEB-010..012), plus no intervention of the monitor.
Feature: AEB availability with the safety monitor in the loop

  @REQ-AEBS-004
  Scenario Outline: A stationary vehicle is still avoided
    Given the ego vehicle drives at <v> km/h
    And a stationary vehicle stands <d> m ahead
    When 15 s pass
    Then no collision occurred
    And the ego vehicle has stopped
    And the monitor never entered SAFE_STATE
    And the actuator applied every command

    Examples:
      | v  | d  |
      | 20 | 40 |
      | 40 | 60 |
      | 60 | 80 |

  @REQ-AEBS-004
  Scenario: The impact speed at 80 km/h stays at 40 km/h or less
    Given the ego vehicle drives at 80 km/h
    And a stationary vehicle stands 100 m ahead
    When 15 s pass
    Then the impact speed was at most 40 km/h
    And the monitor never entered SAFE_STATE
    And the actuator applied every command

  @REQ-AEBS-004
  Scenario: A vehicle driving at 20 km/h is avoided from 80 km/h
    Given the ego vehicle drives at 80 km/h
    And a lead vehicle drives 100 m ahead at 20 km/h
    When 15 s pass
    Then no collision occurred
    And the monitor never entered SAFE_STATE
    And the actuator applied every command

  @REQ-AEBS-004
  Scenario: A late cut-in is braked for at once
    Given the ego vehicle drives at 50 km/h
    When a vehicle cuts in 9 m ahead at 20 km/h
    And 0.2 s pass
    Then the AEB state is FULL_BRAKE
    And the actuator applies -8.0 m/s2
    And the monitor never entered SAFE_STATE

  @REQ-AEBS-004
  Scenario: Kickdown cancels braking without a monitor intervention
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 60 m ahead
    When 4 s pass
    And the driver presses the accelerator pedal to 90 %
    And 0.2 s pass
    Then the AEB state is STANDBY
    And the monitor never entered SAFE_STATE
    And the actuator applied every command
