from config import BoardConfig


def tile_to_coord(tile: tuple[int, int], board: BoardConfig):
	"""convert goban (tile_x, tile_y) to screen (x, y) coordinates"""
	tile_x, tile_y = tile

	x_offset = board.width / (board.size - 1)
	y_offset = board.height / (board.size - 1)

	x = board.offsetLeft + (tile_x * x_offset)
	y = board.offsetTop + (tile_y * y_offset)

	return ((x, y))


def coord_to_nearest_tile(coord: tuple[int, int], board: BoardConfig):
	"""convert screen (x, y) coordinates to the nearest goban tile (tile_x, tile_y)"""
	x, y = coord

	x_offset = board.width / (board.size - 1)
	y_offset = board.height / (board.size - 1)

	tile_x = round((x - board.offsetLeft) / x_offset)
	tile_x = max(0, min(tile_x, board.size - 1))

	tile_y = round((y - board.offsetTop) / y_offset)
	tile_y = max(0, min(tile_y, board.size - 1))

	return ((tile_x, tile_y))


def getOpposingColour(cfg, colour):
	return (
		cfg.game.player1Colour if colour == cfg.game.player2Colour else cfg.game.player1Colour
	)