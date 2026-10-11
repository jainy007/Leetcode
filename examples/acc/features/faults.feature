Feature: ACC input supervision

  @REQ-ACC-021
  Scenario Outline: Lost radar frames below the timeout are tolerated
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When the next <n> radar frames are <fault>
    And 1 s passes
    Then the ACC state is ACTIVE

    Examples:
      | n | fault     |
      | 2 | lost      |
      | 2 | corrupted |

  @REQ-ACC-021 @REQ-ACC-019
  Scenario Outline: Radar frames missing for 120 ms lead to FAULT
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When the next <n> radar frames are <fault>
    And 1 s passes
    Then the ACC state is FAULT
    And neither acceleration nor braking is requested

    Examples:
      | n | fault     |
      | 3 | lost      |
      | 3 | corrupted |

  @REQ-ACC-021
  Scenario Outline: Vehicle frames missing for 60 ms lead to FAULT
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When the next <n> <message> frames are lost
    And 1 s passes
    Then the ACC state is <state>

    Examples:
      | n | message      | state  |
      | 2 | ego state    | ACTIVE |
      | 3 | ego state    | FAULT  |
      | 2 | driver input | ACTIVE |
      | 3 | driver input | FAULT  |

  @REQ-ACC-021
  Scenario Outline: A paused sender is detected exactly at the timeout
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When the <message> sender pauses for <n> frames
    And 1 s passes
    Then the ACC state is <state>

    Examples:
      | message      | n | state  |
      | radar        | 2 | ACTIVE |
      | radar        | 3 | FAULT  |
      | ego state    | 2 | ACTIVE |
      | ego state    | 3 | FAULT  |
      | driver input | 2 | ACTIVE |
      | driver input | 3 | FAULT  |

  @REQ-ACC-022
  Scenario: FAULT is latched until the main switch is turned off
    Given the ego vehicle drives at 100 km/h
    And ACC is engaged
    When the next 5 radar frames are lost
    And 2 s pass
    And the driver presses the set button
    Then the ACC state is FAULT
    When the driver switches ACC off
    Then the ACC state is OFF
    When the driver switches ACC on
    And the driver presses the set button
    Then the ACC state is ACTIVE
