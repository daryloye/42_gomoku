import pygame
from ui.stones import Stones
from utils import utils
from config import *


class Player:
	def __init__(self, colour, stones, game_rules):
		self.colour = colour
		self.stones = stones
		self.game_rules = game_rules

	
	def doAction(self, events):
		mouse_pos = pygame.mouse.get_pos()
		mouse_tile = utils.coord_to_nearest_tile(mouse_pos, self.game_rules.board_size)
		self.stones.addShadow(mouse_tile, self.colour)

		# place stone
		for event in events:
			if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
				if mouse_tile not in self.stones.map:
					won = self.stones.place(mouse_tile, self.colour)
					if won:
						return 'win'
					return True
		return False
