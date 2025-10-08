import pygame
import random
from ui.stones import Stones
from core.utils import *
from core.move import Move
from core.minmax import Minimax
from config import Config
from abc import ABC, abstractmethod


class Player(ABC):
    def __init__(self, cfg, rules, colour, name):
        self.cfg = cfg
        self.rules = rules
        self.colour = colour
        self.name = name
        self.captures = 0

    @abstractmethod
    def doAction(self, stones, events, last_move) -> Move | None:
        pass

    def reset_captures(self):
        self.captures = 0

    @classmethod
    def make(cls, cfg, rules, playerType, colour, name):
        if playerType == cfg.game.humanName:
            return Human(cfg, rules, colour, name)
        elif playerType == cfg.game.aiName:
            if cfg.game.aiType == 'random':
                return RandomAI(cfg, rules, colour, name)
            else:
                return AI(cfg, rules, colour, name)
        else:
            raise ValueError(f"Unknown player type: {playerType}")


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
	def __init__(self, cfg, rules, colour, name):
		super().__init__(cfg, rules, colour, name)
		self.minimax = Minimax(cfg, rules, colour, getOpposingColour(cfg, colour))

	def doAction(self, stones, _, last_move) -> Move | None:
		try:
			score, tile = self.minimax.choose_move(stones, last_move, 5)
			if tile == None:
				return None
			move = Move(tile, self.colour)
			print(score)
			return move
		except MemoryError:
			print("AI ran out of memory! Making random move...")
			empty_tiles = [(x, y) for x in range(self.cfg.board.size)
			               for y in range(self.cfg.board.size)
			               if (x, y) not in stones.map]
			if empty_tiles:
				tile = random.choice(empty_tiles)
				return Move(tile, self.colour)
			return None
		except RecursionError:
			print("AI recursion error! Making random move...")
			empty_tiles = [(x, y) for x in range(self.cfg.board.size)
			               for y in range(self.cfg.board.size)
			               if (x, y) not in stones.map]
			if empty_tiles:
				tile = random.choice(empty_tiles)
				return Move(tile, self.colour)
			return None
		except Exception as e:
			print(f"AI error: {e}. Making random move...")
			empty_tiles = [(x, y) for x in range(self.cfg.board.size)
			               for y in range(self.cfg.board.size)
			               if (x, y) not in stones.map]
			if empty_tiles:
				tile = random.choice(empty_tiles)
				return Move(tile, self.colour)
			return None


class RandomAI(Player):
	def doAction(self, stones, _, last_move) -> Move | None:
		try:
			empty_tiles = []
			for x in range(self.cfg.board.size):
				for y in range(self.cfg.board.size):
					tile = (x, y)
					if tile not in stones.map:
						empty_tiles.append(tile)

			if empty_tiles:
				tile = random.choice(empty_tiles)
				return Move(tile, self.colour)
			return None
		except Exception as e:
			print(f"RandomAI error: {e}")
			return None
