import pygame
import time
from ui.screen import Screen
from ui.stones import Stones
from player.player import Player
from core.rules import Rules
from core.move import Move
from config import Config


class Game:
    def __init__(self):
        self.cfg = Config()

        self.screen = Screen(self.cfg)
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


    def _show_exit_confirmation(self):
        """Show confirmation popup before exiting the application."""
        import pygame_menu

        confirm_menu = pygame_menu.Menu('Confirm Exit', 400, 300,
                                        theme=pygame_menu.themes.THEME_BLUE)

        confirm_menu.add.label('Are you sure you want to quit?', font_size=20)
        confirm_menu.add.vertical_margin(30)

        def quit_app():
            self.exit_type = "quit"
            self.running = False
            confirm_menu.disable()

        def cancel_exit():
            confirm_menu.disable()

        confirm_menu.add.button('Yes, Quit', quit_app)
        confirm_menu.add.button('Cancel', cancel_exit)

        confirm_menu.mainloop(self.screen.screen)


    def _show_menu_confirmation(self):
        """Show confirmation popup before returning to main menu."""
        import pygame_menu

        confirm_menu = pygame_menu.Menu('Return to Menu?', 450, 350,
                                        theme=pygame_menu.themes.THEME_BLUE)

        confirm_menu.add.label('Return to main menu?', font_size=20)
        confirm_menu.add.label('Current game progress will be lost.', font_size=16)
        confirm_menu.add.vertical_margin(30)

        def return_to_menu():
            self.exit_type = "menu"
            self.running = False
            confirm_menu.disable()

        def cancel_return():
            confirm_menu.disable()

        confirm_menu.add.button('Yes, Return to Menu', return_to_menu)
        confirm_menu.add.button('Cancel', cancel_return)

        confirm_menu.mainloop(self.screen.screen)


    def run(self):
        clock = pygame.time.Clock()

        while self.running:
            events = pygame.event.get()
            self._handle_events(events)
            if not self.game_over:
                self._handle_actions(events)
            self._render()

            clock.tick(60)

        return self.exit_type


    def _handle_events(self, events):
        for event in events:
            if event.type == pygame.QUIT:
                self._show_exit_confirmation()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_m:
                    self._show_menu_confirmation()
                elif self.game_over:
                    if event.key == pygame.K_r:
                        self._reset()
                    elif event.key == pygame.K_ESCAPE:
                        self._show_exit_confirmation()
                elif event.key == pygame.K_ESCAPE:
                    self._show_exit_confirmation()


    def _handle_actions(self, events):
        current_time = time.time()

        from player.player import AI
        is_ai_player = isinstance(self.current_player, AI)

        if is_ai_player and (current_time - self.last_move_time) < self.ai_move_delay:
            return

        try:
            move = self.current_player.doAction(self.stones, events, self.last_move)
            if move and self.rules.validateMove(self.stones, move):
                self.last_move = move
                self.stones.place(move)
                self.last_move_time = current_time

                captures = self.rules.getCaptures(self.stones, move)
                if captures:
                    self.stones.remove(captures)

                if self.rules.checkWin(self.stones, move):
                    self.game_over = True
                else:
                    self.current_player = (
                        self.player1 if self.current_player == self.player2 else self.player2
                    )
        except Exception as e:
            print(f"Error in player action: {e}")
            print(f"Current player: {type(self.current_player)} - {self.current_player.name}")


    def _render(self):
        from player.player import AI

        p1_type = " (AI)" if isinstance(self.player1, AI) else ""
        p2_type = " (AI)" if isinstance(self.player2, AI) else ""
        current_type = " (AI)" if isinstance(self.current_player, AI) else ""

        player_info = f"{self.player1.name}{p1_type} vs {self.player2.name}{p2_type}"
        text = f"{self.cfg.game.difficulty} rules ({self.cfg.board.size}x{self.cfg.board.size}) | {player_info}"

        if self.game_over:
            self.screen.update(self.stones, f'{text} | {self.current_player.name}{current_type} wins! R=replay M=menu ESC=quit')
        else:
            self.screen.update(self.stones, f'{text} | {self.current_player.name}{current_type} to move | M=menu ESC=quit')
