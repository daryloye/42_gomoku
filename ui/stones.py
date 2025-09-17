import pygame
from utils import utils
from config import *

class Stones:
	def __init__(self):
		self.map = {}
		self.shadow = None
	

	def place(self, tile, colour):
		self.map[tile] = colour
		print(self.map)


	def addShadow(self, tile, colour):
		self.shadow = (tile, colour)