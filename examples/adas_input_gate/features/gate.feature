# Scenarios may only use steps from sil/steps/input_gate.yaml.
# Every scenario is tagged with the requirement(s) it verifies.
Feature: AdasInputGate interface gate for the ObjectSummary event

  @REQ-GATE-001 @REQ-GATE-002
  Scenario: First valid frame brings the gate into RUN with valid output
    Given a freshly started component
    Then the state is INIT
    When the component receives 1 valid frame
    Then the state is RUN
    And the output qualifier is VALID
    And the output signal nearest_distance equals 42.5 within 0.01
    And the output signal nearest_rel_speed equals -3.25 within 0.01

  @REQ-GATE-001
  Scenario: Corrupted frames do not start the gate
    Given a freshly started component
    When the component receives 2 frames with corrupted CRC
    Then the state is INIT
    And the output qualifier is INVALID

  @REQ-GATE-003
  Scenario: Three consecutive E2E errors degrade the gate
    Given the component has received 5 valid frames
    When the component receives 2 frames with corrupted CRC
    Then the state is RUN
    When the component receives 1 frame with corrupted CRC
    Then the state is DEGRADED
    And the output qualifier is INVALID

  @REQ-GATE-003
  Scenario: Repeated counters count as E2E errors
    Given the component has received 5 valid frames
    When the component receives 3 frames with repeated counter
    Then the state is DEGRADED

  @REQ-GATE-003
  Scenario: Non-consecutive E2E errors are tolerated
    Given the component has received 5 valid frames
    When the component receives 2 frames with corrupted CRC
    And the component receives 1 valid frame
    And the component receives 2 frames with corrupted CRC
    Then the state is RUN

  @REQ-GATE-004
  Scenario Outline: Out-of-range signal values are not forwarded
    Given the component has received 3 valid frames
    When the component receives a frame with <signal> set to <value>
    Then the state is DEGRADED
    And the output qualifier is INVALID
    And the output signal nearest_distance equals 42.5 within 0.01

    Examples:
      | signal             | value  |
      | object_count       | 65     |
      | nearest_distance   | 250.5  |
      | nearest_rel_speed  | -80.5  |
      | nearest_confidence | 100.5  |

  @REQ-GATE-004
  Scenario: Boundary values are accepted
    Given the component has received 1 valid frame
    When the component receives a frame with nearest_distance set to 250
    Then the state is RUN
    And the output signal nearest_distance equals 250 within 0.01
    When the component receives a frame with nearest_rel_speed set to -80
    Then the state is RUN

  @REQ-GATE-005
  Scenario: Five missing cycles in RUN lead to SAFE
    Given the component has received 2 valid frames
    When 4 cycles pass without a frame
    Then the state is RUN
    And the output qualifier is INVALID
    When 1 cycle passes without a frame
    Then the state is SAFE

  @REQ-GATE-005
  Scenario: No frame at all after start leads to SAFE
    Given a freshly started component
    When 5 cycles pass without a frame
    Then the state is SAFE

  @REQ-GATE-006
  Scenario: Ten consecutive valid frames recover from DEGRADED
    Given the component has received 3 valid frames
    And the component receives a frame with nearest_distance set to 300
    Then the state is DEGRADED
    When the component receives 9 valid frames
    Then the state is DEGRADED
    And the output qualifier is INVALID
    When the component receives 1 valid frame
    Then the state is RUN
    And the output qualifier is VALID

  @REQ-GATE-007
  Scenario: SAFE is latched
    Given a freshly started component
    When 5 cycles pass without a frame
    And the component receives 20 valid frames
    Then the state is SAFE
    And the output qualifier is INVALID

  @REQ-GATE-008
  Scenario: Status message reflects the gate every cycle
    Given the component has received 1 valid frame
    Then the status message reports state RUN
    And the status message signal Qualifier is 1
    And the status message signal NearestDistance is 42.5
    When the component receives 3 frames with corrupted CRC
    Then the status message reports state DEGRADED
    And the status message signal Qualifier is 0
    And the status message signal E2eErrorCount is 3

  @REQ-GATE-008 @REQ-GATE-009
  Scenario: Alive counter and heartbeat advance once per cycle
    Given a freshly started component
    When the component receives 9 valid frames
    Then the heartbeat count is 9
    And the status message signal AliveCounter is 1
