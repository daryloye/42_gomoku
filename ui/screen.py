import pygame
import pygame_menu
from config import Config
from core.utils import *
from ui.stones import Stones


class Screen:
    def __init__(self, cfg: Config):
        self.cfg = cfg
        self.screen = pygame.display.set_mode((self.cfg.display.width, self.cfg.display.height))
        self._menu()

    
    def _menu(self):
        menu = pygame_menu.Menu('Gomoku', self.cfg.display.width, self.cfg.display.height,
                                theme=pygame_menu.themes.THEME_BLUE)
        menu.add.selector('Difficulty : ', [('Standard', 'standard'), ('Free-style', 'freestyle')], selector_id='difficulty')
        menu.add.selector('Board Size : ', [('(19x19)', 19), ('(15x15)', 15), ('(11x11)', 11), ('(7x7)', 7), ('(5x5)', 5)], selector_id='board_size')
        menu.add.selector(f'P1 ({self.cfg.game.player1Name}) : ', [(f'{self.cfg.game.humanName}', 1), (f'{self.cfg.game.aiName}', 2)], selector_id='player1')
        menu.add.selector(f'P2 ({self.cfg.game.player2Name}) : ', [(f'{self.cfg.game.humanName}', 1), (f'{self.cfg.game.aiName}', 2)], selector_id='player2')
        menu.add.button('Play', menu.disable)
        menu.add.button('Quit', pygame_menu.events.EXIT)
        
        menu.mainloop(self.screen)

        selection = menu.get_input_data()

        self.cfg.board.size = selection['board_size'][0][1]
        self.cfg.game.difficulty = selection['difficulty'][0][0]
        self.cfg.game.player1Type = selection['player1'][0][0]
        self.cfg.game.player2Type = selection['player2'][0][0]


    def update(self, stones, text):
        self.screen.fill(self.cfg.display.background)
        self._drawBoard()
        self._drawStones(stones)
        self._printText(text)
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

    
    def _printText(self, text):
        if len(text) > 50:
            font_size = 20
        elif len(text) > 40:
            font_size = 24
        else:
            font_size = 28
            
        font = pygame.font.SysFont('Arial', font_size)
        text_surface = font.render(text, True, self.cfg.colour.black)
        text_rect = text_surface.get_rect()
        text_rect.center = (self.cfg.display.width / 2, self.cfg.board.offset / 2)
        self.screen.blit(text_surface, text_rect)
