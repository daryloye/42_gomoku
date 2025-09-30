import os
import json
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
        menu = pygame_menu.Menu('Gomoku', self.cfg.display.width, self.cfg.display.height,
                                theme=pygame_menu.themes.THEME_BLUE)
        menu.add.selector('Rules : ', [('Standard', 'standard'), ('No-overline', 'no_overline'), ('With Captures', 'captures')], selector_id='difficulty')
        menu.add.selector('Board Size : ', [('(19x19)', 19), ('(15x15)', 15), ('(5x5)', 5)], selector_id='board_size')
        menu.add.selector(f'P1 ({self.cfg.game.player1Name}) : ', [(f'{self.cfg.game.humanName}', 1), (f'{self.cfg.game.aiName}', 2)], selector_id='player1')
        menu.add.selector(f'P2 ({self.cfg.game.player2Name}) : ', [(f'{self.cfg.game.humanName}', 1), (f'{self.cfg.game.aiName}', 2)], selector_id='player2')
        menu.add.button('Play', menu.disable)
        menu.add.button('Explain Rules', self._show_rules)
        menu.add.button('Quit', pygame_menu.events.EXIT)
        
        menu.mainloop(self.screen)

        selection = menu.get_input_data()

        self.cfg.board.size = selection['board_size'][0][1]
        self.cfg.game.difficulty = selection['difficulty'][0][1]
        self.cfg.game.player1Type = selection['player1'][0][0]
        self.cfg.game.player2Type = selection['player2'][0][0]


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

        rules_menu.add.label('STANDARD RULES:', font_size=20)
        rules_menu.add.label('5 or more stones in a row wins Traditional gomoku gameplay', font_size=16)
        rules_menu.add.vertical_margin(15)

        rules_menu.add.label('WITH CAPTURES:', font_size=20)
        rules_menu.add.label('Same as standard, but you can capture opponent stones by flanking exactly 2 consecutive stones', font_size=16)
        rules_menu.add.label('Pattern: [Your Stone][Opponent][Opponent][Your New Move] captures the 2 opponent stones', font_size=14)
        rules_menu.add.vertical_margin(15)

        rules_menu.add.button('Standard Rules Examples', self._show_standard_examples)
        rules_menu.add.vertical_margin(20)

        rules_menu.add.label('GAMEPLAY:', font_size=24)
        rules_menu.add.label('Players take turns placing stones Black plays first. Click on intersections to place stones', font_size=16)
        rules_menu.add.vertical_margin(20)

        rules_menu.add.button('Back to Main Menu', rules_menu.disable)

        rules_menu.mainloop(self.screen)


    def _show_standard_examples(self):
        data = self._load_examples()
        examples = [
            {
                'title': 'Standard Rules - WINNING Examples',
                'subtitle': '5 or more stones in a row wins',
                'patterns': data.get('standard_winning', [])
            },
            {
                'title': 'Standard Rules - MORE Examples',
                'subtitle': '6+ stones also win in standard rules',
                'patterns': data.get('standard_more', [])
            }
        ]
        self._show_example_pages(examples)


    def _load_examples(self):
        """Load examples from JSON file"""
        try:
            examples_path = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'examples', 'gomoku_examples.json')
            with open(examples_path, 'r') as f:
                return json.load(f)
        except Exception as e:
            print(f"Failed to load examples: {e}")
            return {'standard_winning': [], 'standard_more': []}


    def _show_example_pages(self, examples):
        current_page = 0
        current_pattern = 0

        while True:
            example = examples[current_page]

            if current_pattern >= len(example['patterns']):
                current_pattern = 0
                current_page += 1
                if current_page >= len(examples):
                    return
                continue

            pattern = example['patterns'][current_pattern]

            board_size = 9
            board_offset = 100
            cell_size = (self.cfg.display.width - 2 * board_offset) // board_size
            stone_radius = cell_size // 3

            self.screen.fill(self.cfg.display.background)

            title_font = pygame.font.SysFont('Arial', 28, bold=True)
            title_surface = title_font.render(example['title'], True, self.cfg.colour.black)
            title_rect = title_surface.get_rect(center=(self.cfg.display.width // 2, 30))
            self.screen.blit(title_surface, title_rect)

            subtitle_font = pygame.font.SysFont('Arial', 20)
            subtitle_surface = subtitle_font.render(example['subtitle'], True, self.cfg.colour.black)
            subtitle_rect = subtitle_surface.get_rect(center=(self.cfg.display.width // 2, 60))
            self.screen.blit(subtitle_surface, subtitle_rect)

            pattern_font = pygame.font.SysFont('Arial', 16)
            pattern_text = f"Pattern {current_pattern + 1} of {len(example['patterns'])}"
            pattern_surface = pattern_font.render(pattern_text, True, self.cfg.colour.black)
            pattern_rect = pattern_surface.get_rect(center=(self.cfg.display.width // 2, 80))
            self.screen.blit(pattern_surface, pattern_rect)

            board_rect = pygame.Rect(board_offset, 90, cell_size * board_size, cell_size * board_size)
            pygame.draw.rect(self.screen, self.cfg.board.background, board_rect)

            for i in range(board_size + 1):
                x = board_offset + i * cell_size
                pygame.draw.line(self.screen, self.cfg.colour.black,
                               (x, 90), (x, 90 + cell_size * board_size))
                y = 90 + i * cell_size
                pygame.draw.line(self.screen, self.cfg.colour.black,
                               (board_offset, y), (board_offset + cell_size * board_size, y))

            if 'background' in pattern:
                for stone in pattern['background']:
                    x, y, color_name = stone
                    if 0 <= x < board_size and 0 <= y < board_size:
                        stone_color = self.cfg.colour.black if color_name == 'black' else self.cfg.colour.white
                        stone_x = board_offset + x * cell_size
                        stone_y = 90 + y * cell_size
                        pygame.draw.circle(self.screen, stone_color, (stone_x, stone_y), stone_radius)
                        pygame.draw.circle(self.screen, self.cfg.colour.black, (stone_x, stone_y), stone_radius, 2)

            for stone in pattern['stones']:
                x, y, color_name = stone
                if 0 <= x < board_size and 0 <= y < board_size:
                    stone_color = self.cfg.colour.black if color_name == 'black' else self.cfg.colour.white
                    stone_x = board_offset + x * cell_size
                    stone_y = 90 + y * cell_size
                    # Draw main stones with red border to highlight them
                    pygame.draw.circle(self.screen, stone_color, (stone_x, stone_y), stone_radius)
                    pygame.draw.circle(self.screen, self.cfg.colour.red, (stone_x, stone_y), stone_radius, 3)

            nav_y = self.cfg.display.height - 60
            nav_font = pygame.font.SysFont('Arial', 16)

            nav_text = "SPACE/CLICK: Next Pattern | ESC: Back to Rules"
            nav_surface = nav_font.render(nav_text, True, self.cfg.colour.black)
            nav_rect = nav_surface.get_rect(center=(self.cfg.display.width // 2, nav_y))
            self.screen.blit(nav_surface, nav_rect)

            pygame.display.flip()

            waiting = True
            while waiting:
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        return
                    elif event.type == pygame.KEYDOWN:
                        if event.key == pygame.K_SPACE or event.key == pygame.K_RETURN:
                            current_pattern += 1
                            waiting = False
                        elif event.key == pygame.K_ESCAPE:
                            return
                    elif event.type == pygame.MOUSEBUTTONDOWN:
                        current_pattern += 1
                        waiting = False

    
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