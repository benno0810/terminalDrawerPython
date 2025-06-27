    Feature: basic checking commands length and type are valid

    Scenario Outline: Input invalid command
    Given the program is started
    When the user enters the command "C 3 3"
    When the user enters the command "<command>"
    Then the output should be an error message "<error>"
    Examples:
        | command            | error                                                        |
        | K -1 -1 2 2 x      | Invalid command type must be in [C,L,R,B,Q].        |
        | L 2 2 2 2 x 2       |invalid literal for int() with base 10: 'x'        |
        | L 2 2 5 x x        | Invalid command format for line creation. example: L x1 y1 x2 y2 color         |
        | R 0 0 6 6 x 2       | Invalid command format for rectangle creation. example: R x1 y1 x2 y2 color            |
        | C 0 0 6         | Canvas command must be: C width height            |
        
    Scenario: Try to draw polygon before creating canvas
    Given the program is started
    When the user enters the command "L 2 2 2 2 X"
    Then the output should be an error message "You must create a canvas first (C x y)."