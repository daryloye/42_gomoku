#!/usr/bin/env python3

import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

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

def validate_example(pattern):
    """Validate stone counts follow proper Gomoku logic"""
    winning_stones = pattern['stones']
    background_stones = pattern.get('background', [])

    black_count = 0
    white_count = 0

    for stone in winning_stones + background_stones:
        if stone[2] == 'black':
            black_count += 1
        else:
            white_count += 1

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

    all_stones = winning_stones + background_stones
    black_lines = find_winning_lines(all_stones, 'black')
    white_lines = find_winning_lines(all_stones, 'white')

    black_has_five_plus = any(len(line) >= 5 for line in black_lines)
    white_has_five_plus = any(len(line) >= 5 for line in white_lines)

    if black_has_five_plus and white_has_five_plus:
        return False, f"Both players have winning lines (only one should win)"

    if not black_has_five_plus and not white_has_five_plus:
        return False, f"No player has winning lines (at least one should win)"

    return True, f"Valid: B:{black_count} W:{white_count} (total: {total_stones})"

def get_examples_from_json():
    """Load examples from JSON file"""
    import json
    import os

    try:
        examples_path = os.path.join('examples', 'gomoku_examples.json')
        with open(examples_path, 'r') as f:
            data = json.load(f)

        all_examples = []

        all_examples.append({
            'title': 'Standard Rules - WINNING Examples',
            'patterns': data.get('standard_winning', [])
        })

        all_examples.append({
            'title': 'Standard Rules - MORE Examples',
            'patterns': data.get('standard_more', [])
        })

        return all_examples
    except Exception as e:
        print(f"Failed to load examples from JSON: {e}")
        return []

def validate_all_examples():
    """Validate all examples from JSON file"""
    print("Validating Gomoku examples...")

    try:
        all_examples = get_examples_from_json()
    except Exception as e:
        print(f"Error loading examples: {e}")
        return False

    all_valid = True

    for example in all_examples:
        print(f"\n{example['title']}:")
        for i, pattern in enumerate(example['patterns']):
            valid, msg = validate_example(pattern)
            if valid:
                print(f"  Pattern {i+1}: PASS - {msg}")
            else:
                print(f"  Pattern {i+1}: FAIL - {msg}")
                all_valid = False

    return all_valid

if __name__ == "__main__":
    success = validate_all_examples()
    if success:
        print("\nAll examples are valid!")
        exit(0)
    else:
        print("\nValidation failed - fix examples before proceeding")
        exit(1)
