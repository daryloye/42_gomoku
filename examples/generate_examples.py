#!/usr/bin/env python3
"""
Generate valid Gomoku examples that pass validation
"""
import json
import random
import itertools
from typing import List, Tuple, Dict, Any

def find_winning_lines(stones_list, color):
    """Find all winning lines (5+ in a row) for a given color"""
    positions = [(x, y) for x, y, c in stones_list if c == color]
    if not positions:
        return []

    winning_lines = []
    directions = [(1,0), (0,1), (1,1), (1,-1)]  

    for pos in positions:
        for dx, dy in directions:
            line = [pos]
            x, y = pos[0] + dx, pos[1] + dy
            while (x, y) in positions:
                line.append((x, y))
                x, y = x + dx, y + dy
            x, y = pos[0] - dx, pos[1] - dy
            while (x, y) in positions:
                line.insert(0, (x, y))
                x, y = x - dx, y - dy

            if len(line) >= 5:
                line_sorted = tuple(sorted(line))
                if line_sorted not in winning_lines:
                    winning_lines.append(line_sorted)

    return winning_lines

def validate_pattern(pattern):
    """Validate a pattern using same logic as test_validate.py"""
    winning_stones = pattern['stones']
    background_stones = pattern.get('background', [])
    all_stones = winning_stones + background_stones

    black_count = sum(1 for x, y, c in all_stones if c == 'black')
    white_count = sum(1 for x, y, c in all_stones if c == 'white')
    total_stones = black_count + white_count

    if total_stones < 30:
        return False, f"Only {total_stones} stones (need at least 30)"

    winner_color = winning_stones[0][2] if winning_stones else None
    if winner_color == 'black':
        if black_count != white_count + 1:
            return False, f"Black wins but count is B:{black_count} W:{white_count} (should be B=W+1)"
    elif winner_color == 'white':
        if black_count != white_count:
            return False, f"White wins but count is B:{black_count} W:{white_count} (should be B=W)"

    black_lines = find_winning_lines(all_stones, 'black')
    white_lines = find_winning_lines(all_stones, 'white')

    black_has_five_plus = any(len(line) >= 5 for line in black_lines)
    white_has_five_plus = any(len(line) >= 5 for line in white_lines)

    if black_has_five_plus and white_has_five_plus:
        return False, f"Both players have winning lines (only one should win)"

    if not black_has_five_plus and not white_has_five_plus:
        return False, f"No player has winning lines (at least one should win)"

    return True, f"Valid: B:{black_count} W:{white_count} (total: {total_stones})"

def generate_scattered_background(num_black, num_white, avoid_positions, board_size=9):
    """Generate scattered background stones avoiding specific positions"""
    occupied = set(avoid_positions)
    background = []

    available = [(x, y) for x in range(board_size) for y in range(board_size) if (x, y) not in occupied]
    random.shuffle(available)

    stones_to_place = []

    pairs = min(num_black, num_white)
    for _ in range(pairs):
        stones_to_place.extend(['black', 'white'])

    remaining = num_black - pairs
    stones_to_place.extend(['black'] * remaining)

    remaining = num_white - pairs
    stones_to_place.extend(['white'] * remaining)

    for i, color in enumerate(stones_to_place):
        if i < len(available):
            x, y = available[i]
            background.append((x, y, color))

    return background

def generate_winning_pattern(win_type, win_length=5, board_size=9):
    """Generate a single winning pattern"""
    attempts = 0

    while True:
        attempts += 1

        if win_type == 'horizontal':
            start_x = random.randint(0, board_size - win_length)
            start_y = random.randint(0, board_size - 1)
            winning_stones = [(start_x + i, start_y, 'black') for i in range(win_length)]
        elif win_type == 'vertical':
            start_x = random.randint(0, board_size - 1)
            start_y = random.randint(0, board_size - win_length)
            winning_stones = [(start_x, start_y + i, 'black') for i in range(win_length)]
        elif win_type == 'diagonal':
            start_x = random.randint(0, board_size - win_length)
            start_y = random.randint(0, board_size - win_length)
            winning_stones = [(start_x + i, start_y + i, 'black') for i in range(win_length)]
        else:
            start_x = random.randint(win_length - 1, board_size - 1)
            start_y = random.randint(0, board_size - win_length)
            winning_stones = [(start_x - i, start_y + i, 'black') for i in range(win_length)]

        winner_color = winning_stones[0][2]
        total = 31  
        if winner_color == 'black':
            num_black = 16
            num_white = 15
        else:
            num_black = 15
            num_white = 15

        winning_positions = [(x, y) for x, y, c in winning_stones]
        num_black -= len([s for s in winning_stones if s[2] == 'black'])
        num_white -= len([s for s in winning_stones if s[2] == 'white'])

        if num_black < 0 or num_white < 0:
            continue

        background = generate_scattered_background(num_black, num_white, winning_positions, board_size)

        pattern = {
            'stones': winning_stones,
            'background': background
        }

        is_valid, msg = validate_pattern(pattern)
        if is_valid:
            print(f"  Generated after {attempts} attempts: {msg}")
            return pattern, msg

def generate_examples():
    """Generate examples for all categories"""
    examples = {
        'standard_winning': [],
        'standard_more': []
    }

    print("Generating Standard Rules - WINNING Examples...")
    win_types = ['horizontal', 'vertical', 'diagonal']
    for win_type in win_types:
        pattern, msg = generate_winning_pattern(win_type, 5)
        examples['standard_winning'].append(pattern)
        if len(examples['standard_winning']) >= 3:
            break

    print("Generating Standard Rules - MORE Examples...")
    for win_length in [6, 7]:
        pattern, msg = generate_winning_pattern('horizontal', win_length)
        examples['standard_more'].append(pattern)

    pattern, msg = generate_winning_pattern('horizontal', 4)
    examples['standard_more'].append(pattern)

    return examples

def main():
    print("Generating Gomoku examples...")
    random.seed(42)  

    examples = generate_examples()

    output_file = 'gomoku_examples.json'
    with open(output_file, 'w') as f:
        json.dump(examples, f, separators=(',', ':'))

    print(f"\nGenerated examples saved to {output_file}")
    print(f"Standard winning: {len(examples['standard_winning'])} examples")
    print(f"Standard more: {len(examples['standard_more'])} examples")

if __name__ == "__main__":
    main()
