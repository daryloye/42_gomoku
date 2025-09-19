from config import BOARD_GRID


class WinChecker:
    @staticmethod
    def check_win(board_map, last_tile, colour):
        x, y = last_tile

        directions = [
            (0, 1),   # vertical
            (1, 0),   # horizontal
            (1, 1),   # diagonal \
            (1, -1),  # diagonal /
        ]

        for dx, dy in directions:
            count = 1

            for direction in [1, -1]:
                nx, ny = x + dx * direction, y + dy * direction
                while 0 <= nx < BOARD_GRID and 0 <= ny < BOARD_GRID:
                    if board_map.get((nx, ny)) == colour:
                        count += 1
                        nx += dx * direction
                        ny += dy * direction
                    else:
                        break

            if count >= 5:
                return True

        return False