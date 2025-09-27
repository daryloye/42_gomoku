import pygame
from ui.stones import Stones
from core.utils import *
from config import Config


class Player:
	def __init__(self, cfg, colour, name):
		self.cfg = cfg
		self.colour = colour
		self.name = name
		self.lastAction = None

	
	def doAction(self, stones, events):
		mouse_pos = pygame.mouse.get_pos()
		mouse_tile = coord_to_nearest_tile(mouse_pos, self.cfg.board)
		stones.addShadow(mouse_tile, self.colour)

		# place stone
		for event in events:
			if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
				if mouse_tile not in stones.map:
					#TODO: validate action from game rules

					stones.place(mouse_tile, self.colour)
					self.lastAction = (mouse_tile, self.colour)
					return True
		return False
	
	
	def getLastAction(self):
		"""Returns (tile_placed, player_colour)"""
		return self.lastAction
