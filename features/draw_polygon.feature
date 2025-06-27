Feature: drawing rectangles and lines

    
    Scenario: Create and print a canvas
    Given the program is started
    When the user enters the command "C 3 3"
    Then the output should be:
        """
        -----
        |   |
        |   |
        |   |
        -----
        """

    Scenario: Draw a horizontal line
    Given the program is started
    Given the user enters the command "C 3 3"
    When the user enters the command "L 1 1 2 1 ."
    Then the output should be:
        """
        -----
        |   |
        | ..|
        |   |
        -----
        """

    Scenario: Draw a vertical line
    Given the program is started
    Given the user enters the command "C 4 4"
    When the user enters the command "L 0 0 0 3 *"
    Then the output should be:
        """
        ------
        |*   |
        |*   |
        |*   |
        |*   |
        ------
        """

    Scenario: Draw a diagonal
    Given the program is started
    Given the user enters the command "C 5 5"
    When the user enters the command "L 1 1 3 3 ."
    Then the output should be:
    """
    -------
    |     |
    | .   |
    |  .  |
    |   . |
    |     |
    -------
    """

    Scenario: Draw a rectangle
    Given the program is started
    Given the user enters the command "C 5 5"
    When the user enters the command "R 1 1 3 3 x"
    Then the output should be:
        """
        -------
        |     |
        | xxx |
        | x x |
        | xxx |
        |     |
        -------
        """
    Scenario Outline: Draw lines and rectangles with error parameters
    Given the program is started
    Given the user enters the command "C <width> <height>"
    When the user enters the command "<command>"
    Then the output should be an error message "<error>"
    Examples:
        | width | height | command            | error                                                        |
        | 3     | 3      | L -1 -1 2 2 x      | Invalid line. Coordinates exceed canvas size.        |
        | 3     | 3      | L 2 2 2 2 x        | Invalid line. Start and end points cannot be the same.        |
        | 3     | 3      | L 2 2 5 5 x        | Invalid line. Coordinates exceed canvas size.          |
        | 5     | 5      | R 0 0 6 6 x        | Invalid rectangle. Coordinates exceed canvas size.            |
        | 5     | 5      | R 0 0 6 6 xx        | Color must be a single character.            |
        | 5     | 5      | L 0 0 6 6 xx        | Color must be a single character.            |
    


    
