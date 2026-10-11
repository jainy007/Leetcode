# Component SIL of the ACC safety monitor. A stimulus node stands in for the controller; the
# actuator ECU model (AoU) applies only commands the monitor approves. Expected values come from
# examples/acc_monitor/requirements.yaml: limits 2.1 / 3.6 m/s2 (2.0 / 3.5 + 0.1 tolerance),
# rate limits 3.5 / 6 m/s3; while TTC < 3 s acceleration must decay by at least 4 m/s3.
Feature: ACC monitor safety envelope

  @REQ-ACCM-001 @REQ-ACCM-002 @REQ-ACCM-010
  Scenario: Commands inside the envelope are approved and applied
    Given the ego vehicle drives at 100 km/h
    And the controller is ACTIVE without request
    When 0.2 s pass
    Then the monitor state is MONITORING
    And the permit is granted
    When the controller ramps the acceleration request to 2.0 m/s2 at 2.5 m/s3
    And the controller ramps the deceleration request to 3.5 m/s2 at 5.0 m/s3
    And 1 s pass
    Then the monitor never entered SAFE_STATE
    And the actuator applied every command
    And the actuator applies -3.5 m/s2
    And the actuator does not request a driver takeover
    And every monitor frame passed the E2E check
    And one monitor frame was sent per 20 ms

  @REQ-ACCM-003
  Scenario Outline: Requests beyond the limits plus tolerance latch SAFE_STATE
    Given the ego vehicle drives at 100 km/h
    And the controller is ACTIVE without request
    When 0.2 s pass
    And the controller ramps the <dir> request to <a> m/s2 at 2.5 m/s3
    And 0.2 s pass
    Then the monitor state is <state>
    And the monitor reports the fault reason <reason>

    Examples:
      | dir          | a   | state      | reason        |
      | acceleration | 2.1 | MONITORING | NONE          |
      | acceleration | 2.2 | SAFE_STATE | REQUEST_LIMIT |
      | deceleration | 3.6 | MONITORING | NONE          |
      | deceleration | 3.7 | SAFE_STATE | REQUEST_LIMIT |

  @REQ-ACCM-003 @REQ-ACCM-009
  Scenario: A request above the limit is never applied and SAFE_STATE stays latched
    Given the ego vehicle drives at 100 km/h
    And the controller is ACTIVE without request
    When 0.2 s pass
    And the controller ramps the acceleration request to 2.5 m/s2 at 2.5 m/s3
    And 0.2 s pass
    Then the monitor state is SAFE_STATE
    And the permit is withdrawn
    And the applied acceleration never exceeded 2.1 m/s2
    And the actuator applies no request
    And the actuator requests a driver takeover
    When the controller is ACTIVE without request
    And 2 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason REQUEST_LIMIT
    And the permit is withdrawn

  @REQ-ACCM-004
  Scenario: Acceleration and deceleration requested together
    Given the ego vehicle drives at 100 km/h
    And the controller is ACTIVE without request
    When 0.2 s pass
    And the controller requests 0.05 m/s2 acceleration and 0.05 m/s2 deceleration
    And 0.1 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason ACCEL_AND_BRAKE
    And the applied acceleration never exceeded 0 m/s2
    And the applied deceleration never exceeded 0 m/s2

  @REQ-ACCM-005
  Scenario Outline: Requests are allowed only while the controller is ACTIVE
    Given the ego vehicle drives at 100 km/h
    And the controller is <s> without request
    When 0.2 s pass
    And the controller requests 0.05 m/s2 <dir> in state <s>
    And 0.1 s pass
    Then the monitor state is <state>
    And the monitor reports the fault reason <reason>

    Examples:
      | s        | dir          | state      | reason                 |
      | ACTIVE   | acceleration | MONITORING | NONE                   |
      | ACTIVE   | deceleration | MONITORING | NONE                   |
      | STANDBY  | acceleration | SAFE_STATE | REQUEST_OUTSIDE_ACTIVE |
      | OVERRIDE | deceleration | SAFE_STATE | REQUEST_OUTSIDE_ACTIVE |
      | FAULT    | deceleration | SAFE_STATE | REQUEST_OUTSIDE_ACTIVE |
      | OFF      | acceleration | SAFE_STATE | REQUEST_OUTSIDE_ACTIVE |

  @REQ-ACCM-006
  Scenario Outline: Request rate limits
    Given the ego vehicle drives at 100 km/h
    And the controller is ACTIVE without request
    When 0.2 s pass
    And the controller ramps the <dir> request to 1.0 m/s2 at <j> m/s3
    And 0.2 s pass
    Then the monitor state is <state>
    And the monitor reports the fault reason <reason>

    Examples:
      | dir          | j   | state      | reason       |
      | acceleration | 3.5 | MONITORING | NONE         |
      | acceleration | 4.0 | SAFE_STATE | REQUEST_RATE |
      | deceleration | 6.0 | MONITORING | NONE         |
      | deceleration | 6.5 | SAFE_STATE | REQUEST_RATE |

  @REQ-ACCM-006
  Scenario: The rate limit scales with the time since the last valid command
    Given the ego vehicle drives at 100 km/h
    And the controller is ACTIVE without request
    When 0.2 s pass
    And the next 2 command frames are lost
    And the controller ramps the acceleration request to 1.0 m/s2 at 3.5 m/s3
    And 0.2 s pass
    Then the monitor state is MONITORING
    And the monitor never entered SAFE_STATE

  @REQ-ACCM-007
  Scenario: Acceleration starting toward a close in-lane vehicle latches SAFE_STATE at once
    Given the ego vehicle drives at 60 km/h
    And a stationary vehicle stands 30 m ahead
    And the controller is ACTIVE without request
    When 0.2 s pass
    And the controller ramps the acceleration request to 0.5 m/s2 at 2.5 m/s3
    And 1 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason UNINTENDED_ACCEL
    And the monitor entered SAFE_STATE between 0.2 s and 0.24 s
    And the applied acceleration never exceeded 0 m/s2
    And the actuator requests a driver takeover

  @REQ-ACCM-007
  Scenario Outline: After a cut-in the acceleration must decay by at least 4 m/s3
    Given the ego vehicle drives at 60 km/h
    And a stationary vehicle stands 200 m ahead
    And the controller is ACTIVE without request
    When 0.2 s pass
    And the controller ramps the acceleration request to 1.0 m/s2 at 2.5 m/s3
    And a vehicle cuts in 30 m ahead at 20 km/h
    And the controller ramps the acceleration request to 0.0 m/s2 at <j> m/s3
    And 0.2 s pass
    Then the monitor state is <state>
    And the monitor reports the fault reason <reason>

    Examples:
      | j   | state      | reason           |
      | 5.0 | MONITORING | NONE             |
      | 3.0 | SAFE_STATE | UNINTENDED_ACCEL |

  @REQ-ACCM-007
  Scenario Outline: The decay is required from TTC 3 s on
    Given the ego vehicle drives at 60 km/h
    And a stationary vehicle stands 200 m ahead
    And the controller is ACTIVE without request
    When 0.2 s pass
    And the controller ramps the acceleration request to 0.3 m/s2 at 2.5 m/s3
    And a vehicle cuts in <d> m ahead at 20 km/h
    And the controller ramps the acceleration request to 0.0 m/s2 at 3.0 m/s3
    And 0.2 s pass
    Then the monitor state is <state>

    Examples:
      | d  | state      |
      | 32 | SAFE_STATE |
      | 36 | MONITORING |

  @REQ-ACCM-007
  Scenario Outline: Acceleration is allowed when no in-lane vehicle is closer than 3 s
    Given the ego vehicle drives at 60 km/h
    And a vehicle drives <d> m ahead at 0 km/h with <lat> m lateral offset
    And the controller is ACTIVE without request
    When 0.2 s pass
    And the controller ramps the acceleration request to 0.5 m/s2 at 2.5 m/s3
    And 1 s pass
    Then the monitor state is MONITORING
    And the monitor never entered SAFE_STATE

    Examples:
      | d   | lat |
      | 80  | 0.0 |
      | 30  | 2.0 |
      | 220 | 0.0 |

  @REQ-ACCM-007
  Scenario: Acceleration without a valid radar latches SAFE_STATE
    Given the ego vehicle drives at 60 km/h
    And the controller is ACTIVE without request
    When 0.2 s pass
    And the radar sender pauses for 50 frames
    And the controller ramps the acceleration request to 0.5 m/s2 at 2.5 m/s3
    And 1 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason UNINTENDED_ACCEL
