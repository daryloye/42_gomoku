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
    

    def getCaptures(self, stones, move) -> list[Move]:
        """
        Returns a list of opponent stones that should be captured after placing the given move.
        Only works when capture rules are enabled.
        """
        if self.cfg.game.difficulty != "captures":
            return []

        if move.tile not in stones.map:
            return []

        captures = []

        if isinstance(move.colour, str):
            opponent_color = "black" if move.colour == "white" else "white"
        else:
            black_color = self.cfg.colour.black
            white_color = self.cfg.colour.white
            opponent_color = black_color if move.colour == white_color else white_color

        directions = [(0, 1), (1, 0), (1, 1), (1, -1)]  # horizontal, vertical, diagonal
        x, y = move.tile

        for dx, dy in directions:
            # Check positive direction
            captured_stones = []
            nx, ny = x + dx, y + dy

            while (0 <= nx < self.cfg.board.size and
                   0 <= ny < self.cfg.board.size and
                   stones.map.get((nx, ny)) == opponent_color):
                captured_stones.append(Move((nx, ny), opponent_color))
                nx += dx
                ny += dy

            if (captured_stones and
                0 <= nx < self.cfg.board.size and
                0 <= ny < self.cfg.board.size and
                stones.map.get((nx, ny)) == move.colour):
                captures.extend(captured_stones)

            captured_stones = []
            nx, ny = x - dx, y - dy

            while (0 <= nx < self.cfg.board.size and
                   0 <= ny < self.cfg.board.size and
                   stones.map.get((nx, ny)) == opponent_color):
                captured_stones.append(Move((nx, ny), opponent_color))
                nx -= dx
                ny -= dy

            if (captured_stones and
                0 <= nx < self.cfg.board.size and
                0 <= ny < self.cfg.board.size and
                stones.map.get((nx, ny)) == move.colour):
                captures.extend(captured_stones)


        return captures



    def checkWin(self, stones, move):
        if self.cfg.game.difficulty == "no_overline":
            return self._checkWinNoOverline(stones, move)
        else:
            row_length = self._getRowLength(stones, move)
            return row_length >= 5

    
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


    def _checkWinNoOverline(self, stones, move):
        row_length = self._getRowLength(stones, move)
        return row_length == 5


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
