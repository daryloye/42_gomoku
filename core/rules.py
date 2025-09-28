from config import Config
from core.move import Move


class Rules:
    """docstring for the fun of it"""
    def __init__(self, cfg):
        self.cfg = cfg


    def validateMove(self, stones, move):
        if move.tile in stones.map:
            return False
        return True
    

    def getCaptures(self, stones, move) -> tuple[Move, Move] | None:
        return None


    def checkWin(self, stones, move):
        if self.cfg.game.difficulty == "no_overline":
            return self._checkWinNoOverline(stones, move)
        else:
            row_length = self._getRowLength(stones, move)
            return row_length >= 5

    
    def evalutateBoard(self, stones, move):
        score = self._getRowLength(stones, move)

        if self.cfg.game.difficulty == "no_overline":
            if score > 5:
                score = 0
            elif score == 5:
                score = 1000

        return score


    def _checkWinNoOverline(self, stones, move):
        directions = [
            (0, 1),
            (1, 0),
            (1, 1),
            (1, -1),
        ]

        x, y = move.tile
        color = move.colour

        for dx, dy in directions:
            count = 1

            nx, ny = x + dx, y + dy
            while (0 <= nx < self.cfg.board.size and
                   0 <= ny < self.cfg.board.size and
                   stones.map.get((nx, ny)) == color):
                count += 1
                nx += dx
                ny += dy

            nx, ny = x - dx, y - dy
            while (0 <= nx < self.cfg.board.size and
                   0 <= ny < self.cfg.board.size and
                   stones.map.get((nx, ny)) == color):
                count += 1
                nx -= dx
                ny -= dy

            if count == 5:
                return True

        return False


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
