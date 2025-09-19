#!/usr/bin/env python3
import os
os.environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "1"
import warnings
warnings.filterwarnings("ignore", category=UserWarning, module="pygame.pkgdata")
import pygame
from ui.screen import Screen
from ui.stones import Stones
from player.player import Player
from utils.game_rules import GameRules
from config import *


def main():
    game_rules = GameRules("standard")
    stones = Stones(game_rules)
    screen = Screen(stones, game_rules)

    game_rules = GameRules(screen.selected_mode)
    stones = Stones(game_rules)
    screen.stones = stones
    screen.game_rules = game_rules
    
    player1 = Player(BLACK, stones, game_rules)
    player2 = Player(WHITE, stones, game_rules)

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
            mode_text = game_rules.get_mode_description()
            screen.update(f'{mode_text} | {color_name} wins! R=replay ESC=quit')
        else:
            color_name = "Black" if player_turn == 1 else "White"
            mode_text = game_rules.get_mode_description()
            screen.update(f'{mode_text} | {color_name} to move')

    pygame.quit()

if __name__ == "__main__":
    main()