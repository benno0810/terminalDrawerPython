    Feature: drawing rectangles and lines with overlap

    Scenario: Complex overlap and z-index (rectangles draw later oberwrite rectangles draw earlier)
    Given the program is started
    Given the user enters the command "C 7 5"
    And the user enters the command "R 1 1 5 4 a"
    And the user enters the command "R 3 2 6 4 b"
    Then the output should be:
    """
    ---------
    |       |
    | aaaaa |
    | a bbbb|
    | a b ab|
    | aabbbb|
    ---------
    """

    Scenario: Complex overlap and z-index (line over rectangle)
    Given the program is started
    Given the user enters the command "C 6 4"
    And the user enters the command "R 0 0 5 3 x"
    And the user enters the command "L 0 1 5 1 o"
    Then the output should be:
    """
    --------
    |xxxxxx|
    |oooooo|
    |x    x|
    |xxxxxx|
    --------
    """

