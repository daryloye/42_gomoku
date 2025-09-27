#!/usr/bin/env python3
import os
os.environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "1"
import warnings
warnings.filterwarnings("ignore", category=UserWarning, module="pygame.pkgdata")
from core.game import Game
import pygame


def main():
    pygame.init()
    pygame.display.set_caption("Gomoku")

    game = Game()
    game.run()

    pygame.quit()

if __name__ == "__main__":
    main()
