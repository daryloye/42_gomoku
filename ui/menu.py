import pygame_menu
import pygame
from config import Config
from ui.screen import Screen


class Menu:
    def __init__(self, cfg, screen):
        self.cfg = cfg
        self.screen = screen
        self.main_menu()


    def main_menu(self):
        try:
            menu = pygame_menu.Menu('Gomoku', self.cfg.display.width, self.cfg.display.height,
                                    theme=pygame_menu.themes.THEME_BLUE)
            menu.add.selector('Rules : ', [('Standard', 'standard'), ('No-overline', 'no_overline'), ('Ninuki-renju', 'ninuki'), ('Pente', 'pente')], selector_id='difficulty')
            menu.add.selector('Board Size : ', [('(19x19)', 19), ('(15x15)', 15), ('(13x13)', 13), ('(11x11)', 11), ('(9x9)', 9), ('(7x7)', 7), ('(5x5)', 5)], selector_id='board_size')
            menu.add.selector(f'P1 ({self.cfg.game.player1Name}) : ', [(f'{self.cfg.game.humanName}', 1), (f'{self.cfg.game.aiName}', 2)], selector_id='player1')
            menu.add.selector(f'P2 ({self.cfg.game.player2Name}) : ', [(f'{self.cfg.game.humanName}', 1), (f'{self.cfg.game.aiName}', 2)], selector_id='player2')
            menu.add.selector('AI Type : ', [('Minmax', 'minmax'), ('Random', 'random')], selector_id='ai_type')
            menu.add.selector('No Double-Threes : ', [('Off', False), ('On', True)], selector_id='no_double_threes')
            menu.add.button('Play', menu.disable)
            menu.add.button('Explain Rules', self._show_rules)
            menu.add.button('Quit', pygame_menu.events.EXIT)

            menu.mainloop(self.screen)

            selection = menu.get_input_data()

            self.cfg.board.size = selection['board_size'][0][1]
            self.cfg.game.difficulty = selection['difficulty'][0][1]
            self.cfg.game.player1Type = selection['player1'][0][0]
            self.cfg.game.player2Type = selection['player2'][0][0]
            self.cfg.game.aiType = selection['ai_type'][0][1]
            self.cfg.game.noDoubleThrees = selection['no_double_threes'][0][1]
        except Exception as e:
            print(f"Menu error: {e}")
            raise


    def _show_rules(self):
        rules_menu = pygame_menu.Menu('Game Rules', self.cfg.display.width, self.cfg.display.height,
                                      theme=pygame_menu.themes.THEME_BLUE)

        rules_menu.add.label('GOMOKU RULES', font_size=32)
        rules_menu.add.vertical_margin(20)

        rules_menu.add.label('OBJECTIVE:', font_size=24)
        rules_menu.add.label('Be the first player to get 5 stones in a row, horizontally, vertically, or diagonally)', font_size=18)
        rules_menu.add.vertical_margin(20)

        rules_menu.add.label('GAME MODES:', font_size=24)
        rules_menu.add.vertical_margin(10)


        rules_menu.add.label('NO-OVERLINE:', font_size=20)
        rules_menu.add.label('Win by placing EXACTLY 5 stones in a row (6+ does not win)', font_size=16)
        rules_menu.add.vertical_margin(15)

        rules_menu.add.label('NINUKI-RENJU:', font_size=20)
        rules_menu.add.label('Win by 5 in a row OR by capturing 10 opponent stones (5 pairs)', font_size=16)
        rules_menu.add.label('Capture exactly 2 consecutive opponent stones by flanking them', font_size=14)
        rules_menu.add.label('Pattern: [Your Stone][Opponent][Opponent][Your New Move]', font_size=14)
        rules_menu.add.vertical_margin(15)

        rules_menu.add.label('PENTE:', font_size=20)
        rules_menu.add.label('Same rules as Ninuki-renju (5 in a row OR 10 captured stones)', font_size=16)
        rules_menu.add.label('Capture exactly 2 consecutive opponent stones by flanking them', font_size=14)
        rules_menu.add.vertical_margin(15)

        rules_menu.add.label('GAMEPLAY:', font_size=24)
        rules_menu.add.label('Players take turns placing stones Black plays first. Click on intersections to place stones', font_size=16)
        rules_menu.add.vertical_margin(20)

        rules_menu.add.button('Back to Main Menu', rules_menu.disable)

        rules_menu.mainloop(self.screen)
    def show_exit_confirmation(self):
        """Show confirmation popup before exiting the application."""
        confirm_menu = pygame_menu.Menu('Confirm Exit', 400, 300,
                                        theme=pygame_menu.themes.THEME_BLUE)

        confirm_menu.add.label('Are you sure you want to quit?', font_size=20)
        confirm_menu.add.vertical_margin(30)

        confirm = None

        def quit_app():
            nonlocal confirm
            confirm = True
            confirm_menu.disable()

        def cancel_exit():
            nonlocal confirm
            confirm = False
            confirm_menu.disable()

        confirm_menu.add.button('Yes, Quit', quit_app)
        confirm_menu.add.button('Cancel', cancel_exit)

        confirm_menu.mainloop(self.screen)
        return confirm


    def show_menu_confirmation(self):
        """Show confirmation popup before returning to main menu."""
        confirm_menu = pygame_menu.Menu('Return to Menu?', 450, 350,
                                        theme=pygame_menu.themes.THEME_BLUE)

        confirm_menu.add.label('Return to main menu?', font_size=20)
        confirm_menu.add.label('Current game progress will be lost.', font_size=16)
        confirm_menu.add.vertical_margin(30)

        confirm = None

        def return_to_menu():
            nonlocal confirm
            confirm = True
            confirm_menu.disable()

        def cancel_return():
            nonlocal confirm
            confirm = False
            confirm_menu.disable()

        confirm_menu.add.button('Yes, Return to Menu', return_to_menu)
        confirm_menu.add.button('Cancel', cancel_return)

        confirm_menu.mainloop(self.screen)
        return confirm