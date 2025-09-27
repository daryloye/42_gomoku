from config import Config


class Rules:
    def __init__(self, cfg):
        self.cfg = cfg
    

    def checkWin(self, stones, last_tile_placed, last_player_colour):
        """Check if the last move resulted in a win."""
        if self.getRowLength(stones, last_tile_placed, last_player_colour) > 5:
            return True
        return False

    
    def getRowLength(self, stones, last_tile_placed, last_player_colour):
        x, y = last_tile_placed
    
        directions = [
            (0, 1),
            (1, 0),
            (1, 1),
            (1, -1),
        ]
        
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
            
            return count
