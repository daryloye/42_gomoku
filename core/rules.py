from config import Config


class Rules:
    def __init__(self, cfg):
        self.cfg = cfg
    

    def checkWin(self, stones, last_tile_placed, last_player_colour):
        """Check if the last move resulted in a win."""
        if self.getRowLength(stones, last_tile_placed, last_player_colour) >= 5:
            return True
        return False

    
    def getRowLength(self, stones, last_tile_placed, last_player_colour):
        directions = [
            (0, 1),
            (1, 0),
            (1, 1),
            (1, -1),
        ]

        x, y = last_tile_placed
        max_count = 0

        for dx, dy in directions:
            count = 1

            nx, ny = x + dx, y + dy
            while 0 <= nx < self.cfg.board.size and 0 <= ny < self.cfg.board.size:
                if stones.map.get((nx, ny)) == last_player_colour:
                    count += 1
                    nx += dx
                    ny += dy
                else:
                    break

            nx, ny = x - dx, y - dy
            while 0 <= nx < self.cfg.board.size and 0 <= ny < self.cfg.board.size:
                if stones.map.get((nx, ny)) == last_player_colour:
                    count += 1
                    nx -= dx
                    ny -= dy
                else:
                    break
            
            max_count = max(max_count, count)
 
        return max_count
