import pygame
from core.utils import *
from config import Config


class Stones:
	def __init__(self, cfg):
		self.cfg = cfg
		self.map = {}
		self.shadow = None
	

	def place(self, tile, colour):
		self.map[tile] = colour


	def addShadow(self, tile, colour):
		self.shadow = (tile, colour)
