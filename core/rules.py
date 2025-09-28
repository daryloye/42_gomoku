from config import Config
from core.move import Move
from functools import lru_cache


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

    
    def evaluate(self, stones, ai_colour, move):
        if move == None:
            return 0
        if self.checkWin(stones, move):
            return float('inf') if move.colour == ai_colour else -float('inf')

        scores = {
            1: 1,
            2: 10,
            3: 100,
            4: 1000,
        }

        cache = {}
        def get_row_length_cached(move):
            key = (move.tile, move.colour)
            if key not in cache:
                cache[key] = self._getRowLength(stones, move)
            return cache[key]

        max_ai = 0
        max_opp = 0
        for tile, colour in stones.map.items():
            temp_move = Move(tile, colour)
            length = get_row_length_cached(temp_move)
            score = scores.get(length, 10 ** (length - 1))
            # length = self._getRowLength(stones, temp_move)
            if colour == ai_colour:
                max_ai = max(max_ai, score)
            else:
                if length >= 4:
                    return -1e9
                if length == 3:
                    return -1e6
                max_opp = max(max_opp, score)
        
        print(max_ai, max_opp)
        return max_ai - max_opp * 2


    # TODO: cache this
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
