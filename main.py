#!/usr/bin/env python3
import pygame
from ui.screen import Screen

def main():
    screen = Screen()

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        screen.update()

    pygame.quit()

if __name__ == "__main__":
    main()
