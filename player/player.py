import pygame
from ui.stones import Stones
from utils import utils
from config import *


class Player:
	def __init__(self, colour, stones):
		self.colour = colour
		self.stones = stones

	
	def doAction(self):
		mouse_pos = pygame.mouse.get_pos()
		mouse_tile = utils.coord_to_nearest_tile(mouse_pos)
		self.stones.addShadow(mouse_tile, GREY)

		# place stone
		if pygame.mouse.get_pressed()[0]:
			#TODO: check collision with existing stones

			self.stones.place(mouse_tile, self.colour)
			return True

		return False
