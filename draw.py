from constants import MAX_DEPTH
from objects import point, line, rectangle, canvasBoard

def processCommand(command: str, canvas: canvasBoard|None) -> canvasBoard|None:
    parts = command.split()
    if not parts:
        raise ValueError("Empty command")
    op = parts[0]

    if op == "C":
        if len(parts) != 3:
            raise ValueError("Canvas command must be: C width height")
        w, h = map(int, parts[1:])
        return canvasBoard(w, h)

    if canvas is None:
        raise ValueError("You must create a canvas first (C x y).")

    match op:
        case "L" | "R":
            canvas.drawObject(parts)
        case "B":
            canvas.fillColor(parts)
        case "Q":
            raise SystemExit("Quitting.")
        case _:
            raise ValueError("Invalid command type must be in [C,L,R,B,Q].")
    return canvas


if __name__ == "__main__":
    canvas = None
    while True:
        cmd = input("Enter command: ")
        try:
            canvas = processCommand(cmd, canvas)
            if canvas:
                canvas.printCanvas()
        except SystemExit as e:
            print(e)
            break
        except Exception as e:
            print("Error:", e)
