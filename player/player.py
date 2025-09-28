import pygame
from ui.stones import Stones
from core.utils import *
from core.move import Move
from core.minmax import *
from config import Config
from abc import ABC, abstractmethod


class Player(ABC):
	def __init__(self, cfg, rules, colour, name):
		self.cfg = cfg
		self.rules = rules
		self.colour = colour
		self.name = name

	@abstractmethod
	def doAction(self, stones, events, last_move) -> Move | None:
		pass

	@classmethod
	def make(cls, cfg, rules, playerType, colour, name):
		mapping = {
			cfg.game.humanName: Human,
			cfg.game.aiName: AI
		}
		return mapping[playerType](cfg, rules, colour, name)


class Human(Player):
	def doAction(self, stones, events, _) -> Move | None:
		mouse_pos = pygame.mouse.get_pos()
		mouse_tile = coord_to_nearest_tile(mouse_pos, self.cfg.board)
		move = Move(mouse_tile, self.colour)
		stones.addShadow(move)

		for event in events:
			if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
				return move
		return None


class AI(Player):
	def doAction(self, stones, _, last_move) -> Move | None:
		score, tile = minimax(self.cfg, stones, self.rules, self.colour, last_move, 3)
		if tile == None:
			return None
		move = Move(tile, self.colour)
		print(score)
		return move
