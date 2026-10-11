# Component SIL of the AEB safety monitor. A stimulus node stands in for the AEB controller; the
# actuator ECU model (AoU) applies only approved commands. Thresholds from the requirements:
# braking onset TTC <= 2.1 s, full braking onset TTC <= 1.5 s, lane +-2.1 m, limits 8.1 / 4.1 m/s2.
# At 50 km/h (13.9 m/s) TTC 2.1 s is 29.2 m and TTC 1.5 s is 20.8 m; the ego vehicle covers
# 1.4 m per 0.1 s before the command is issued.
Feature: AEB monitor braking justification

  @REQ-AEBM-001 @REQ-AEBM-002 @REQ-AEBM-010
  Scenario: A justified braking cascade is approved until the vehicle has stopped
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 22 m ahead
    And the AEB controller is in STANDBY with 0 m/s2 braking
    When 0.1 s pass
    Then the monitor state is MONITORING
    And the permit is granted
    When the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking
    And 0.1 s pass
    Then the actuator applies -4.0 m/s2
    When the AEB controller is in FULL_BRAKE with 8 m/s2 braking
    And 3 s pass
    Then the monitor never entered SAFE_STATE
    And the actuator applied every command
    And the actuator applies -8.0 m/s2
    And no collision occurred
    And the ego vehicle has stopped
    And every monitor frame passed the E2E check
    And one monitor frame was sent per 20 ms

  @REQ-AEBM-005
  Scenario Outline: Braking onset needs an in-lane target within TTC 2.1 s
    Given the ego vehicle drives at 50 km/h
    And a vehicle drives <d> m ahead at 0 km/h with <lat> m lateral offset
    And the AEB controller is in STANDBY with 0 m/s2 braking
    When 0.1 s pass
    And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking
    And 0.1 s pass
    Then the monitor state is <state>
    And the monitor reports the fault reason <reason>

    Examples:
      | d  | lat | state      | reason              |
      | 27 | 0.0 | MONITORING | NONE                |
      | 32 | 0.0 | SAFE_STATE | UNJUSTIFIED_BRAKING |
      | 27 | 2.0 | MONITORING | NONE                |
      | 27 | 2.2 | SAFE_STATE | UNJUSTIFIED_BRAKING |

  @REQ-AEBM-005 @REQ-AEBM-009
  Scenario: Braking without any target is never applied
    Given the ego vehicle drives at 50 km/h
    And the AEB controller is in STANDBY with 0 m/s2 braking
    When 0.1 s pass
    And the AEB controller is in FULL_BRAKE with 8 m/s2 braking
    And 0.5 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason UNJUSTIFIED_BRAKING
    And the permit is withdrawn
    And the applied deceleration never exceeded 0 m/s2
    And the actual deceleration of the vehicle never exceeded 0.1 m/s2
    And the actuator requests a driver takeover

  @REQ-AEBM-005
  Scenario: A target receding is no reason to brake
    Given the ego vehicle drives at 50 km/h
    And a lead vehicle drives 15 m ahead at 60 km/h
    And the AEB controller is in STANDBY with 0 m/s2 braking
    When 0.1 s pass
    And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking
    And 0.1 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason UNJUSTIFIED_BRAKING

  @REQ-AEBM-006
  Scenario Outline: Full braking onset needs TTC 1.5 s
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands <d> m ahead
    And the AEB controller is in STANDBY with 0 m/s2 braking
    When 0.1 s pass
    And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking
    And 0.1 s pass
    And the AEB controller is in FULL_BRAKE with 8 m/s2 braking
    And 0.1 s pass
    Then the monitor state is <state>
    And the monitor reports the fault reason <reason>

    Examples:
      | d  | state      | reason                   |
      | 20 | MONITORING | NONE                     |
      | 25 | SAFE_STATE | UNJUSTIFIED_FULL_BRAKING |

  @REQ-AEBM-003
  Scenario Outline: Deceleration limits per braking state
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 15 m ahead
    And the AEB controller is in STANDBY with 0 m/s2 braking
    When 0.1 s pass
    And the AEB controller is in <s> with <a> m/s2 braking
    And 0.1 s pass
    Then the monitor state is <state>
    And the monitor reports the fault reason <reason>

    Examples:
      | s             | a   | state      | reason        |
      | FULL_BRAKE    | 8.1 | MONITORING | NONE          |
      | FULL_BRAKE    | 8.2 | SAFE_STATE | REQUEST_LIMIT |
      | PARTIAL_BRAKE | 4.1 | MONITORING | NONE          |
      | PARTIAL_BRAKE | 4.2 | SAFE_STATE | REQUEST_LIMIT |

  @REQ-AEBM-004
  Scenario Outline: Braking is allowed only in a braking state
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 15 m ahead
    And the AEB controller is in STANDBY with 0 m/s2 braking
    When 0.1 s pass
    And the AEB controller is in <s> with 4 m/s2 braking
    And 0.1 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason REQUEST_OUTSIDE_BRAKING

    Examples:
      | s       |
      | STANDBY |
      | WARNING |
      | FAULT   |

  @REQ-AEBM-007
  Scenario: Braking that continues after the target left the lane latches SAFE_STATE
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 22 m ahead
    And the AEB controller is in STANDBY with 0 m/s2 braking
    When 0.1 s pass
    And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking
    And 0.1 s pass
    And the lead vehicle leaves the lane
    And 0.2 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason UNJUSTIFIED_BRAKING
    And the actuator requests a driver takeover

  @REQ-AEBM-005
  Scenario: A new braking onset is justified again after braking ended
    Given the ego vehicle drives at 50 km/h
    And a lead vehicle drives 15 m ahead at 20 km/h
    And the AEB controller is in STANDBY with 0 m/s2 braking
    When 0.1 s pass
    And the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking
    And 0.1 s pass
    And the AEB controller is in STANDBY with 0 m/s2 braking
    And the lead vehicle changes speed to 80 km/h at 5 m/s2
    And 3 s pass
    Then the monitor state is MONITORING
    When the AEB controller is in PARTIAL_BRAKE with 4 m/s2 braking
    And 0.1 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason UNJUSTIFIED_BRAKING
