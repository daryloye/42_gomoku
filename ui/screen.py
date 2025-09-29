import pygame
import pygame_menu
import json
import os
from config import Config
from core.utils import *
from ui.stones import Stones


class Screen:
    def __init__(self, cfg: Config):
        self.cfg = cfg
        self.screen = pygame.display.set_mode((self.cfg.display.width, self.cfg.display.height))
        self._menu()

    
    def _menu(self):
        menu = pygame_menu.Menu('Gomoku', self.cfg.display.width, self.cfg.display.height,
                                theme=pygame_menu.themes.THEME_BLUE)
        menu.add.selector('Rules : ', [('Standard', 'standard'), ('No-overline', 'no_overline')], selector_id='difficulty')
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

        def show_standard_examples():
            self._show_standard_examples()

        rules_menu.add.button('Standard Rules Examples', show_standard_examples)
        rules_menu.add.vertical_margin(20)

        rules_menu.add.label('GAMEPLAY:', font_size=24)
        rules_menu.add.label('Players take turns placing stones Black plays first. Click on intersections to place stones', font_size=16)
        rules_menu.add.vertical_margin(20)

        def exit_rules():
            rules_menu.disable()

        rules_menu.add.button('Back to Main Menu', exit_rules)

        rules_menu.mainloop(self.screen)


    def update(self, stones, text):
        self.screen.fill(self.cfg.display.background)
        self._drawBoard()
        self._drawStones(stones)
        self._printText(text)
        pygame.display.flip()


    def _drawBoard(self):
        board = self.cfg.board

        board_rect = pygame.Rect(board.offset, board.offset, board.width, board.height)
        pygame.draw.rect(self.screen, board.background, board_rect)

        # vertical lines
        for i in range(board.size):
            pygame.draw.line(self.screen, self.cfg.colour.black,
                tile_to_coord((i, 0), board),
                tile_to_coord((i, board.size - 1), board))
        
        # horizontal lines
        for i in range(board.size):
            pygame.draw.line(self.screen, self.cfg.colour.black,
                tile_to_coord((0, i), board),
                tile_to_coord((board.size - 1, i), board))


    def _drawStones(self, stones):
        radius = self.cfg.stone.radius
        if stones.shadow:
            tile = stones.shadow.tile
            colour = stones.shadow.colour
            if tile not in stones.map:
                shadow_surface = pygame.Surface((radius*2, radius*2), pygame.SRCALPHA)
                color_with_alpha = (*colour, 128)
                pygame.draw.circle(shadow_surface, color_with_alpha, (radius, radius), radius)
                coord = tile_to_coord(tile, self.cfg.board)
                self.screen.blit(shadow_surface, (coord[0] - radius, coord[1] - radius))

        for tile, colour in stones.map.items():
            pygame.draw.circle(self.screen, colour, tile_to_coord(tile, self.cfg.board), radius)
            pygame.draw.circle(self.screen, getOpposingColour(self.cfg, colour), tile_to_coord(tile, self.cfg.board), radius, 1)

    
    def _printText(self, text):
        if len(text) > 50:
            font_size = 20
        elif len(text) > 40:
            font_size = 24
        else:
            font_size = 28

        font = pygame.font.SysFont('Arial', font_size)
        text_surface = font.render(text, True, self.cfg.colour.black)
        text_rect = text_surface.get_rect()
        text_rect.center = (self.cfg.display.width / 2, self.cfg.board.offset / 2)
        self.screen.blit(text_surface, text_rect)

    def _load_examples(self):
        """Load examples from JSON file"""
        try:
            examples_path = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'examples', 'gomoku_examples.json')
            with open(examples_path, 'r') as f:
                return json.load(f)
        except Exception as e:
            print(f"Failed to load examples: {e}")
            return {'standard_winning': [], 'standard_more': []}

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
