from config import Config
from core.move import Move


class Suggestions:
    """
    Finds positions where placing a stone would create N-in-a-row patterns.
    """
    def __init__(self, cfg: Config):
        self.cfg = cfg

    def find_suggestions(self, stones, colour):
        """
        Find all positions where placing a stone would create different N-in-a-row patterns.
        """
        suggestions = {}

        for x in range(self.cfg.board.size):
            for y in range(self.cfg.board.size):
                tile = (x, y)

                if tile in stones.map:
                    continue

                row_length = self._calculate_row_length_at_position(stones, tile, colour)

                if row_length >= 2:
                    if row_length not in suggestions:
                        suggestions[row_length] = []
                    suggestions[row_length].append(tile)

        return suggestions

    def _calculate_row_length_at_position(self, stones, tile, colour):
        """
        Calculate the maximum row length that would be created by placing a stone at the given position.
        """
        directions = [
            (0, 1),
            (1, 0),
            (1, 1),
            (1, -1),
        ]

        x, y = tile
        max_count = 0

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

            max_count = max(max_count, count)

        return max_count

    def get_best_suggestions(self, stones, colour, max_per_length=5):
        """
        Get the best suggestions for each N-in-a-row, limited to max_per_length positions.
        """
        all_suggestions = self.find_suggestions(stones, colour)

        best_suggestions = {}
        for length, positions in all_suggestions.items():
            center = self.cfg.board.size // 2
            sorted_positions = sorted(
                positions,
                key=lambda pos: abs(pos[0] - center) + abs(pos[1] - center)
            )
            best_suggestions[length] = sorted_positions[:max_per_length]

        return best_suggestions
