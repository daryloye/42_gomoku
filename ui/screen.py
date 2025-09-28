import pygame
import pygame_menu
from config import Config
from core.utils import *
from ui.stones import Stones


class Screen:
    """DOCSTRINGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG"""
    def __init__(self, cfg: Config):
        self.cfg = cfg
        self.screen = pygame.display.set_mode((self.cfg.display.width, self.cfg.display.height))
        self._menu()

    
    def _menu(self):
        menu = pygame_menu.Menu('Gomoku', self.cfg.display.width, self.cfg.display.height,
                                theme=pygame_menu.themes.THEME_BLUE)
        menu.add.selector('Rules : ', [('Standard', 'standard'), ('No-overline', 'no_overline')], selector_id='difficulty')
        menu.add.selector('Board Size : ', [('(19x19)', 19), ('(15x15)', 15), ('(5x5)', 5)], selector_id='board_size')
        menu.add.selector('P1 (Black) : ', [('Human', 'human'), ('AI', 'ai')], selector_id='player1')
        menu.add.selector('P2 (White) : ', [('Human', 'human'), ('AI', 'ai')], selector_id='player2')
        menu.add.button('Play', menu.disable)
        menu.add.button('Explain Rules', self._show_rules)
        menu.add.button('Quit', pygame_menu.events.EXIT)
        
        menu.mainloop(self.screen)

        selection = menu.get_input_data()

        self.cfg.board.size = selection['board_size'][0][1]
        self.cfg.game.difficulty = selection['difficulty'][0][1]
        self.cfg.game.player1 = selection['player1'][0][0]
        self.cfg.game.player2 = selection['player2'][0][0]


    def _show_rules(self):
        rules_menu = pygame_menu.Menu('Game Rules', self.cfg.display.width, self.cfg.display.height,
                                      theme=pygame_menu.themes.THEME_BLUE)

        rules_menu.add.label('GOMOKU RULES', font_size=32)
        rules_menu.add.vertical_margin(20)

        rules_menu.add.label('OBJECTIVE:', font_size=24)
        rules_menu.add.label('Be the first player to get 5 stones in a row', font_size=18)
        rules_menu.add.label('(horizontally, vertically, or diagonally)', font_size=18)
        rules_menu.add.vertical_margin(20)

        rules_menu.add.label('GAME MODES:', font_size=24)
        rules_menu.add.vertical_margin(10)

        rules_menu.add.label('STANDARD RULES:', font_size=20)
        rules_menu.add.label('5 or more stones in a row wins', font_size=16)
        rules_menu.add.label('Traditional gomoku gameplay', font_size=16)
        rules_menu.add.vertical_margin(15)

        rules_menu.add.label('NO-OVERLINE RULES:', font_size=20)
        rules_menu.add.label('EXACTLY 5 stones in a row wins', font_size=16)
        rules_menu.add.label('6 or more stones in a row does NOT win', font_size=16)
        rules_menu.add.label('Adds strategic depth and prevents some', font_size=16)
        rules_menu.add.label('automatic winning patterns', font_size=16)
        rules_menu.add.vertical_margin(20)

        rules_menu.add.label('GAMEPLAY:', font_size=24)
        rules_menu.add.label('Players take turns placing stones', font_size=16)
        rules_menu.add.label('Black plays first', font_size=16)
        rules_menu.add.label('Click on intersections to place stones', font_size=16)
        rules_menu.add.vertical_margin(20)

        def exit_rules():
            rules_menu.disable()

        rules_menu.add.button('Back to Main Menu', exit_rules)

        rules_menu.mainloop(self.screen)


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
                color_with_alpha = (*colour, 128) if colour != self.cfg.colour.grey else (*colour, 64) 
                pygame.draw.circle(shadow_surface, color_with_alpha, (radius, radius), radius)
                coord = tile_to_coord(tile, self.cfg.board)
                self.screen.blit(shadow_surface, (coord[0] - radius, coord[1] - radius))

        for tile, colour in stones.map.items():
            pygame.draw.circle(self.screen, colour, tile_to_coord(tile, self.cfg.board), radius)
            pygame.draw.circle(self.screen,
                self.cfg.colour.black if colour == self.cfg.colour.white else self.cfg.colour.black,
                tile_to_coord(tile, self.cfg.board), radius, 1)

    
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
