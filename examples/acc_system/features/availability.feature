# Fault-free ACC scenarios with the monitor and the actuator gate in the loop: same expected
# values as examples/acc (from the ACC requirements), plus no intervention of the monitor.
Feature: ACC availability with the safety monitor in the loop

  @REQ-ACCS-006
  Scenario: Accelerate to a higher set speed
    Given the ego vehicle drives at 80 km/h
    And ACC is engaged
    When the driver presses the speed up button 8 times
    And 40 s pass
    Then the ego speed is 120 km/h within 1 km/h
    And the monitor never entered SAFE_STATE
    And the actuator applied every command

  @REQ-ACCS-006
  Scenario: Follow a slower vehicle at the default time gap
    Given the ego vehicle drives at 120 km/h
    And a lead vehicle drives 80 m ahead at 100 km/h
    And ACC is engaged
    When 60 s pass
    Then the distance to the lead vehicle is 46.7 m within 2 m
    And the monitor never entered SAFE_STATE
    And the actuator applied every command

  @REQ-ACCS-006
  Scenario: Brake for a vehicle cutting in
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When 2 s pass
    And a vehicle cuts in 25 m ahead at 70 km/h
    And 20 s pass
    Then no collision occurred
    And the monitor never entered SAFE_STATE
    And the actuator applied every command

  @REQ-ACCS-006
  Scenario: Follow a lead vehicle braking to a lower speed
    Given the ego vehicle drives at 100 km/h
    And a lead vehicle drives 50 m ahead at 100 km/h
    And ACC is engaged
    When 3 s pass
    And the lead vehicle changes speed to 40 km/h at 3 m/s2
    And 30 s pass
    Then no collision occurred
    And the ego speed is 40 km/h within 1 km/h
    And the monitor never entered SAFE_STATE
    And the actuator applied every command

  @REQ-ACCS-006
  Scenario: Driver override, cancel and resume
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When the driver presses the accelerator pedal to 30 %
    And 2 s pass
    And the driver releases the accelerator pedal
    And 5 s pass
    And the driver presses the brake pedal
    And the driver releases the brake pedal
    And the driver presses the resume button
    And 10 s pass
    Then the ACC state is ACTIVE
    And the monitor never entered SAFE_STATE
    And the actuator applied every command
