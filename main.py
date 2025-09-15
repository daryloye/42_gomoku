#!/usr/bin/env python3
import pygame

def main():
    pygame.init()
    screen = pygame.display.set_mode((800, 800))
    pygame.display.set_caption("Gomoku")

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        screen.fill((220, 179, 92))
        pygame.display.flip()

    pygame.quit()

if __name__ == "__main__":
    main()
