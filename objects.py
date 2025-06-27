from constants import MAX_DEPTH
from collections import defaultdict

from util import bresenham_line, is_point_on_line
class point:
    def __init__(self, x:int, y:int,z:int=MAX_DEPTH):
        if x<0 or y<0 or z<0:
            raise ValueError("Coordinates must be non-negative integers")
        self.x = x
        self.y = y
        self.z = z
    def __eq__(self, other):
        if not isinstance(other, point):
            return NotImplemented
        return self.x == other.x and self.y == other.y and self.z == other.z
class line:
    def __init__(self, start:point, end:point, color:str='o'):
        if start==end:
            raise ValueError("Invalid line. Start and end points cannot be the same.")
        if len(color)!= 1:
                raise ValueError("Color must be a single character.")
        self.start = start
        self.end = end
        self.color = color

class rectangle:
    def __init__(self, a:point, c:point,color:str = 'x'):

        self.width = abs(c.x-a.x)
        self.height = abs(c.y-a.y)
        self.a = a
        self.c = c
        self.b = point(a.x, c.y, a.z)
        self.d = point(c.x, a.y, a.z)
        self.color = color
        self.filled = False 
        self.filled_color = ' '
    def fill(self,color:str):
        self.filled_color = color
        self.filled = True

class canvasBoard:

    def __init__(self, width: int, height: int):
        if width < 0 or height < 0:
            raise ValueError("Canvas dimensions must be non-negative integers")
        self.width = width
        self.height = height
        self.current_z = MAX_DEPTH
        self.objects = []
        self.canvas = [[' ' for _ in range(width)] for _ in range(height)]
        self.depth_map = [[MAX_DEPTH for _ in range(width)] for _ in range(height)]
        self.z_index = defaultdict(list)
    def _checkPoint(self, x: int, y: int):
        return x >= 0 and x < self.width and y >= 0 and y < self.height
    def _checkColor(self, color: str):
        return len(color) == 1
    def _findTopZ(self, x, y):
        topmost_z = MAX_DEPTH # actually is the lowest z-index)
        obj_candidates = []
        if not self._checkPoint(x, y):
            raise ValueError("Coordinates exceed canvas size.")
        for z, objects in sorted(self.z_index.items(), reverse=True):
            obj_candidates = [] # reset candidate list for each z-index
            for obj in objects:
                if isinstance(obj, rectangle):
                    min_x = min(obj.a.x, obj.c.x)
                    max_x = max(obj.a.x, obj.c.x)
                    min_y = min(obj.a.y, obj.c.y)
                    max_y = max(obj.a.y, obj.c.y)
                    if (min_x < x < max_x) and (min_y < y < max_y): # strictly inside
                        if z > topmost_z: # find a hit
                            topmost_z = z
                            obj_candidates.append(obj)
                        
                elif isinstance(obj, line):
                    # cross product to check point on line
                    if is_point_on_line(x, y, obj.start.x, obj.start.y, obj.end.x, obj.end.y):
                        if z > topmost_z:
                            topmost_z = z
                            obj_candidates.append(obj)
            if obj_candidates:
                return [topmost_z, obj_candidates[-1]] # get latest added objects with highest z-index
        return  [MAX_DEPTH,None] 
    def drawObject(self, params):
        self.current_z += 1
        op = params[0]
        try:
            if op == "L":
                if len(params) != 6:
                    raise ValueError("Invalid command format for line creation. example: L x1 y1 x2 y2 color")
                _, x1s, y1s, x2s, y2s, color = params
                x1, y1, x2, y2 = map(int, (x1s, y1s, x2s, y2s))
                #color check
                if not self._checkColor(color):
                    raise ValueError("Color must be a single character.")
                # bounds check
                if not self._checkPoint(x1, y1) or not self._checkPoint(x2, y2):
                    raise ValueError("Invalid line. Coordinates exceed canvas size.")
                start = point(x1, y1, self.current_z)
                end   = point(x2, y2, self.current_z)
                obj   = line(start, end, color)

            elif op == "R":
                if len(params) != 6:
                    raise ValueError("Invalid command format for rectangle creation. example: R x1 y1 x2 y2 color")
                _, x1s, y1s, x2s, y2s, color = params
                x1, y1, x2, y2 = map(int, (x1s, y1s, x2s, y2s))

                #color check
                if not self._checkColor(color):
                    raise ValueError("Color must be a single character.")
                # bounds check
                if not self._checkPoint(x1, y1) or not self._checkPoint(x2, y2):
                    raise ValueError("Invalid rectangle. Coordinates exceed canvas size.")

                a   = point(x1, y1, self.current_z)
                c   = point(x2, y2, self.current_z)
                obj = rectangle(a, c, color)

            else:
                raise ValueError("Invalid draw command.")

            self.addObject(obj)
        except Exception as e:
            self.current_z -= 1
            raise e
    
            
    def fillColor(self, params):
        if len(params) != 4:
            raise ValueError("Invalid command format for bucket filling.")
        x, y = int(params[1]), int(params[2])
        color = params[3]
        if not self._checkColor(color):
            raise ValueError("Color must be a single character.")
        if not self._checkPoint(x, y):
            raise ValueError("Coordinates exceed canvas size.")
        target_z, obj = self._findTopZ(x, y)
        
        # pointing at the background, do nothing
        if target_z == MAX_DEPTH:
            return
        if isinstance(obj, rectangle) and (obj.a.x <= x <= obj.c.x) and (obj.a.y <= y <= obj.c.y):
            obj.fill(color)
        elif isinstance(obj, line) and ((obj.start.x <= x <= obj.end.x) and (obj.start.y <= y <= obj.end.y)):
                obj.color = color

    def addObject(self, obj):
        if not isinstance(obj, (line, rectangle)):
            raise TypeError("Object must be of type line or rectangle")
        self.objects.append(obj)
        if isinstance(obj, line):
            self.z_index[obj.start.z].append(obj)
        if isinstance(obj, rectangle):
            self.z_index[obj.a.z].append(obj)

    def printCanvas(self):
        self.renderCanvas()
        lines = []
        lines.append('-' * (self.width + 2))
        for row in self.canvas:
            lines.append('|' + ''.join(row) + '|')
        lines.append('-' * (self.width + 2))
        print( '\n'.join(lines))
        print('\n')

    def renderCanvas(self):
        # Clear
        self.canvas = [[' ' for _ in range(self.width)] for _ in range(self.height)]
        self.depth_map = [[MAX_DEPTH for _ in range(self.width)] for _ in range(self.height)]
        for obj in self.objects:
            if isinstance(obj, line):
                line_color = obj.color
                line_z = obj.start.z
                start_x = obj.start.x
                start_y = obj.start.y
                end_x = obj.end.x
                end_y = obj.end.y
                points = bresenham_line(start_x, start_y, end_x, end_y)
                for x, y in points:
                    if self._checkPoint(x, y):
                        # border pixels always rendered
                        if self.depth_map[y][x] < line_z:
                            self.canvas[y][x] = line_color
                            self.depth_map[y][x] = line_z
            elif isinstance(obj, rectangle):
                rect_color = obj.color
                rect_z = obj.a.z
                min_x = min(obj.a.x, obj.c.x)
                max_x = max(obj.a.x, obj.c.x)
                min_y = min(obj.a.y, obj.c.y)
                max_y = max(obj.a.y, obj.c.y)
                for x in range(min_x, max_x + 1):
                    for y in range(min_y, max_y + 1):
                        # border pixels always rendered
                        if (x == min_x or x == max_x or y == min_y or y == max_y):
                            if self.depth_map[y][x]  < rect_z:
                                self.canvas[y][x] = rect_color
                                self.depth_map[y][x] = rect_z
                        elif obj.filled and self.depth_map[y][x] < rect_z:
                            self.canvas[y][x] = obj.filled_color
                            self.depth_map[y][x] = rect_z


