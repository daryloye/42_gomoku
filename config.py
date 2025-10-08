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
	offsetTop: int = 80
	offsetLeft: int = 40
	offsetRight: int = 40
	offsetBottom: int = 40
	width: int = field(init=False)
	height: int = field(init=False)
	background: tuple[int, int, int] = (217,217,214)
	size: int = 19


@dataclass
class StoneConfig:
	radius: int = 15


@dataclass
class GameConfig:
	difficulty: str = field(init=False)
	player1Type: str = field(init=False)
	player1Colour: str = (0, 0, 0)
	player1Name: str = "BLACK"
	player2Type: str = field(init=False)
	player2Colour: str = (255, 255, 255)
	player2Name: str = "WHITE"
	humanName: str = "Human"
	aiName: str = "AI"
	aiType: str = "minmax"
	noDoubleThrees: bool = field(init=False)


@dataclass
class Config:
	colour: ColourConfig = field(default_factory=ColourConfig)
	display: DisplayConfig = field(default_factory=DisplayConfig)
	board: BoardConfig = field(default_factory=BoardConfig)
	stone: StoneConfig = field(default_factory=StoneConfig)
	game: GameConfig = field(default_factory=GameConfig)

	def __post_init__(self):
		self.board.width = self.display.width - self.board.offsetLeft - self.board.offsetRight
		self.board.height = self.display.height - self.board.offsetTop - self.board.offsetBottom
