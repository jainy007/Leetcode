Feature: ACC longitudinal control

  @REQ-ACC-010 @REQ-ACC-014 @REQ-ACC-015 @REQ-ACC-017 @REQ-ACC-018
  Scenario: Accelerate to a higher set speed within the comfort limits
    Given the ego vehicle drives at 80 km/h
    And ACC is engaged
    When the driver presses the speed up button 8 times
    And 40 s pass
    Then the set speed is 120 km/h
    And the ego speed is 120 km/h within 1 km/h
    And the requested acceleration never exceeded 2.0 m/s2
    And the requested jerk never exceeded 2.5 m/s3 increasing and 5.0 m/s3 decreasing
    And acceleration and braking were never requested together
    And the steering request was always 0 deg

  @REQ-ACC-011 @REQ-ACC-012
  Scenario Outline: Follow a slower vehicle at the selected time gap
    Given the ego vehicle drives at 120 km/h
    And the time gap setting is <setting>
    And a lead vehicle drives 80 m ahead at 100 km/h
    And ACC is engaged
    When 60 s pass
    Then the distance to the lead vehicle is <distance> m within 2 m
    And the ego speed is 100 km/h within 1 km/h
    And the reported time gap setting is <setting>
    And the ego speed never exceeded 120 km/h

    Examples:
      | setting | distance |
      | 1       | 32.8     |
      | 2       | 46.7     |
      | 3       | 60.6     |

  @REQ-ACC-011 @REQ-ACC-010
  Scenario: A faster lead vehicle does not pull the ego vehicle above the set speed
    Given the ego vehicle drives at 100 km/h
    And a lead vehicle drives 50 m ahead at 130 km/h
    And ACC is engaged
    When 30 s pass
    Then the ego speed is 100 km/h within 1 km/h
    And the ego speed never exceeded 101 km/h

  @REQ-ACC-013
  Scenario: A vehicle in the adjacent lane is ignored
    Given the ego vehicle drives at 120 km/h
    And a vehicle drives 50 m ahead at 80 km/h with 1.9 m lateral offset
    And ACC is engaged
    When 20 s pass
    Then the ego speed is 120 km/h within 1 km/h
    And braking was never requested

  @REQ-ACC-013
  Scenario: A vehicle just inside the lane boundary is followed
    Given the ego vehicle drives at 120 km/h
    And a vehicle drives 60 m ahead at 80 km/h with 1.7 m lateral offset
    And ACC is engaged
    When 2 s pass
    Then braking is requested

  @REQ-ACC-013
  Scenario: A vehicle beyond 200 m is not a target even when closing fast
    Given the ego vehicle drives at 180 km/h
    And a lead vehicle drives 245 m ahead at 0 km/h
    And ACC is engaged
    When 0.4 s pass
    Then braking was never requested
    And the distance to the lead vehicle is 205 m within 2 m

  @REQ-ACC-013
  Scenario: A vehicle just inside 200 m is a target
    Given the ego vehicle drives at 180 km/h
    And a lead vehicle drives 215 m ahead at 0 km/h
    And ACC is engaged
    When 0.2 s pass
    Then braking is requested
    And the distance to the lead vehicle is 185 m within 2 m

  @REQ-ACC-016 @REQ-ACC-014 @REQ-ACC-017
  Scenario Outline: The lead vehicle brakes while being followed
    Given the ego vehicle drives at 100 km/h
    And a lead vehicle drives 46.7 m ahead at 100 km/h
    And ACC is engaged
    When the lead vehicle changes speed to <target> km/h at <decel> m/s2
    And 30 s pass
    Then the distance to the lead vehicle never fell below 5 m
    And no collision occurred
    And the requested deceleration never exceeded 3.5 m/s2
    And acceleration and braking were never requested together

    Examples:
      | target | decel |
      | 50     | 2     |
      | 50     | 3     |
      | 30     | 3     |

  @REQ-ACC-016 @REQ-ACC-014 @REQ-ACC-015
  Scenario: A slower vehicle cuts in 25 m ahead
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When a vehicle cuts in 25 m ahead at 80 km/h
    And 30 s pass
    Then the distance to the lead vehicle never fell below 5 m
    And the requested deceleration never exceeded 3.5 m/s2
    And the requested jerk never exceeded 2.5 m/s3 increasing and 5.0 m/s3 decreasing
    And the distance to the lead vehicle is 38.3 m within 2 m

  @REQ-ACC-011
  Scenario: The lead vehicle leaves and the ego vehicle resumes the set speed
    Given the ego vehicle drives at 120 km/h
    And a lead vehicle drives 46.7 m ahead at 100 km/h
    And ACC is engaged
    When 10 s pass
    And the lead vehicle leaves the lane
    And 40 s pass
    Then the ego speed is 120 km/h within 1 km/h

  @REQ-ACC-020 @REQ-ACC-018
  Scenario: Every command frame is E2E protected and sent every cycle
    Given the ego vehicle drives at 100 km/h
    And a lead vehicle drives 60 m ahead at 90 km/h
    And ACC is engaged
    When 10 s pass
    Then every command frame passed the E2E check
    And one command frame was sent per 20 ms
    And the steering request was always 0 deg
