from config import Config
from core.move import Move
from core.move_rules import MoveRules
from core.win_rules import WinRules


class Rules:
    """
    Inherits MoveRules and WinRules
    """
    def __init__(self, cfg):
        self.cfg = cfg
        self.move_rules = MoveRules(cfg)
        self.win_rules = WinRules(cfg, self.move_rules)

    # MoveRules methods
    def validateMove(self, stones, move):
        return self.move_rules.validateMove(stones, move)

    def getCaptures(self, stones, move) -> list[Move]:
        return self.move_rules.getCaptures(stones, move)

    def _calculateCaptures(self, stones, move) -> list[Move]:
        return self.move_rules._calculateCaptures(stones, move)

    def countFreeThrees(self, stones, move):
        return self.move_rules.countFreeThrees(stones, move)

    # WinRules methods
    def checkWin(self, stones, move):
        return self.win_rules.checkWin(stones, move)

    def getWinningTiles(self, stones, move):
        return self.win_rules.getWinningTiles(stones, move)

    def canOpponentBreakLine(self, stones, winning_tiles, opponent_colour):
        return self.win_rules.canOpponentBreakLine(stones, winning_tiles, opponent_colour)

    def wouldOpponentWinByCapture(self, stones, winning_tiles, opponent_colour, opponent_captures):
        return self.win_rules.wouldOpponentWinByCapture(stones, winning_tiles, opponent_colour, opponent_captures)

    def evaluate(self, stones, ai_colour, move):
        return self.win_rules.evaluate(stones, ai_colour, move)

    def check_four_in_a_row(self, stones, colour):
        return self.win_rules.check_four_in_a_row(stones, colour)

