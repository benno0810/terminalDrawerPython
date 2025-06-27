    Feature: color filling simple shapes

        Scenario: Fill outside the canvas should raise error
        Given the program is started
        And the user enters the command "C 5 5"
        When the user enters the command "R 1 1 3 3 x"
        When the user enters the command "B 5 5 o"
        Then the output should be an error message "Coordinates exceed canvas size."


        Scenario: Bucket fill inside a rectangle
        Given the program is started
        Given the user enters the command "C 5 5"
        When the user enters the command "R 1 1 3 3 x"
        When the user enters the command "B 2 2 ."
        Then the output should be:
            """
            -------
            |     |
            | xxx |
            | x.x |
            | xxx |
            |     |
            -------
            """


    
        Scenario: Bucket fill on the boarder of rectangle (nothing should happen)
        Given the program is started
        Given the user enters the command "C 5 5"
        When the user enters the command "R 1 1 3 3 x"
        When the user enters the command "B 1 1 ."
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
        
        Scenario: Bucket fill on the empty canvas (nothing should happen)
        Given the program is started
        Given the user enters the command "C 5 5"
        When the user enters the command "R 1 1 3 3 x"
        When the user enters the command "B 0 0 ."
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

        Scenario: Bucket fill on the line (line color will be changed)
        Given the program is started
        Given the user enters the command "C 5 5"
        When the user enters the command "L 1 1 3 3 x"
        When the user enters the command "B 2 2 ."
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