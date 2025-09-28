import pygame
from core.utils import *
from core.move import Move
from config import Config


class Stones:
	def __init__(self, cfg):
		self.cfg = cfg
		self.map = {}
		self.shadow = None
	

	def place(self, move: Move):
		self.map[move.tile] = move.colour


	def remove(self, moves: list[Move]):
		for move in moves:
			del self.map[move.tile]


	def addShadow(self, move: Move):
		self.shadow = move
