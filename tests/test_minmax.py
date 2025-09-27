from utils.minmax import *
from ui.stones import Stones

def test_minmax():
	# blank map, depth is 0
	state = Stones()
	(score, move) = minmax(state, 0, BLACK)
	assert score == 0
	assert move == None

	# blank map, depth is 1
	state = Stones()
	(score, move) = minmax(state, 0, BLACK)
	assert score == 0
	assert move == None


def test_getPossibleMove():
	stones = Stones()
	state = stones.place((0,1), BLACK)
	moves = getPossibleMoves(state)
	assert len(moves) == 19 * 19 - 1
	assert (0, 0) in moves
	assert (18, 18) in moves
	assert (19, 19) not in moves
	assert (0, 1) not in moves


