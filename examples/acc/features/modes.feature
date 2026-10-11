# Closed-loop SIL scenarios: the plant (sil/plant) simulates ego vehicle, lead vehicle, radar,
# driver and CAN bus. Steps come from sil/steps/can_function.yaml and sil/steps/features/acc.yaml only.
Feature: ACC modes and driver interaction

  @REQ-ACC-001 @REQ-ACC-019
  Scenario: Main switch on enters STANDBY without requesting acceleration or braking
    Given the ego vehicle drives at 100 km/h
    When the driver switches ACC on
    Then the ACC state is STANDBY
    And neither acceleration nor braking is requested

  @REQ-ACC-002 @REQ-ACC-020
  Scenario: Set engages at the current speed rounded to the nearest km/h
    Given the ego vehicle drives at 87.4 km/h
    And the driver switches ACC on
    When the driver presses the set button
    Then the ACC state is ACTIVE
    And the set speed is 87 km/h
    And the reported time gap setting is 2

  @REQ-ACC-002
  Scenario: Set is ignored while the brake pedal is pressed
    Given the ego vehicle drives at 100 km/h
    And the driver switches ACC on
    And the driver presses the brake pedal
    When the driver presses the set button
    Then the ACC state is STANDBY

  @REQ-ACC-002
  Scenario: Releasing the brake while Set is still held does not engage
    Given the ego vehicle drives at 100 km/h
    And the driver switches ACC on
    And the driver presses the brake pedal
    When the driver releases the brake pedal while holding the set button
    Then the ACC state is STANDBY

  @REQ-ACC-003
  Scenario Outline: Engagement is refused below 30 km/h
    Given the ego vehicle drives at 60 km/h
    And ACC is engaged
    And the driver presses the cancel button
    And the ego vehicle drives at 29 km/h
    When the driver presses the <button> button
    Then the ACC state is STANDBY

    Examples:
      | button |
      | set    |
      | resume |

  @REQ-ACC-004 @REQ-ACC-006
  Scenario: Cancel keeps the set speed and Resume restores it
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When the driver presses the cancel button
    Then the ACC state is STANDBY
    And the set speed is 100 km/h
    When the ego vehicle drives at 80 km/h
    And the driver presses the resume button
    Then the ACC state is ACTIVE
    And the set speed is 100 km/h

  @REQ-ACC-005
  Scenario: Speed Up and Speed Down change the set speed in 5 km/h steps
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When the driver presses the speed up button
    Then the set speed is 105 km/h
    When the driver presses the speed down button
    And the driver presses the speed down button
    Then the set speed is 95 km/h

  @REQ-ACC-005
  Scenario Outline: The set speed stays within 30 km/h to 180 km/h
    Given the ego vehicle drives at <speed> km/h
    And ACC is engaged
    When the driver presses the <button> button
    Then the set speed is <result> km/h

    Examples:
      | speed | button     | result |
      | 178   | speed up   | 180    |
      | 32    | speed down | 30     |

  @REQ-ACC-006 @REQ-ACC-019
  Scenario: The brake pedal disengages within one cycle
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When the driver presses the brake pedal
    Then the ACC state is STANDBY
    And the ACC state changed within 20 ms of the driver action
    And the set speed is 100 km/h
    And neither acceleration nor braking is requested

  @REQ-ACC-007
  Scenario: Accelerator pedal override suspends control
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When the driver presses the accelerator pedal to 30 %
    Then the ACC state is OVERRIDE
    And neither acceleration nor braking is requested
    When 2 s pass
    And the driver releases the accelerator pedal
    Then the ACC state is ACTIVE

  @REQ-ACC-006
  Scenario: The brake pedal also ends OVERRIDE
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When the driver presses the accelerator pedal to 30 %
    Then the ACC state is OVERRIDE
    When the driver presses the brake pedal
    Then the ACC state is STANDBY

  @REQ-ACC-007
  Scenario: A light touch of the accelerator pedal does not override
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When the driver presses the accelerator pedal to 4 %
    Then the ACC state is ACTIVE

  @REQ-ACC-008 @REQ-ACC-019
  Scenario: Main switch off clears the set speed
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When the driver switches ACC off
    Then the ACC state is OFF
    And the set speed is 0 km/h
    And neither acceleration nor braking is requested
    When the driver switches ACC on
    And the driver presses the resume button
    Then the ACC state is STANDBY

  @REQ-ACC-009
  Scenario Outline: ACC hands over below 25 km/h and only then
    Given the ego vehicle drives at 60 km/h
    And a lead vehicle drives 30 m ahead at 60 km/h
    And ACC is engaged
    When the lead vehicle changes speed to <lead> km/h at 2 m/s2
    And 40 s pass
    Then the ACC state is <state>

    Examples:
      | lead | state   |
      | 22   | STANDBY |
      | 27   | ACTIVE  |
