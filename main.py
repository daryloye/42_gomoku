#!/usr/bin/env python3
import pygame
from settings import *

def drawBoard(screen):
    board = pygame.Rect(BOARD_OFFSET, BOARD_OFFSET, BOARD_WIDTH, BOARD_HEIGHT)
    pygame.draw.rect(screen, BOARD_BACKGROUND, board)
    
    pygame.draw.line(screen, BLACK, (50,50), (200,100))


def main():
    pygame.init()
    screen = pygame.display.set_mode(size=(SCREEN_WIDTH, SCREEN_HEIGHT))
    pygame.display.set_caption("Gomoku")

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        screen.fill(SCREEN_BACKGROUND)

        drawBoard(screen)

        pygame.display.flip()

    pygame.quit()

if __name__ == "__main__":
    main()
