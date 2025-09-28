from config import Config
from core.move import Move


class Rules:
    def __init__(self, cfg):
        self.cfg = cfg


    def validateMove(self, stones, move):
        if move.tile in stones.map:
            return False
        return True
    

    def getCaptures(self, stones, move) -> tuple[Move, Move] | None:
        return None


    def checkWin(self, stones, move):
        """Check if the last move resulted in a win."""
        if self._getRowLength(stones, move) >= 5:
            return True
        return False

    
    def evalutateBoard(self, stones, move):
        score = self._getRowLength(stones, move)
        return score


    def _getRowLength(self, stones, move):
        directions = [
            (0, 1),
            (1, 0),
            (1, 1),
            (1, -1),
        ]

        x, y = move.tile
        max_count = 0

        for dx, dy in directions:
            count = 1

            nx, ny = x + dx, y + dy
            while 0 <= nx < self.cfg.board.size and 0 <= ny < self.cfg.board.size:
                if stones.map.get((nx, ny)) == move.colour:
                    count += 1
                    nx += dx
                    ny += dy
                else:
                    break

            nx, ny = x - dx, y - dy
            while 0 <= nx < self.cfg.board.size and 0 <= ny < self.cfg.board.size:
                if stones.map.get((nx, ny)) == move.colour:
                    count += 1
                    nx -= dx
                    ny -= dy
                else:
                    break
            
            max_count = max(max_count, count)
 
        return max_count
