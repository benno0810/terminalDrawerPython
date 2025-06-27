# terminalDrawerPython

draw on terminal with python

# Requirement

```bash
You are to implement a terminal-based drawing program
```

# Example command sequence and result
```text
C 5 10
-------
|     |
|     |
|     |
|     |
|     |
|     |
|     |
|     |
|     |
|     |
-------

L 0 0 0 5 x
-------
|x    |
|x    |
|x    |
|x    |
|x    |
|x    |
|     |
|     |
|     |
|     |
-------

R 0 0 3 3 y
-------
|yyyy |
|y  y |
|y  y |
|yyyy |
|x    |
|x    |
|     |
|     |
|     |
|     |
-------

R 2 2 4 4 z
-------
|yyyy |
|y  y |
|y zzz|
|yyzyz|
|x zzz|
|x    |
|     |
|     |
|     |
|     |
-------

B 3 3 z 
-------
|yyyy |
|y  y |
|y zzz|
|yyzzz|
|x zzz|
|x    |
|     |
|     |
|     |
|     |
-------

B 0 1 y 
-------
|yyyy |
|y  y |
|y zzz|
|yyzzz|
|y zzz|
|y    |
|     |
|     |
|     |
|     |
------- （nothing happened）

L 0 6 4 9 d 
-------
|yyyy |
|y  y |
|y zzz|
|yyzzz|
|y zzz|
|y    |
|d    |
| dd  |
|   d |
|    d|
------- (diagonal line, bresenham line;s algo from googling )

B 0 6 c
-------
|yyyy |
|y  y |
|y zzz|
|yyzzz|
|y zzz|
|y    |
|c    |
| cc  |
|   c |
|    c|
-------

Q
Quitting. (then application quit)
```

## Acceptance criteria

- The program will prompt for input, and render the response to the screen, before prompting for more input, up to the point it is terminated with “Q”.
- A readme explaining how to run the code
- Adequate test coverage. We recommend a test-driven approach
- Where requirements are unclear, make a choice and document if necessary


# How to test

```bash
pip install behave # (or pip install -r requirements.txt)
behave # if you want to test all feature file
behave features/command_basic_check.feature # if you want to test a single feature file
```

# How to run
```bash
python draw.py
```




# Miscellaneous
## Hypotheses / Assumptions
- consult scenarios (.feature files) under folder features if you find any confliction and then raise an issue about this confliction

- (0,0) is at the left top of the canvas,  larger x means more close to right, larger y means more close to bottom, all coordinates can not be negative

- you must create a canvas board before drawing anything

- all coordinates cannot exceed the canvas, otherwise an error message will throw but application continue to run

- for invalid command input, and error message will throw out but application continue to run

- virtual z axis for rectangles and lines objects (I learnt it from computer graphic, it's called z-buffer)
  - only colors of largest z (min depth, cloest to the "camera") will be rendered on canvas
  - we maintain coordinate z in-house, i.e when a command pass in, an increasing z coordinate is generated for any object that is being created
  - theoretically two polygons will never be overlapping with each other given increasing z coordinates, but it can be confused to look at what's being printing out since we can only look through z axis

- Any command that specifies coordinates outside the current canvas dimensions  
  - returns an error message:  
  `"Invalid coordinates. Coordinates exceed canvas size."`

- Any command that specifies negative coordinates  
  - returns an error message:  
  `"Invalid coordinates. Coordinates must be non-negative."`

- Zero-length lines or rectangles (start == end)  
  - return an error message:  
  `"Invalid line. Start and end points cannot be the same."`  
  or  
  `"Invalid rectangle. Width and height must be > 0."`

- Bucket-fill (`B x1 y1 c`):

  - If `(x1,y1)` is on a standalone line → only that line segment is filled.
  - If `(x1,y1)` is inside a rectangle → only the **interior** is filled (borders remain).
  - If `(x1,y1)` is on the **border** of a rectangle → no fill is performed.
  - If a line crosses a rectangle and `(x1,y1)` is on that line → only the line is filled, interior of the rectangle stays unchanged.
  - More scenarios are covered in features/color_filling_overlap.feature

- Rectangle drawing (`R x1 y1 x2 y2 c`) should always draw borders only; the inside remains blank until a bucket-fill command is applied.

## Why I use BDD(behavioral driven development) instead of TDD
- Purpose of this application (aka behavior of this application ) is clear but interface and api definition is not known up front
- Feature file has higher readability and easy to understand when elaborating numerous hypothesis and can use as a check when evolving features development, failing feature test means some missing behavior of the application that needs to be developed

