from config import *

def tile_to_coord(tile):
	"""convert goban (tile_x, tile_y) to screen (x, y) coordinates"""
	tile_x, tile_y = tile
	x_offset = BOARD_WIDTH / (BOARD_GRID - 1)
	y_offset = BOARD_HEIGHT / (BOARD_GRID - 1)
	return ((BOARD_OFFSET + (tile_x * x_offset), BOARD_OFFSET + (tile_y * y_offset)))

	