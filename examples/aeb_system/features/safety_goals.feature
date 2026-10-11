# System SIL: the generated AebController and AebMonitor on one bus, the actuator ECU model
# (AoU) applying only approved commands. "Fault detection budget" = FTTI minus the actuator's
# assumed reaction time (100 ms), resolved from examples/aeb_system/model.yaml.
Feature: AEB safety goals under node faults

  @REQ-AEBS-001
  Scenario Outline: Braking requested on a free road is never applied
    Given the ego vehicle drives at 80 km/h
    When 1 s pass
    And the controller sends AebBrakeRequest = <a> with a valid CRC for 100 frames
    And 2 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason REQUEST_OUTSIDE_BRAKING
    And the actuator entered its safe state within the fault detection budget of REQ-AEBS-001
    And the applied deceleration never exceeded 0 m/s2
    And the actual deceleration of the vehicle never exceeded 0.1 m/s2
    And the actuator requests a driver takeover

    Examples:
      | a   |
      | 4.0 |
      | 8.0 |

  @REQ-AEBS-001
  Scenario: Full braking requested during a warning is never applied
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 60 m ahead
    When 1.6 s pass
    Then the AEB state is WARNING
    When the controller sends AebBrakeRequest = 8.0 with a valid CRC for 5 frames
    And 0.2 s pass
    Then the monitor state is SAFE_STATE
    And the actuator entered its safe state within the fault detection budget of REQ-AEBS-001
    And the applied deceleration never exceeded 0 m/s2

  @REQ-AEBS-001
  Scenario: Braking above the full braking level is never applied
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 60 m ahead
    When 4 s pass
    Then the AEB state is FULL_BRAKE
    When the controller sends AebBrakeRequest = 9.5 with a valid CRC for 100 frames
    And 0.2 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason REQUEST_LIMIT
    And the actuator entered its safe state within the fault detection budget of REQ-AEBS-001
    And the applied deceleration never exceeded 8.1 m/s2

  @REQ-AEBS-002 @REQ-AEBS-003
  Scenario Outline: A stopped or frozen controller during full braking is ramped out
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 60 m ahead
    When 4 s pass
    Then the AEB state is FULL_BRAKE
    When the command sender <f>
    And 2 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason COMMAND_TIMEOUT
    And the actuator entered its safe state within the fault detection budget of REQ-AEBS-002
    And the request was ramped out within 460 ms of the first fault
    And the actuator requests a driver takeover

    Examples:
      | f       |
      | stops   |
      | freezes |

  @REQ-AEBS-003
  Scenario: A lost monitor during braking makes the actuator ramp out
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 60 m ahead
    When 4 s pass
    And the monitor sender stops
    And 2 s pass
    Then the actuator requests a driver takeover
    And the actuator entered its safe state within 40 ms of the first fault
    And the actuator applies no request
