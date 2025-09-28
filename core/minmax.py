from config import Config
from ui.stones import Stones
from core.rules import Rules
from core.move import Move
from core.utils import *


def minimax(cfg, stones, rules, ai_colour, last_move, depth) -> tuple[int, tuple[int, int]]:
	"""
	ai wants to maximize its score
	opponent wants to minimize ai's score
	"""
	if is_terminal(stones, rules, last_move) or depth == 0:
		raw_score = rules.evaluate(stones, last_move)
		if last_move is None:
			return (0, None)
		# if last move was by opponent, score is negative, else positive
		signed_score = -raw_score if last_move and last_move.colour != ai_colour else raw_score
		return (signed_score, last_move.tile)
	
	current_colour = ai_colour if (last_move is None or last_move.colour != ai_colour) else getOpposingColour(cfg, ai_colour)
	is_ai_turn = (current_colour == ai_colour)
	best_score = -1 if is_ai_turn else 1
	best_tile = None
	
	for t in getPossibleTiles(stones):
		move = Move(t, current_colour)
		new_stones = stones.copy()
		new_stones.place(move)

		score, _ = minmax(cfg, new_stones, rules, ai_colour, move, depth - 1)

		if is_ai_turn and score > best_score:
			best_score, best_tile = score, t
		elif not is_ai_turn and score < best_score:
			best_score, best_tile = score, t

	return (best_score, best_tile)


def is_terminal(stones, rules, last_move):
	return last_move == None or rules.evaluate(stones, last_move) in (-1, 1) or stones.isFull()


def getPossibleTiles(stones):
	return list(set(stones.allMoves) - set(stones.map))
