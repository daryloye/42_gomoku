import pygame
import pygame_menu
from config import *
from utils import utils
from ui.stones import Stones
from player.player import Player

class Screen:
    def __init__(self, stones):
        self.stones = stones

        pygame.init()
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("Gomoku")
        self.menu()

    
    def menu(self):
        def start_game():
            selection = menu.get_input_data()
            print(selection)
            for widget_id, (selected, idx) in selection.items():
                difficulty_name, number = selected
                print("Selected:", difficulty_name)
                print("Code:", number)
                print("Index:", idx)
            # TODO: save selection. currently this is not used
            menu.disable()

        menu = pygame_menu.Menu('Gomoku', SCREEN_WIDTH, SCREEN_HEIGHT,
                                theme=pygame_menu.themes.THEME_BLUE)
        menu.add.selector('Difficulty : ', [('Standard', 1), ('Pro', 2), ('Swap', 3), ('Swap2', 4)])
        menu.add.selector('Colour : ', [('Black', 1), ('White',2)])
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

        # vertical lines
        for i in range(BOARD_GRID):
            pygame.draw.line(self.screen, BLACK,
                utils.tile_to_coord((i, 0)),
                utils.tile_to_coord((i, BOARD_GRID - 1)))
        
        # horizontal lines
        for i in range(BOARD_GRID):
            pygame.draw.line(self.screen, BLACK,
                utils.tile_to_coord((0, i)),
                utils.tile_to_coord((BOARD_GRID - 1, i)))


    def drawStones(self):
        tile, colour = self.stones.shadow
        pygame.draw.circle(self.screen, GREY, utils.tile_to_coord(tile), STONE_RADIUS)
        
        for tile, colour in self.stones.map.items():
            pygame.draw.circle(self.screen, colour, utils.tile_to_coord(tile), STONE_RADIUS)
            pygame.draw.circle(self.screen, GREY, utils.tile_to_coord(tile), STONE_RADIUS, 1)

    
    def printText(self, text):
        font = pygame.font.SysFont('Arial', 32)
        text_surface = font.render(text, True, BLACK)
        text_rect = text_surface.get_rect()
        text_rect.center = (SCREEN_WIDTH / 2, BOARD_OFFSET / 2)
        self.screen.blit(text_surface, text_rect)
