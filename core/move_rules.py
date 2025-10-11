from config import Config
from core.move import Move


class MoveRules:
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
