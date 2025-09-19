from config import *

def tile_to_coord(tile, board_size):
	"""convert goban (tile_x, tile_y) to screen (x, y) coordinates"""
	tile_x, tile_y = tile
	
	x_offset = BOARD_WIDTH / (board_size - 1)
	y_offset = BOARD_HEIGHT / (board_size - 1)
	
	x = BOARD_OFFSET + (tile_x * x_offset)
	y = BOARD_OFFSET + (tile_y * y_offset)
	
	return ((x, y))


def coord_to_nearest_tile(coord, board_size):
	"""convert screen (x, y) coordinates to the nearest goban tile (tile_x, tile_y)"""
	x, y = coord
	
	x_offset = BOARD_WIDTH / (board_size - 1)
	y_offset = BOARD_HEIGHT / (board_size - 1)
	
	tile_x = round((x - BOARD_OFFSET) / x_offset)
	tile_x = max(0, min(tile_x, board_size - 1))

	tile_y = round((y - BOARD_OFFSET) / y_offset)
	tile_y = max(0, min(tile_y, board_size - 1))
	
	return ((tile_x, tile_y))