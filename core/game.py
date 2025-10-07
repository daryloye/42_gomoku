import pygame
import time
from ui.screen import Screen
from ui.menu import Menu
from ui.stones import Stones
from player.player import Player
from core.rules import Rules
from core.move import Move
from config import Config


class Game:
    def __init__(self):
        self.cfg = Config()
        self.screen = Screen(self.cfg)
        self.menu = Menu(self.cfg, self.screen.screen)
        self.rules = Rules(self.cfg)

        self.player1 = Player.make(
            self.cfg,
            self.rules,
            self.cfg.game.player1Type,
            self.cfg.game.player1Colour,
            self.cfg.game.player1Name
        )
        self.player2 = Player.make(
            self.cfg,
            self.rules,
            self.cfg.game.player2Type,
            self.cfg.game.player2Colour,
            self.cfg.game.player2Name
        )
        self._reset()
        self.running = True
        self.exit_type = None
        self.ai_move_delay = 1.0
        self.last_move_time = 0


    def _reset(self):
        self.stones = Stones(self.cfg)
        self.current_player = self.player1
        self.game_over = False
        self.last_move = None
        self.player1.reset_captures()
        self.player2.reset_captures()
        self.invalid_move_message = None
        self.invalid_move_time = 0
        self.invalid_move_position = None


    def run(self):
        """
        Setting clock for fps to reduce loops per second to reduce resources consumed
        Secondly also attempting to use this to time the AI, not yet implemented
        """
        clock = pygame.time.Clock()

        while self.running:
            events = pygame.event.get()
            self._handle_events(events)
            if not self.game_over:
                self._handle_actions(events)
            self._render()

            clock.tick(30)

        return self.exit_type


    def _handle_events(self, events):
        def show_exit():
            if self.menu.show_exit_confirmation():
                self.exit_type = "quit"
                self.running = False
        
        def show_menu():
            if self.menu.show_menu_confirmation():
                self.exit_type = "menu"
                self.running = False

        for event in events:
            if event.type == pygame.QUIT:
                show_exit()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_m:
                    show_menu()
                elif self.game_over:
                    if event.key == pygame.K_r:
                        self._reset()
                    elif event.key == pygame.K_ESCAPE:
                        show_exit()
                elif event.key == pygame.K_ESCAPE:
                    show_exit()


    def _handle_actions(self, events):
        current_time = time.time()

        if self.invalid_move_message and (current_time - self.invalid_move_time) > 2.0:
            self.invalid_move_message = None
            self.invalid_move_position = None

        from player.player import AI
        is_ai_player = isinstance(self.current_player, AI)

        if is_ai_player and (current_time - self.last_move_time) < self.ai_move_delay:
            return

        try:
            move = self.current_player.doAction(self.stones, events, self.last_move)
            if move:
                is_valid, error_message = self.rules.validateMove(self.stones, move)
                if is_valid:
                    # Clear any previous error messages
                    self.invalid_move_message = None
                    self.invalid_move_position = None

                    self.last_move = move
                    self.stones.place(move)
                    self.last_move_time = current_time

                    captures = self.rules.getCaptures(self.stones, move)
                    if captures:
                        self.stones.remove(captures)
                        self.current_player.captures += len(captures)

                        self._render()
                        pygame.display.flip()
                        pygame.display.update()

                    if self.rules.checkWin(self.stones, move) or self.current_player.captures >= 10:
                        self.game_over = True
                    else:
                        self.current_player = (
                            self.player1 if self.current_player == self.player2 else self.player2
                        )
                else:
                    self.invalid_move_message = error_message
                    self.invalid_move_time = current_time
                    self.invalid_move_position = move.tile
        except Exception as e:
            print(f"Error in player action: {e}")
            print(f"Current player: {type(self.current_player)} - {self.current_player.name}")


    def _render(self):
        from player.player import AI

        p1_type = " (AI)" if isinstance(self.player1, AI) else ""
        p2_type = " (AI)" if isinstance(self.player2, AI) else ""
        current_type = " (AI)" if isinstance(self.current_player, AI) else ""

        line1 = f"{self.cfg.game.difficulty} rules ({self.cfg.board.size}x{self.cfg.board.size}) | {self.player1.name}{p1_type} vs {self.player2.name}{p2_type}"

        if self.game_over:
            if self.current_player.captures >= 10:
                status = f"{self.current_player.name}{current_type} wins by capture!"
            else:
                status = f"{self.current_player.name}{current_type} wins!"

            if self.cfg.game.difficulty in ["ninuki", "pente"]:
                capture_display = f"Captures: {self.player1.name}={self.player1.captures} {self.player2.name}={self.player2.captures} | "
            else:
                capture_display = ""

            line2 = f"{capture_display}{status} | R=replay M=menu ESC=quit"
        else:
            if self.cfg.game.difficulty in ["ninuki", "pente"]:
                capture_display = f"Captures: {self.player1.name}={self.player1.captures} {self.player2.name}={self.player2.captures} | "
            else:
                capture_display = ""

            line2 = f"{capture_display}{self.current_player.name}{current_type} to move | M=menu ESC=quit"

        text_lines = [line1, line2]
        if self.invalid_move_message:
            text_lines.append(f"INVALID MOVE: {self.invalid_move_message}")

        self.screen.update(self.stones, text_lines, self.invalid_move_position)
