 Feature: drawing rectangles and lines which are overlap to each other


Scenario: Color fill on background affects nothing
  Given the program is started
  Given the user enters the command "C 7 5"
  And the user enters the command "R 1 1 5 4 a"
  And the user enters the command "R 3 2 6 4 b"
  When the user enters the command "B 0 0 y"
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


 Scenario: Color fill on overlapping rectangles affects only the topmost shape
  Given the program is started
  Given the user enters the command "C 7 5"
  And the user enters the command "R 1 1 5 4 a"
  And the user enters the command "R 3 2 6 4 b"
  When the user enters the command "B 4 3 z"
  Then the output should be:
    """
    ---------
    |       |
    | aaaaa |
    | a bbbb|
    | a bzzb|
    | aabbbb|
    ---------
    """



 Scenario: Color fill on overlapping rectangles affects only the topmost shape - when we fill rect-a, only part overlap between rect-a and rect-b in rect-b will be filled with "z"
  Given the program is started
  Given the user enters the command "C 8 6"
  And the user enters the command "R 1 1 5 4 a"
  And the user enters the command "R 3 2 7 5 b"
  When the user enters the command "B 2 2 z"
  Then the output should be:
    """
    ----------
    |        |
    | aaaaa  |
    | azbbbbb|
    | azbza b|
    | aabaa b|
    |   bbbbb|
    ----------
    """


Scenario: Fill a line on top of a rectangle only changes the line's color
  Given the program is started
  And the user enters the command "C 7 5"
  And the user enters the command "R 1 1 5 4 r"
  And the user enters the command "L 0 2 6 2 l"
  When the user enters the command "B 3 2 z"
  Then the output should be:
    """
    ---------
    |       |
    | rrrrr |
    |zzzzzzz|
    | r   r |
    | rrrrr |
    ---------
    """

Scenario: Fill a rectangle on top of a line only changes the rectangle's fill
  Given the program is started
  And the user enters the command "C 7 5"
  And the user enters the command "L 0 2 6 2 l"
  And the user enters the command "R 1 1 5 4 r"
  When the user enters the command "B 3 3 q"
  Then the output should be:
    """
    ---------
    |       |
    | rrrrr |
    |lrqqqrl|
    | rqqqr |
    | rrrrr |
    ---------
    """