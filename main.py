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
    player1 = Player(BLACK, stones)  # Black typically goes first in Gomoku
    player2 = Player(WHITE, stones)

    player_turn = 1
    game_over = False
    winner = None
    running = True

    while running:
        events = pygame.event.get()
        for event in events:
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN and game_over:
                if event.key == pygame.K_r:
                    stones.reset()
                    player_turn = 1
                    game_over = False
                    winner = None
                elif event.key == pygame.K_ESCAPE:
                    running = False

        if not game_over:
            if player_turn == 1:
                result = player1.doAction(events)
                if result == 'win':
                    game_over = True
                    winner = 1
                elif result:
                    player_turn = 2
            elif player_turn == 2:
                result = player2.doAction(events)
                if result == 'win':
                    game_over = True
                    winner = 2
                elif result:
                    player_turn = 1

        if game_over:
            color_name = "Black" if winner == 1 else "White"
            screen.update(f'Player {winner} ({color_name}) wins! Press R to replay or ESC to quit')
        else:
            color_name = "Black" if player_turn == 1 else "White"
            screen.update(f'Player {player_turn} ({color_name}) to move')

    pygame.quit()

if __name__ == "__main__":
    main()
