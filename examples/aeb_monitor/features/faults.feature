# Faults of the command sender. A frame loss starting at t0 leaves the last valid frame at
# t0 - 20 ms, so the 60 ms command timeout (REQ-AEBM-008) expires at t0 + 40 ms.
Feature: AEB monitor faults

  @REQ-AEBM-008 @REQ-AEBM-009
  Scenario Outline: A stopped or frozen command sender latches SAFE_STATE within the timeout
    Given the ego vehicle drives at 50 km/h
    And the AEB controller is in STANDBY with 0 m/s2 braking
    When 0.2 s pass
    And the command sender <f>
    And 0.5 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason COMMAND_TIMEOUT
    And the monitor entered SAFE_STATE within 40 ms of the first fault
    And the permit is withdrawn

    Examples:
      | f       |
      | stops   |
      | freezes |

  @REQ-AEBM-008
  Scenario Outline: Up to two lost or corrupted command frames are tolerated
    Given the ego vehicle drives at 50 km/h
    And the AEB controller is in STANDBY with 0 m/s2 braking
    When 0.2 s pass
    And the next 2 command frames are <f>
    And 0.5 s pass
    Then the monitor state is MONITORING
    And the monitor never entered SAFE_STATE

    Examples:
      | f         |
      | lost      |
      | corrupted |

  @REQ-AEBM-001 @REQ-AEBM-008
  Scenario: Without any command the monitor stays in INIT and grants no permit
    Given the ego vehicle drives at 50 km/h
    When the command sender stops
    And 1 s pass
    Then the monitor state is INIT
    And the permit is withdrawn
    And the monitor never entered SAFE_STATE

  @REQ-AEBM-010
  Scenario: A frozen monitor makes the actuator ramp out braking and request a takeover
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 22 m ahead
    And the AEB controller is in STANDBY with 0 m/s2 braking
    When 0.1 s pass
    And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking
    And 0.1 s pass
    And the monitor sender freezes
    And 1 s pass
    Then the actuator requests a driver takeover
    And the actuator entered its safe state within 40 ms of the first fault
    And the request was ramped out within 260 ms of the first fault
    And the actuator applies no request

  @REQ-AEBM-001
  Scenario: A radar pause without braking does not trigger the monitor
    Given the ego vehicle drives at 50 km/h
    And the AEB controller is in STANDBY with 0 m/s2 braking
    When 0.2 s pass
    And the radar sender pauses for 10 frames
    And 1 s pass
    Then the monitor state is MONITORING
    And the monitor never entered SAFE_STATE
