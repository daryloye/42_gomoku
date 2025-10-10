from config import Config
from core.move import Move
from functools import lru_cache


class Rules:
    def __init__(self, cfg):
        self.cfg = cfg


    def validateMove(self, stones, move):
        """If the move is valid, error_message is None"""
        if move.tile in stones.map:
            return False, "Position already occupied!"

        if self.cfg.game.difficulty == "pente":
            move_count = len(stones.map)
            center = self.cfg.board.size // 2
            x, y = move.tile

            if move_count == 0:
                if (x, y) != (center, center):
                    return False, "First move must be at center!"

            elif move_count == 1:
                distance = max(abs(x - center), abs(y - center))
                if distance < 3:
                    return False, "Second move must be at least 3 spaces from center!"

        if self.cfg.game.noDoubleThrees:
            stones.map[move.tile] = move.colour
            free_three_count = self.countFreeThrees(stones, move)
            del stones.map[move.tile]

            if free_three_count >= 2:
                return False, "Cannot create double free-threes!"

        return True, None
    

    def getCaptures(self, stones, move) -> list[Move]:
        """
        Ninuki-renju and Pente: captures exactly 2 opponent stones flanked by your stones.
        """
        if self.cfg.game.difficulty not in ["ninuki", "pente"]:
            return []

        if move.tile not in stones.map:
            return []

        return self._calculateCaptures(stones, move)


    def _calculateCaptures(self, stones, move) -> list[Move]:
        """Calculate captures for a move (helper method that doesn't check if stone is placed)"""
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
            nx1, ny1 = x + dx, y + dy
            nx2, ny2 = x + 2*dx, y + 2*dy
            nx3, ny3 = x + 3*dx, y + 3*dy

            if (0 <= nx1 < self.cfg.board.size and 0 <= ny1 < self.cfg.board.size and
                0 <= nx2 < self.cfg.board.size and 0 <= ny2 < self.cfg.board.size and
                0 <= nx3 < self.cfg.board.size and 0 <= ny3 < self.cfg.board.size and
                stones.map.get((nx1, ny1)) == opponent_color and
                stones.map.get((nx2, ny2)) == opponent_color and
                stones.map.get((nx3, ny3)) == move.colour):
                captures.append(Move((nx1, ny1), opponent_color))
                captures.append(Move((nx2, ny2), opponent_color))

            nx1, ny1 = x - dx, y - dy
            nx2, ny2 = x - 2*dx, y - 2*dy
            nx3, ny3 = x - 3*dx, y - 3*dy

            if (0 <= nx1 < self.cfg.board.size and 0 <= ny1 < self.cfg.board.size and
                0 <= nx2 < self.cfg.board.size and 0 <= ny2 < self.cfg.board.size and
                0 <= nx3 < self.cfg.board.size and 0 <= ny3 < self.cfg.board.size and
                stones.map.get((nx1, ny1)) == opponent_color and
                stones.map.get((nx2, ny2)) == opponent_color and
                stones.map.get((nx3, ny3)) == move.colour):
                captures.append(Move((nx1, ny1), opponent_color))
                captures.append(Move((nx2, ny2), opponent_color))

        return captures


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

        for x in range(self.cfg.board.size):
            for y in range(self.cfg.board.size):
                tile = (x, y)
                if tile in stones.map:
                    continue

                test_move = Move(tile, opponent_colour)
                captures = self._calculateCaptures(stones, test_move)

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

        for x in range(self.cfg.board.size):
            for y in range(self.cfg.board.size):
                tile = (x, y)
                if tile in stones.map:
                    continue

                test_move = Move(tile, opponent_colour)
                captures = self._calculateCaptures(stones, test_move)

                captures_from_line = 0
                for capture in captures:
                    if capture.tile in winning_tiles:
                        captures_from_line += 1

                if captures_from_line > 0 and opponent_captures + captures_from_line >= 10:
                    return True

        return False


    def _isFreeThree(self, stones, move, direction):
        """
        A free-three is a pattern of 3 stones (including gaps) that can develop into a four
        """
        dx, dy = direction
        x, y = move.tile
        colour = move.colour

        positions = []

        for i in range(1, 6):
            nx, ny = x + i * dx, y + i * dy
            if not (0 <= nx < self.cfg.board.size and 0 <= ny < self.cfg.board.size):
                break
            stone_colour = stones.map.get((nx, ny))
            if stone_colour == colour:
                positions.append(i)
            elif stone_colour is not None:
                # Opponent stone blocks this direction
                break

        backward_positions = []
        for i in range(1, 6):
            nx, ny = x - i * dx, y - i * dy
            if not (0 <= nx < self.cfg.board.size and 0 <= ny < self.cfg.board.size):
                break
            stone_colour = stones.map.get((nx, ny))
            if stone_colour == colour:
                backward_positions.append(-i)
            elif stone_colour is not None:
                # Opponent stone blocks this direction
                break

        all_positions = sorted(backward_positions + [0] + positions)

        if len(all_positions) != 3:
            return False

        span = all_positions[-1] - all_positions[0]
        if span > 4:
            return False

        forward_end_x = x + (all_positions[-1] + 1) * dx
        forward_end_y = y + (all_positions[-1] + 1) * dy

        backward_end_x = x + (all_positions[0] - 1) * dx
        backward_end_y = y + (all_positions[0] - 1) * dy

        forward_empty = (
            0 <= forward_end_x < self.cfg.board.size and
            0 <= forward_end_y < self.cfg.board.size and
            (forward_end_x, forward_end_y) not in stones.map
        )

        backward_empty = (
            0 <= backward_end_x < self.cfg.board.size and
            0 <= backward_end_y < self.cfg.board.size and
            (backward_end_x, backward_end_y) not in stones.map
        )

        return forward_empty and backward_empty


    def countFreeThrees(self, stones, move):
        """Count the number of free-three patterns created by a move"""
        directions = [(0, 1), (1, 0), (1, 1), (1, -1)]
        count = 0

        for direction in directions:
            if self._isFreeThree(stones, move, direction):
                count += 1

        return count


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
