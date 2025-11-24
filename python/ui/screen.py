import pygame
from config import Config
from core.utils import *
from ui.stones import Stones


class Screen:
    def __init__(self, cfg: Config):
        self.cfg = cfg
        self.screen = pygame.display.set_mode((self.cfg.display.width, self.cfg.display.height))


    def update(self, stones, text_lines, invalid_position=None, winning_tiles=None, suggestions=None):
        try:
            self.screen.fill(self.cfg.display.background)
            self._drawBoard()
            if suggestions:
                self._drawSuggestions(suggestions)
            self._drawStones(stones)
            if invalid_position:
                self._drawInvalidMarker(invalid_position)
            if winning_tiles:
                self._drawWinningTiles(winning_tiles)
            if isinstance(text_lines, str):
                self._printText([text_lines])
            else:
                self._printText(text_lines)
            pygame.display.flip()
        except pygame.error as e:
            print(f"Display update error: {e}")
        except Exception as e:
            print(f"Screen update error: {e}")


    def _drawBoard(self):
        board = self.cfg.board

        board_rect = pygame.Rect(board.offsetLeft, board.offsetTop, board.width, board.height)
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


    def _drawInvalidMarker(self, tile):
        """Draw a red X marker at the invalid move position"""
        coord = tile_to_coord(tile, self.cfg.board)
        radius = self.cfg.stone.radius
        red = (255, 0, 0)
        thickness = 3

        offset = radius * 0.6
        pygame.draw.line(self.screen, red,
                        (coord[0] - offset, coord[1] - offset),
                        (coord[0] + offset, coord[1] + offset), thickness)
        pygame.draw.line(self.screen, red,
                        (coord[0] + offset, coord[1] - offset),
                        (coord[0] - offset, coord[1] + offset), thickness)

        pygame.draw.circle(self.screen, red, coord, radius, 2)

    def _drawWinningTiles(self, winning_tiles):
        """Draw a green circle around the winning tiles"""
        green = (0, 200, 0)
        radius = self.cfg.stone.radius
        for tile in winning_tiles:
            coord = tile_to_coord(tile, self.cfg.board)
            pygame.draw.circle(self.screen, green, coord, radius + 5, 4)

    def _drawSuggestions(self, suggestions):
        """
        Draw suggestion highlights for positions that would create N-in-a-row.
        """
        styles = {
            2: {'color': (173, 216, 230), 'radius_offset': 3, 'thickness': 2},
            3: {'color': (0, 255, 255), 'radius_offset': 5, 'thickness': 2},
            4: {'color': (255, 165, 0), 'radius_offset': 7, 'thickness': 3},
            5: {'color': (255, 0, 255), 'radius_offset': 9, 'thickness': 3},
        }

        base_radius = self.cfg.stone.radius

        for length in sorted(styles.keys()):
            if length in suggestions:
                style = styles[length]
                for tile in suggestions[length]:
                    coord = tile_to_coord(tile, self.cfg.board)

                    center_radius = 4 if length <= 3 else 6
                    pygame.draw.circle(self.screen, style['color'], coord, center_radius)

                    pygame.draw.circle(
                        self.screen,
                        style['color'],
                        coord,
                        base_radius + style['radius_offset'],
                        style['thickness']
                    )

        for length, tiles in suggestions.items():
            if length > 5:
                for tile in tiles:
                    coord = tile_to_coord(tile, self.cfg.board)
                    pygame.draw.circle(self.screen, (255, 0, 255), coord, 6)
                    pygame.draw.circle(self.screen, (255, 0, 255), coord, base_radius + 9, 3)


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

        start_y = (self.cfg.board.offsetTop - total_height) / 2

        for i, line in enumerate(text_lines):
            text_surface = font.render(line, True, self.cfg.colour.black)
            text_rect = text_surface.get_rect()
            text_rect.center = (self.cfg.display.width / 2, start_y + i * line_height + line_height / 2)
            self.screen.blit(text_surface, text_rect)
