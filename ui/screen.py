import pygame
import pygame_menu
from config import *
from utils import utils

class Screen:
    def __init__(self, stones, game_rules):
        self.stones = stones
        self.game_rules = game_rules
        self.selected_mode = "standard"

        pygame.init()
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("Gomoku")
        self.menu()
    
    def menu(self):
        def start_game():
            selection = menu.get_input_data()
            print(selection)
            for widget_id, (selected, idx) in selection.items():
                if 'difficulty' in widget_id.lower():
                    difficulty_name, number = selected
                    print("Selected:", difficulty_name)
                    print("Code:", number)
                    print("Index:", idx)
                    if number == 1:
                        self.selected_mode = "standard"
                    elif number == 2:
                        self.selected_mode = "freestyle"
            menu.disable()

        menu = pygame_menu.Menu('Gomoku', SCREEN_WIDTH, SCREEN_HEIGHT,
                                theme=pygame_menu.themes.THEME_BLUE)
        menu.add.selector('Difficulty : ', [('Standard', 1), ('Free-style', 2)], selector_id='difficulty')
        menu.add.selector('Colour : ', [('Black', 1), ('White', 2)])
        menu.add.button('Play', start_game)
        menu.add.button('Quit', pygame_menu.events.EXIT)
        menu.mainloop(self.screen)

    


    def update(self, text):
        self.screen.fill(SCREEN_BACKGROUND)
        self.drawBoard()
        self.drawStones()
        self.printText(text)
        pygame.display.flip()


    def drawBoard(self):
        board = pygame.Rect(BOARD_OFFSET, BOARD_OFFSET, BOARD_WIDTH, BOARD_HEIGHT)
        pygame.draw.rect(self.screen, BOARD_BACKGROUND, board)

        board_size = self.game_rules.board_size
        
        # vertical lines
        for i in range(board_size):
            pygame.draw.line(self.screen, BLACK,
                utils.tile_to_coord((i, 0), board_size),
                utils.tile_to_coord((i, board_size - 1), board_size))
        
        # horizontal lines
        for i in range(board_size):
            pygame.draw.line(self.screen, BLACK,
                utils.tile_to_coord((0, i), board_size),
                utils.tile_to_coord((board_size - 1, i), board_size))


    def drawStones(self):
        if self.stones.shadow:
            tile, colour = self.stones.shadow
            if tile not in self.stones.map:
                shadow_surface = pygame.Surface((STONE_RADIUS * 2, STONE_RADIUS * 2), pygame.SRCALPHA)
                color_with_alpha = (*colour, 128) if colour != GREY else (*colour, 64) 
                pygame.draw.circle(shadow_surface, color_with_alpha, (STONE_RADIUS, STONE_RADIUS), STONE_RADIUS)
                coord = utils.tile_to_coord(tile, self.game_rules.board_size)
                self.screen.blit(shadow_surface, (coord[0] - STONE_RADIUS, coord[1] - STONE_RADIUS))

        for tile, colour in self.stones.map.items():
            pygame.draw.circle(self.screen, colour, utils.tile_to_coord(tile, self.game_rules.board_size), STONE_RADIUS)
            pygame.draw.circle(self.screen, BLACK if colour == WHITE else WHITE, utils.tile_to_coord(tile, self.game_rules.board_size), STONE_RADIUS, 1)

    
    def printText(self, text):
        # Eh text dont exceed screen length le
        if len(text) > 50:
            font_size = 20
        elif len(text) > 40:
            font_size = 24
        else:
            font_size = 28
            
        font = pygame.font.SysFont('Arial', font_size)
        text_surface = font.render(text, True, BLACK)
        text_rect = text_surface.get_rect()
        text_rect.center = (SCREEN_WIDTH / 2, BOARD_OFFSET / 2)
        self.screen.blit(text_surface, text_rect)
