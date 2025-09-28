import pygame
from ui.screen import Screen
from ui.stones import Stones
from player.player import Player
from core.rules import Rules
from core.move import Move
from config import Config


class Game:
	def __init__(self):
		self.cfg = Config()

		self.screen = Screen(self.cfg)
		self.rules = Rules(self.cfg)
		
		self.player1 = Player.make(self.cfg.game.player1, self.cfg, self.cfg.colour.black, "BLACK")
		self.player2 = Player.make(self.cfg.game.player2, self.cfg, self.cfg.colour.white, "WHITE")

		self._reset()
		self.running = True


	def _reset(self):
		self.stones = Stones(self.cfg)
		self.current_player = self.player1
		self.game_over = False
		self.last_action = None
	
		
	def run(self):
		while self.running:
			events = pygame.event.get()
			self._handle_events(events)
			if not self.game_over:
				self._handle_actions(events)
			self._render()


	def _handle_events(self, events):
		for event in events:
			if event.type == pygame.QUIT:
				self.running = False
			elif event.type == pygame.KEYDOWN and self.game_over:
				if event.key == pygame.K_r:
					self._reset()
				elif event.key == pygame.K_ESCAPE:
					self.running = False

	
	def _handle_actions(self, events):
		move = self.current_player.doAction(self.stones, events)
		if move and self.rules.validateMove(self.stones, move):
			self.stones.place(move)

			captures = self.rules.getCaptures(self.stones, move)
			if captures:
				self.stones.remove(captures)

			if self.rules.checkWin(self.stones, move):
				self.game_over = True
			else:
				self.current_player = (
					self.player1 if self.current_player == self.player2 else self.player2
				)


	def _render(self):
		text = f"{self.cfg.game.difficulty} ({self.cfg.board.size}x{self.cfg.board.size}) | {self.current_player.name}"
		if self.game_over:
			self.screen.update(self.stones, f'{text} wins! R=replay ESC=quit')
		else:
			self.screen.update(self.stones, f'{text} to move')

