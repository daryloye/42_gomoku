import pygame
from ui.stones import Stones
from core.utils import *
from core.move import Move
from config import Config
from abc import ABC, abstractmethod


class Player(ABC):
	def __init__(self, cfg, colour, name):
		self.cfg = cfg
		self.colour = colour
		self.name = name

	@abstractmethod
	def doAction(self, stones, events) -> Move | None:
		pass

	@classmethod
	def make(cls, kind, cfg, colour, name):
		mapping = {
			"Human": Human,
			"AI": AI
		}
		return mapping[kind](cfg, colour, name)


class Human(Player):
	def doAction(self, stones, events) -> Move | None:
		mouse_pos = pygame.mouse.get_pos()
		mouse_tile = coord_to_nearest_tile(mouse_pos, self.cfg.board)
		move = Move(mouse_tile, self.colour)
		stones.addShadow(move)

		for event in events:
			if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
				return move
		return None


class AI(Player):
	def doAction(self, stones, events) -> Move | None:
		pass
