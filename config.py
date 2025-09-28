from dataclasses import dataclass, field

@dataclass
class ColourConfig:
	white: tuple[int, int, int] = (255, 255, 255)
	black: tuple[int, int, int] = (0, 0, 0)
	grey: tuple[int, int, int] = (150, 150, 150)
	red: tuple[int, int, int] = (255, 0, 0)
	blue: tuple[int, int, int] = (0, 0, 255)


@dataclass
class DisplayConfig:
	width: int = 800
	height: int = 800
	background: tuple[int, int, int] = (237, 232, 208)


@dataclass
class BoardConfig:
	offset: int = 50
	width: int = field(init=False)
	height: int = field(init=False)
	background: tuple[int, int, int] = (217,217,214)
	size: int = field(init=False)


@dataclass
class StoneConfig:
	radius: int = 15


@dataclass
class GameConfig:
	difficulty: str = field(init=False)
	player1: str = field(init=False)
	player2: str = field(init=False)


@dataclass
class Config:
	colour: ColourConfig = field(default_factory=ColourConfig)
	display: DisplayConfig = field(default_factory=DisplayConfig)
	board: BoardConfig = field(default_factory=BoardConfig)
	stone: StoneConfig = field(default_factory=StoneConfig)
	game: GameConfig = field(default_factory=GameConfig)

	def __post_init__(self):
		self.board.width = self.display.width - self.board.offset * 2
		self.board.height = self.display.height - self.board.offset * 2
