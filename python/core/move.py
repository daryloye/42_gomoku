from dataclasses import dataclass

@dataclass
class Move:
	tile: tuple[int, int]
	colour: str