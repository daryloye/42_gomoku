from config import Config
from core.move import Move


class WinRules:
    def __init__(self, cfg, move_rules=None):
        self.cfg = cfg
        self.move_rules = move_rules


    def checkWin(self, stones, move):
        if self.cfg.game.difficulty == "no_overline":
            return self._checkWinNoOverline(stones, move)
        else:
            row_length = self._getRowLength(stones, move)
            return row_length >= 5


    def getWinningTiles(self, stones, move):
        """Returns the list of tiles that form the winning line, or None if no win"""
        if self.cfg.game.difficulty == "no_overline":
            if not self._checkWinNoOverline(stones, move):
                return None
            return self._getWinningTilesForMove(stones, move, exactly=5)
        else:
            row_length = self._getRowLength(stones, move)
            if row_length < 5:
                return None
            return self._getWinningTilesForMove(stones, move)


    def canOpponentBreakLine(self, stones, winning_tiles, opponent_colour):
        """Check if opponent can break the winning line by capturing a pair from it"""
        if self.cfg.game.difficulty not in ["ninuki", "pente"]:
            return False

        if not winning_tiles or len(winning_tiles) < 2:
            return False

        if self.move_rules is None:
            return False

        for x in range(self.cfg.board.size):
            for y in range(self.cfg.board.size):
                tile = (x, y)
                if tile in stones.map:
                    continue

                test_move = Move(tile, opponent_colour)
                captures = self.move_rules._calculateCaptures(stones, test_move)

                for capture in captures:
                    if capture.tile in winning_tiles:
                        return True

        return False


    def wouldOpponentWinByCapture(self, stones, winning_tiles, opponent_colour, opponent_captures):
        """Check if opponent would win by capture when breaking the line"""
        if self.cfg.game.difficulty not in ["ninuki", "pente"]:
            return False

        if opponent_captures < 8:
            return False

        if not winning_tiles or len(winning_tiles) < 2:
            return False

        if self.move_rules is None:
            return False

        for x in range(self.cfg.board.size):
            for y in range(self.cfg.board.size):
                tile = (x, y)
                if tile in stones.map:
                    continue

                test_move = Move(tile, opponent_colour)
                captures = self.move_rules._calculateCaptures(stones, test_move)

                captures_from_line = 0
                for capture in captures:
                    if capture.tile in winning_tiles:
                        captures_from_line += 1

                if captures_from_line > 0 and opponent_captures + captures_from_line >= 10:
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


    def _checkWinNoOverline(self, stones, move):
        row_length = self._getRowLength(stones, move)
        return row_length == 5

    def _getWinningTilesForMove(self, stones, move, exactly=None):
        """Get all tiles that form the winning line. If exactly is set, only return lines of that exact length."""
        directions = [
            (0, 1),
            (1, 0),
            (1, 1),
            (1, -1),
        ]

        x, y = move.tile

        for dx, dy in directions:
            tiles_in_line = [move.tile]

            nx, ny = x + dx, y + dy
            while 0 <= nx < self.cfg.board.size and 0 <= ny < self.cfg.board.size:
                if stones.map.get((nx, ny)) == move.colour:
                    tiles_in_line.append((nx, ny))
                    nx += dx
                    ny += dy
                else:
                    break

            nx, ny = x - dx, y - dy
            while 0 <= nx < self.cfg.board.size and 0 <= ny < self.cfg.board.size:
                if stones.map.get((nx, ny)) == move.colour:
                    tiles_in_line.append((nx, ny))
                    nx -= dx
                    ny -= dy
                else:
                    break

            if exactly is not None:
                if len(tiles_in_line) == exactly:
                    return tiles_in_line
            else:
                if len(tiles_in_line) >= 5:
                    return tiles_in_line

        return None

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


    def check_four_in_a_row(self, stones, colour):
        """
        Find all empty positions where placing a stone would create 5 or more in a row (winning moves).
        """
        winning_moves = set()
        directions = [(0, 1), (1, 0), (1, 1), (1, -1)]  # horizontal, vertical, diagonal, anti-diagonal

        for x in range(self.cfg.board.size):
            for y in range(self.cfg.board.size):
                if (x, y) in stones.map:
                    continue

                for dx, dy in directions:
                    count = 1

                    nx, ny = x + dx, y + dy
                    while 0 <= nx < self.cfg.board.size and 0 <= ny < self.cfg.board.size:
                        if stones.map.get((nx, ny)) == colour:
                            count += 1
                            nx += dx
                            ny += dy
                        else:
                            break

                    nx, ny = x - dx, y - dy
                    while 0 <= nx < self.cfg.board.size and 0 <= ny < self.cfg.board.size:
                        if stones.map.get((nx, ny)) == colour:
                            count += 1
                            nx -= dx
                            ny -= dy
                        else:
                            break

                    if count >= 5:
                        winning_moves.add((x, y))

        return list(winning_moves)
