import pygame
from ui.screen import Screen
from ui.stones import Stones
from player.player import Player
from core.rules import Rules
from config import Config


class Game:
	def __init__(self):
		self.cfg = Config()

		self.screen = Screen(self.cfg)
		self.rules = Rules(self.cfg)
		
		#TODO: create human or ai player
		self.player1 = Player(self.cfg, self.cfg.colour.black, "BLACK")
		self.player2 = Player(self.cfg, self.cfg.colour.white, "WHITE")

		self.reset()
		self.running = True


	def reset(self):
		self.stones = Stones(self.cfg)
		self.current_player = self.player1
		self.game_over = False
		self.last_action = None
	
		
	def run(self):
		while self.running:
			events = pygame.event.get()
			for event in events:
				if event.type == pygame.QUIT:
					running = False
				elif event.type == pygame.KEYDOWN and self.game_over:
					if event.key == pygame.K_r:
						self.reset()
					elif event.key == pygame.K_ESCAPE:
						self.running = False

			if not self.game_over and self.current_player.doAction(self.stones, events):
				last_tile_placed, last_player_colour = self.current_player.getLastAction()

				# check result
				if self.rules.checkWin(self.stones, last_tile_placed, last_player_colour):
					game_over = True
				else:
					self.current_player = (self.player1 if self.current_player == self.player2 else self.player2)
			
			self.render()
		
	
	def render(self):
		mode_text = f"{self.cfg.game.difficulty} ({self.cfg.board.size}x{self.cfg.board.size}) | {self.current_player.name}"
		if self.game_over:
			self.screen.update(self.stones, f'{mode_text} wins! R=replay ESC=quit')
		else:
			self.screen.update(self.stones, f'{mode_text} to move')

