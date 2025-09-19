import pygame
from utils import utils
from config import *


class Stones:
	def __init__(self, game_rules):
		self.map = {}
		self.shadow = None
		self.winner = None
		self.game_rules = game_rules

	def reset(self):
		self.map = {}
		self.shadow = None
		self.winner = None

	def place(self, tile, colour):
		self.map[tile] = colour
		print(self.map)
		if self.game_rules.check_win(self.map, tile, colour):
			self.winner = colour
			return True
		return False

	def addShadow(self, tile, colour):
		self.shadow = (tile, colour)
