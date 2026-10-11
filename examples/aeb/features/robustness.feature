Feature: AEB relevance, speed range and input supervision

  @REQ-AEB-008
  Scenario Outline: Only vehicles within 1.8 m of the lane centre are relevant
    Given the ego vehicle drives at 50 km/h
    And a vehicle drives 60 m ahead at 0 km/h with <offset> m lateral offset
    When 4 s pass
    Then <result>

    Examples:
      | offset | result                                                                   |
      | 2.0    | no forward collision warning was issued                                  |
      | 1.6    | a forward collision warning was issued at a time to collision between 2.7 s and 2.9 s |

  @REQ-AEB-008
  Scenario: A vehicle followed at the same speed is not a hazard
    Given the ego vehicle drives at 80 km/h
    And a lead vehicle drives 10 m ahead at 80 km/h
    When 10 s pass
    Then no forward collision warning was issued
    And emergency braking was never requested

  @REQ-AEB-009
  Scenario Outline: Warnings start only between 10 km/h and 150 km/h
    Given the ego vehicle drives at <speed> km/h
    And a stationary vehicle stands <distance> m ahead
    When <time> s pass
    Then <result>

    Examples:
      | speed | distance | time | result                                                               |
      | 9     | 20       | 5    | no forward collision warning was issued                              |
      | 11    | 20       | 5    | a forward collision warning was issued at a time to collision between 2.7 s and 2.9 s |
      | 145   | 140      | 2    | a forward collision warning was issued at a time to collision between 2.7 s and 2.9 s |
      | 155   | 140      | 2    | no forward collision warning was issued                              |

  @REQ-AEB-013
  Scenario: A paused radar leads to FAULT without intervention, then recovers
    Given the ego vehicle drives at 50 km/h
    And a stationary vehicle stands 100 m ahead
    When 1 s passes
    And the radar sender pauses for 5 frames
    And 0.16 s pass
    Then the AEB state is FAULT
    And the warning is off
    And emergency braking was never requested
    When 0.2 s pass
    Then the AEB state is STANDBY

  @REQ-AEB-013
  Scenario Outline: Vehicle input timeouts lead to FAULT
    Given the ego vehicle drives at 50 km/h
    When 1 s passes
    And the <message> sender pauses for <n> frames
    And 0.06 s pass
    Then the AEB state is <state>

    Examples:
      | message      | n | state   |
      | ego state    | 2 | STANDBY |
      | ego state    | 3 | FAULT   |
      | driver input | 2 | STANDBY |
      | driver input | 3 | FAULT   |

  @REQ-AEB-008
  Scenario: A vehicle beyond 150 m is not relevant
    Given the ego vehicle drives at 100 km/h
    And a stationary vehicle stands 170 m ahead
    When 0.5 s pass
    Then no forward collision warning was issued
