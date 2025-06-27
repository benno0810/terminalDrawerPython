def bresenham_line(x1, y1, x2, y2):
    # Bresenham's line algorithm to generate points between (x1, y1) and (x2, y2)
    if (x1, y1) == (x2, y2):
        return [(x1, y1)]
    points = []
    dx = abs(x2 - x1)
    dy = abs(y2 - y1)
    x, y = x1, y1
    sx = 1 if x2 > x1 else -1
    sy = 1 if y2 > y1 else -1
    if dx > dy:
        err = dx // 2
        while x != x2:
            points.append((x, y))
            err -= dy
            if err < 0:
                y += sy
                err += dx
            x += sx
        points.append((x2, y2))
    else:
        err = dy // 2
        while y != y2:
            points.append((x, y))
            err -= dx
            if err < 0:
                x += sx
                err += dy
            y += sy
        points.append((x2, y2))
    return points

def is_point_on_line(x, y, x1, y1, x2, y2):
    # check if point (x, y) is on the line segment from (x1, y1) to (x2, y2)
    if (x1 == x2 and y1 == y2):
        return x == x1 and y == y1
    if (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1) > 1e-5:
        return False
    return min(x1, x2) <= x <= max(x1, x2) and min(y1, y2) <= y <= max(y1, y2)