#!/usr/bin/env python3
import os
os.environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "1"
import pygame
from ui.screen import Screen
from ui.stones import Stones
from player.player import Player
from config import *


def main():
    stones = Stones()
    screen = Screen(stones)
    player1 = Player(WHITE, stones)
    player2 = Player(BLACK, stones)

    player_turn = 1
    running = True
    while running:
        events = pygame.event.get()
        for event in events:
            if event.type == pygame.QUIT:
                running = False
        
        if player_turn == 1 and player1.doAction(events):
            player_turn = 2
        elif player_turn == 2 and player2.doAction(events):
            player_turn = 1
        screen.update(f'Player {player_turn} to move')

    pygame.quit()

if __name__ == "__main__":
    main()
