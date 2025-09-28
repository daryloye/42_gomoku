from config import Config
from ui.stones import Stones
from core.rules import Rules
from core.move import Move
from core.utils import *


def minmax(cfg, stones, rules, player_colour, last_move, depth) -> tuple[int, tuple[int, int]]:
	"""
	player wants to get score = 1
	opponent (last_move.colour) wants to get score = -1
	"""
	if is_terminal(stones, rules, last_move) or depth == 0:
		raw_score = rules.evaluate(stones, last_move)
		signed_score = raw_score if player_colour == last_move.colour else -raw_score
		return (signed_score, last_move.tile)
	
	best_score = -1 if player_colour == last_move.colour else 1
	best_tile = None
	
	for t in getPossibleTiles(stones):
		move = Move(t, player_colour)
		new_stones = stones.copy()
		new_stones.place(move)
		score, _ = minmax(cfg, new_stones, rules, getOpposingColour(cfg, player_colour), move, depth - 1)

		if player_colour == last_move.colour:
			if score > best_score:
				best_score, best_tile = score, t
		else:
			if score < best_score:
				best_score, best_tile = score, t

	return (best_score, best_tile)


def is_terminal(stones, rules, last_move):
	return last_move == None or rules.evaluate(stones, last_move) in (-1, 1) or stones.isFull()


def getPossibleTiles(stones):
	return list(set(stones.allMoves) - set(stones.map))
