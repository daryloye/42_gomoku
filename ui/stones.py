import pygame
from core.utils import *
from core.move import Move
from config import Config
from itertools import product


class Stones:
	def __init__(self, cfg):
		self.cfg = cfg
		self.map = {}
		self.shadow = None
		
		lst = list(range(0, self.cfg.board.size))
		self.allMoves = [c for c in product(lst, repeat=2)]


	def copy(self):
		new_stones = Stones(self.cfg)
		new_stones.map = self.map.copy()
		return new_stones


	def place(self, move, colour=None):
		"""Place a stone. Accepts either a Move object or (tile, colour) tuple."""
		if isinstance(move, Move):
			self.map[move.tile] = move.colour
		else:
			self.map[move] = colour


	def remove(self, moves: list[Move]):
		for move in moves:
			del self.map[move.tile]


	def addShadow(self, move: Move):
		self.shadow = move

	
	def isFull(self):
		return len(self.map) == len(self.allMoves)