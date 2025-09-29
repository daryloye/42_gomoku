from config import Config
from ui.stones import Stones
from core.rules import Rules
from core.move import Move
from core.utils import *


class Minimax:
	def __init__(self, cfg, rules, ai_colour, opponent_colour):
		self.cfg = cfg
		self.rules = rules
		self.ai_colour = ai_colour
		self.opponent_colour = opponent_colour


	def choose_move(self, stones, last_move, depth, current_colour=None, alpha=-float('inf'), beta=float('inf')) -> tuple[int, tuple[int, int]]:
		"""
		minimax algorithm: ai wants to maximize its score, opponent wants to minimize ai's score
		alpha-beta pruning: 
			- alpha: best score for ai so far
			- beta: best score for opponent so far
		"""
		if current_colour is None:
			# If first call, decide whose turn it is based on last_move
			if last_move is None:
				current_colour = self.ai_colour
			else:
				current_colour = self.opponent_colour if last_move.colour == self.ai_colour else self.ai_colour

		if self._is_terminal(stones, last_move) or depth == 0:
			score = self.rules.evaluate(stones, self.ai_colour, last_move)
			return (score, last_move.tile if last_move else None)

		is_ai_turn = (current_colour == self.ai_colour)
		best_score = -float('inf') if is_ai_turn else float('inf')
		best_tile = None

		for t in self._getPossibleTiles(stones):
			move = Move(t, current_colour)
			new_stones = stones.copy()
			new_stones.place(move)

			score, _ = self.choose_move(
				new_stones, move, depth - 1,
				self.opponent_colour if current_colour == self.ai_colour else self.ai_colour,
				alpha, beta
			)

			# If a winning move is found, return it immediately
			if (is_ai_turn and score == float('inf')) or (not is_ai_turn and score == -float('inf')):
				return (score, t)

			if is_ai_turn:
				if score > best_score:
					best_score, best_tile = score, t
				alpha = max(alpha, best_score)
				if beta <= alpha:
					break
			else:
				if score < best_score:
					best_score, best_tile = score, t
				beta = min(beta, best_score)
				if beta <= alpha:
					break

		return (best_score, best_tile)


	def _is_terminal(self, stones, last_move):
		if last_move == None:
			return False
		elif self.rules.checkWin(stones, last_move):
			return True
		elif stones.isFull():
			return True
		return False


	def _getPossibleTiles(self, stones):
		# return all empty tiles adjacent to occupied tiles
		occupied = set(stones.map)
		candidates = set()
		size = self.cfg.board.size
		
		for tile in occupied:
			x, y = tile
			for dx in [-1, 0, 1]:
				for dy in [-1, 0, 1]:
					if dx == 0 and dy == 0:
						continue
					neighbor = (x + dx, y + dy)
					if 0 <= neighbor[0] < size and 0 <= neighbor[1] < size:
						if neighbor not in occupied:
							candidates.add(neighbor)
		if not candidates:
			return [(size // 2, size // 2)]
		return list(candidates)


		# unordered_list = list(set(stones.allMoves) - set(stones.map))

		# # prioritize tiles near center, returns 10 closest tiles
		# size = self.cfg.board.size
		# center = (size // 2, size // 2)
		# ordered_list = sorted(
		# 	unordered_list,
		# 	key=lambda t: (t[0] - center[0]) ** 2 + (t[1] - center[1]) ** 2
		# )
		# return ordered_list[:min(len(ordered_list), 50)]
