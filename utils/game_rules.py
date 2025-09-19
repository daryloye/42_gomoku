class GameRules:
    def __init__(self, mode="standard"):
        """
        - "standard": 19x19 board, 5 in a row to win
        - "freestyle": 15x15 board, 5 in a row to win
        """
        self.mode = mode
        self.board_size = 19 if mode == "standard" else 15
        self.win_length = 5
    
    def check_win(self, board_map, last_tile, colour):
        """Check if the last move resulted in a win."""
        if not last_tile:
            return False
            
        x, y = last_tile
        
        directions = [
            (0, 1),
            (1, 0),
            (1, 1),
            (1, -1),
        ]
        
        for dx, dy in directions:
            count = 1

            nx, ny = x + dx, y + dy
            while 0 <= nx < self.board_size and 0 <= ny < self.board_size:
                if board_map.get((nx, ny)) == colour:
                    count += 1
                    nx += dx
                    ny += dy
                else:
                    break

            nx, ny = x - dx, y - dy
            while 0 <= nx < self.board_size and 0 <= ny < self.board_size:
                if board_map.get((nx, ny)) == colour:
                    count += 1
                    nx -= dx
                    ny -= dy
                else:
                    break
            
            if count >= self.win_length:
                return True
        
        return False
    
    def get_board_size(self):
        """Get the board size for current mode."""
        return self.board_size
    
    def get_mode_description(self):
        """Get a description of the current mode."""
        if self.mode == "standard":
            return f"Standard ({self.board_size}x{self.board_size})"
        else:
            return f"Free-style ({self.board_size}x{self.board_size})"