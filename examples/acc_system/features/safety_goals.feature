# System SIL: the generated AccController and AccMonitor on one bus, the actuator ECU model
# (AoU) applying only approved commands. Faults are injected on the controller's or monitor's
# output frames. "Fault detection budget" = FTTI of the safety goal minus the actuator's assumed
# reaction time (100 ms), resolved from examples/acc_system/model.yaml.
Feature: ACC safety goals under node faults

  @REQ-ACCS-001
  Scenario: An acceleration request above the limit is never applied
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When 2 s pass
    And the controller sends AccelRequest = 3.0 with a valid CRC for 100 frames
    And 2 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason REQUEST_LIMIT
    And the actuator entered its safe state within the fault detection budget of REQ-ACCS-001
    And the applied acceleration never exceeded 2.1 m/s2
    And the actual acceleration of the vehicle never exceeded 2.1 m/s2
    And the actuator requests a driver takeover

  @REQ-ACCS-001
  Scenario: An acceleration step within the limit but above the jerk envelope is never applied
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When 2 s pass
    And the controller sends AccelRequest = 1.0 with a valid CRC for 100 frames
    And 2 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason REQUEST_RATE
    And the actuator entered its safe state within the fault detection budget of REQ-ACCS-001
    And the applied acceleration never exceeded 0.1 m/s2

  @REQ-ACCS-002
  Scenario: A deceleration request above the limit is never applied
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When 2 s pass
    And the controller sends BrakeRequest = 6.0 with a valid CRC for 100 frames
    And 2 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason REQUEST_LIMIT
    And the actuator entered its safe state within the fault detection budget of REQ-ACCS-002
    And the applied deceleration never exceeded 3.6 m/s2
    And the actual deceleration of the vehicle never exceeded 3.6 m/s2

  @REQ-ACCS-003
  Scenario: Acceleration kept up while the controller brakes for a cut-in is caught
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When 2 s pass
    And the controller sends AccelRequest = 0.06 with a valid CRC for 2000 frames
    And a vehicle cuts in 25 m ahead at 70 km/h
    And 3 s pass
    Then the monitor state is SAFE_STATE
    And the actuator entered its safe state within the fault detection budget of REQ-ACCS-003
    And the applied acceleration never exceeded 0.06 m/s2
    And the actuator requests a driver takeover

  @REQ-ACCS-004 @REQ-ACCS-005
  Scenario Outline: A stopped or frozen controller while braking behind a slower vehicle
    Given the ego vehicle drives at 100 km/h
    And a lead vehicle drives 50 m ahead at 100 km/h
    And ACC is engaged
    When 3 s pass
    And the lead vehicle changes speed to 70 km/h at 1.5 m/s2
    And 2 s pass
    Then braking is requested
    When the command sender <f>
    And 2 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason COMMAND_TIMEOUT
    And the actuator entered its safe state within the fault detection budget of REQ-ACCS-004
    And the request was ramped out within 800 ms of the first fault
    And the actuator requests a driver takeover

    Examples:
      | f       |
      | stops   |
      | freezes |

  @REQ-ACCS-004
  Scenario: A controller reporting STANDBY while requesting braking is caught
    Given the ego vehicle drives at 100 km/h
    And a lead vehicle drives 50 m ahead at 100 km/h
    And ACC is engaged
    When 3 s pass
    And the lead vehicle changes speed to 70 km/h at 1.5 m/s2
    And 2 s pass
    And the controller sends AccStatus = 1 with a valid CRC for 100 frames
    And 1 s pass
    Then the monitor state is SAFE_STATE
    And the monitor reports the fault reason REQUEST_OUTSIDE_ACTIVE
    And the actuator entered its safe state within the fault detection budget of REQ-ACCS-004

  @REQ-ACCS-005
  Scenario: A lost monitor makes the actuator ramp out and request a takeover
    Given the ego vehicle drives at 100 km/h
    And a lead vehicle drives 50 m ahead at 100 km/h
    And ACC is engaged
    When 3 s pass
    And the lead vehicle changes speed to 70 km/h at 1.5 m/s2
    And 2 s pass
    And the monitor sender stops
    And 2 s pass
    Then the actuator requests a driver takeover
    And the actuator entered its safe state within 40 ms of the first fault
    And the actuator applies no request

  @REQ-ACCS-006
  Scenario: Two corrupted command frames neither stop ACC nor trigger the monitor
    Given the ego vehicle drives at 100 km/h
    And a lead vehicle drives 50 m ahead at 100 km/h
    And ACC is engaged
    When 3 s pass
    And the next 2 command frames are corrupted
    And 2 s pass
    Then the monitor never entered SAFE_STATE
    And the ACC state is ACTIVE
    And the actuator does not request a driver takeover
