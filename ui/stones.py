import pygame
from utils import utils
from config import *


class Stones:
	def __init__(self):
		self.map = {}
		self.shadow = None
		self.winner = None
		self.allMoves = {(x, y) for x in range(BOARD_GRID) for y in range(BOARD_GRID)}

	def reset(self):
		self.map = {}
		self.shadow = None
		self.winner = None

	def place(self, tile, colour):
		self.map[tile] = colour
		return self
		# if self.check_win(tile, colour):
		# 	self.winner = colour
		# 	return True
		# return False

	def addShadow(self, tile, colour):
		self.shadow = (tile, colour)

	def check_win(self, last_tile, colour):
		x, y = last_tile

		directions = [
			(0, 1),   # vertical
			(1, 0),   # horizontal
			(1, 1),   # diagonal \
			(1, -1),  # diagonal /
		]

		for dx, dy in directions:
			count = 1

			for direction in [1, -1]:
				nx, ny = x + dx * direction, y + dy * direction
				while 0 <= nx < BOARD_GRID and 0 <= ny < BOARD_GRID:
					if self.map.get((nx, ny)) == colour:
						count += 1
						nx += dx * direction
						ny += dy * direction
					else:
						break

			if count >= 5:
				return True

		return False
