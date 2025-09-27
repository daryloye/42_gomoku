import pygame
import pygame_menu
from config import Config
from core.utils import *
from ui.stones import Stones

class Screen:
    def __init__(self, cfg: Config):
        self.cfg = cfg
        self.screen = pygame.display.set_mode((self.cfg.display.width, self.cfg.display.height))
        self.menu()

    
    def menu(self):
        menu = pygame_menu.Menu('Gomoku', self.cfg.display.width, self.cfg.display.height,
                                theme=pygame_menu.themes.THEME_BLUE)
        menu.add.selector('Difficulty : ', [('Standard', 'standard'), ('Free-style', 'freestyle')], selector_id='difficulty')
        menu.add.selector('Board Size : ', [('(19x19)', 19), ('(15x15)', 15), ('(5x5)', 5)], selector_id='board_size')
        menu.add.selector('P1 (Black) : ', [('Human', 'human'), ('AI', 'ai')], selector_id='player1')
        menu.add.selector('P2 (White) : ', [('Human', 'human'), ('AI', 'ai')], selector_id='player2')
        menu.add.button('Play', menu.disable)
        menu.add.button('Quit', pygame_menu.events.EXIT)
        
        menu.mainloop(self.screen)

        selection = menu.get_input_data()

        self.cfg.board.size = selection['board_size'][0][1]
        self.cfg.game.difficulty = selection['difficulty'][0][1]
        self.cfg.game.player1 = selection['player1'][0][1]
        self.cfg.game.player2 = selection['player2'][0][1]


    def update(self, stones, text):
        self.screen.fill(self.cfg.display.background)
        self.drawBoard()
        self.drawStones(stones)
        self.printText(text)
        pygame.display.flip()


    def drawBoard(self):
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


    def drawStones(self, stones):
        radius = self.cfg.stone.radius
        if stones.shadow:
            tile, colour = stones.shadow
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

    
    def printText(self, text):
        # Eh text dont exceed screen length le
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
