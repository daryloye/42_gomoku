import pygame
from utils import utils
from config import *

class Stone:
	def __init__(self, tile, colour):
		self.tile = tile
		self.colour = colour
	

	def draw(self, screen):
		pygame.draw.circle(screen, self.colour, utils.tile_to_coord(self.tile), STONE_RADIUS)
