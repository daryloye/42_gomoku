import pygame
from config import *
from utils import utils
from ui.stone import Stone

class Screen:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("Gomoku")


    def update(self):
        self.screen.fill(SCREEN_BACKGROUND)
        self.drawBoard()
        self.drawStones()
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
        # TODO: function will take in the player / ai stones as a list. For each stone in the list, draw the stone
        stone = Stone((1, 2), BLACK)
        stone.draw(self.screen)

        stone1 = Stone((3, 4), WHITE)
        stone1.draw(self.screen)