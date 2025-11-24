#!/usr/bin/env python3

import sys
sys.path.append('.')

from config import Config
from core.rules import Rules
from ui.stones import Stones
from core.move import Move

def test_capture():
    cfg = Config()
    cfg.board.size = 19
    cfg.game.difficulty = "captures"

    rules = Rules(cfg)

    stones = Stones(cfg)

    stones.place(Move((5, 5), "white"))
    stones.place(Move((6, 5), "black"))
    stones.place(Move((7, 5), "black"))

    print("Board before capture move:")
    for y in range(3, 10):
        row = ""
        for x in range(3, 12):
            if (x, y) in stones.map:
                if stones.map[(x, y)] == "white":
                    row += "W "
                else:
                    row += "B "
            else:
                row += ". "
        print(f"{y}: {row}")

    test_move = Move((8, 5), "white")
    stones.place(test_move)

    captures = rules.getCaptures(stones, test_move)
    print(f"\nCapture move: {test_move.tile} {test_move.colour}")
    print(f"Expected captures: [(6, 5), (7, 5)] black stones")
    print(f"Actual captures: {[(c.tile, c.colour) for c in captures]}")

    stones2 = Stones(cfg)

    stones2.place(Move((5, 5), "black"))
    stones2.place(Move((5, 6), "white"))
    stones2.place(Move((5, 7), "white"))

    test_move2 = Move((5, 8), "black")
    stones2.place(test_move2)

    captures2 = rules.getCaptures(stones2, test_move2)
    print(f"Capture move: {test_move2.tile} {test_move2.colour}")
    print(f"Expected captures: [(5, 6), (5, 7)] white stones")
    print(f"Actual captures: {[(c.tile, c.colour) for c in captures2]}")

    stones3 = Stones(cfg)

    stones3.place(Move((5, 5), "white"))
    stones3.place(Move((6, 5), "black"))

    test_move3 = Move((7, 5), "white")
    stones3.place(test_move3)

    captures3 = rules.getCaptures(stones3, test_move3)
    print(f"Capture move: {test_move3.tile} {test_move3.colour}")
    print(f"Expected captures: [(6, 5)] black stone")
    print(f"Actual captures: {[(c.tile, c.colour) for c in captures3]}")

if __name__ == "__main__":
    test_capture()
