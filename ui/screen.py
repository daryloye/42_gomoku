import pygame
from config import Config
from core.utils import *
from ui.stones import Stones


class Screen:
    def __init__(self, cfg: Config):
        self.cfg = cfg
        self.screen = pygame.display.set_mode((self.cfg.display.width, self.cfg.display.height))


    def update(self, stones, text_lines):
        self.screen.fill(self.cfg.display.background)
        self._drawBoard()
        self._drawStones(stones)
        if isinstance(text_lines, str):
            self._printText([text_lines])
        else:
            self._printText(text_lines)
        pygame.display.flip()


    def _drawBoard(self):
        board = self.cfg.board

        board_rect = pygame.Rect(board.offset, board.offset, board.width, board.height)
        pygame.draw.rect(self.screen, board.background, board_rect)

        # vertical lines
        for i in range(board.size):
            pygame.draw.line(self.screen, self.cfg.colour.black,
                tile_to_coord((i, 0), board),
                tile_to_coord((i, board.size - 1), board))
        
        # horizontal lines
        for i in range(board.size):
            pygame.draw.line(self.screen, self.cfg.colour.black,
                tile_to_coord((0, i), board),
                tile_to_coord((board.size - 1, i), board))


    def _drawStones(self, stones):
        radius = self.cfg.stone.radius
        if stones.shadow:
            tile = stones.shadow.tile
            colour = stones.shadow.colour
            if tile not in stones.map:
                shadow_surface = pygame.Surface((radius*2, radius*2), pygame.SRCALPHA)
                color_with_alpha = (*colour, 128)
                pygame.draw.circle(shadow_surface, color_with_alpha, (radius, radius), radius)
                coord = tile_to_coord(tile, self.cfg.board)
                self.screen.blit(shadow_surface, (coord[0] - radius, coord[1] - radius))

        for tile, colour in stones.map.items():
            pygame.draw.circle(self.screen, colour, tile_to_coord(tile, self.cfg.board), radius)
            pygame.draw.circle(self.screen, getOpposingColour(self.cfg, colour), tile_to_coord(tile, self.cfg.board), radius, 1)

    
    def _printText(self, text_lines):
        font_size = 22 if len(text_lines) > 1 else 28

        max_length = max(len(line) for line in text_lines)
        if max_length > 60:
            font_size = 18
        elif max_length > 50:
            font_size = 20

        font = pygame.font.SysFont('Arial', font_size)

        line_height = font.get_height()
        total_height = line_height * len(text_lines)

        start_y = (self.cfg.board.offset - total_height) / 2

        for i, line in enumerate(text_lines):
            text_surface = font.render(line, True, self.cfg.colour.black)
            text_rect = text_surface.get_rect()
            text_rect.center = (self.cfg.display.width / 2, start_y + i * line_height + line_height / 2)
            self.screen.blit(text_surface, text_rect)
