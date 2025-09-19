import pygame
from utils import utils
from utils.win_checker import WinChecker
from config import *


class Stones:
	def __init__(self):
		self.map = {}
		self.shadow = None
		self.winner = None

	def reset(self):
		self.map = {}
		self.shadow = None
		self.winner = None

	def place(self, tile, colour):
		self.map[tile] = colour
		print(self.map)
		if WinChecker.check_win(self.map, tile, colour):
			self.winner = colour
			return True
		return False

	def addShadow(self, tile, colour):
		self.shadow = (tile, colour)
