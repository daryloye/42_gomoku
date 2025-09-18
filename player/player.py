import pygame
from ui.stones import Stones
from utils import utils
from config import *


class Player:
	def __init__(self, colour, stones):
		self.colour = colour
		self.stones = stones

	
	def doAction(self, events):
		mouse_pos = pygame.mouse.get_pos()
		mouse_tile = utils.coord_to_nearest_tile(mouse_pos)
		self.stones.addShadow(mouse_tile, GREY)

		# place stone
		for event in events:
			if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
				if mouse_tile not in self.stones.map:
					self.stones.place(mouse_tile, self.colour)
					return True
		return False
