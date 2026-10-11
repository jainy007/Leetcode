Feature: AEB collision avoidance and mitigation performance

  @REQ-AEB-010
  Scenario Outline: A stationary vehicle is avoided from 20 km/h to 60 km/h
    Given the ego vehicle drives at <speed> km/h
    And a stationary vehicle stands 100 m ahead
    When 20 s pass
    Then no collision occurred
    And the ego vehicle has stopped

    Examples:
      | speed |
      | 20    |
      | 40    |
      | 60    |

  @REQ-AEB-011
  Scenario: The impact speed on a stationary vehicle at 80 km/h is at most 40 km/h
    Given the ego vehicle drives at 80 km/h
    And a stationary vehicle stands 100 m ahead
    When 10 s pass
    Then the impact speed was at most 40 km/h

  @REQ-AEB-012
  Scenario Outline: A vehicle at 20 km/h is avoided from up to 80 km/h
    Given the ego vehicle drives at <speed> km/h
    And a lead vehicle drives 100 m ahead at 20 km/h
    When 20 s pass
    Then no collision occurred

    Examples:
      | speed |
      | 50    |
      | 80    |
