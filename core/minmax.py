from config import Config


def minmax(state, depth, player_colour):
	"""
	state = stones.map
	action = available moves??

	to test: assign utility of each possible action based on
	how close it will bring the player to victory
	eg. 4 stones in a row = 0.4
	"""
	if is_terminal(state) or depth == 0:			# is_terminal --> found a winner?
		return (getScore(state), None)
	
	if player_colour == BLACK:
		max_score = -1
		best_move = None													# if -1, the min player wins
		for move in getPossibleMoves(state):									# actions(state) gets list of possible actions based on current state
			(score, move) = minmax(state.place(move, BLACK), depth - 1, WHITE)		# result(state, action) -> state
			if score > max_score:
				max_score = score
				best_move = move
		return (max_score, move)
	else:
		min_score = 1
		best_move = None
		for action in getPossibleMoves(state):
			(score, move) = minmax(state.place(move, WHITE), depth - 1, BLACK)
			if score < min_score:
				min_score = score
				best_move = move
		return (min_score, move)


def is_terminal(state):
	return False


def getScore(state):
	return 0


def getPossibleMoves(state):
	return list(state.allMoves - set(state.map))
