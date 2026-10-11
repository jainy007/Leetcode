# Faults of the command sender and of the monitor itself. A frame loss starting at t0 leaves the
# last valid frame at t0 - 20 ms, so the 60 ms command timeout (REQ-ACCM-008) expires at
# t0 + 40 ms. The actuator ECU (AoU-3) also uses a 60 ms timeout for the monitor message.
Feature: ACC monitor faults

  @REQ-ACCM-008 @REQ-ACCM-009
  Scenario Outline: A stopped or frozen command sender latches SAFE_STATE within the timeout
    Given the ego vehicle drives at 100 km/h
    And the controller is ACTIVE without request
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

  @REQ-ACCM-008
  Scenario Outline: Up to two lost or corrupted command frames are tolerated
    Given the ego vehicle drives at 100 km/h
    And the controller is ACTIVE without request
    When 0.2 s pass
    And the next 2 command frames are <f>
    And 0.5 s pass
    Then the monitor state is MONITORING
    And the monitor never entered SAFE_STATE

    Examples:
      | f         |
      | lost      |
      | corrupted |

  @REQ-ACCM-001 @REQ-ACCM-008
  Scenario: Without any command the monitor stays in INIT and grants no permit
    Given the ego vehicle drives at 100 km/h
    When the command sender stops
    And 1 s pass
    Then the monitor state is INIT
    And the permit is withdrawn
    And the monitor never entered SAFE_STATE
    And the actuator applies no request

  @REQ-ACCM-010
  Scenario: A lost monitor makes the actuator ramp out braking and request a takeover
    Given the ego vehicle drives at 100 km/h
    And the controller is ACTIVE without request
    When 0.2 s pass
    And the controller ramps the deceleration request to 2.0 m/s2 at 5.0 m/s3
    And the monitor sender stops
    And 1 s pass
    Then the actuator requests a driver takeover
    And the actuator entered its safe state within 40 ms of the first fault
    And the request was ramped out within 460 ms of the first fault
    And the actuator applies no request
