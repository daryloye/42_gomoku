#include "Gomoku.hpp"

static std::vector<Coord> getPossibleMoves(const std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE>& grid) {
  std::vector<Coord> ret = {};

  for (int y = 0; y < BOARD_SIZE; y++) {
		for (int x = 0; x < BOARD_SIZE; x++) {
      if (grid[y][x] == Stone::EMPTY) {
        ret.push_back({x, y});
      }
    }
  }

  return ret;
}


MinimaxResult minimax(
  const std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE>& grid,
  Coord last_move,
  Stone current_colour,
  int depth,
  bool isAiTurn,
  float alpha,
  float beta
) {
  // Minimax algorithm: AI wants to maximize its score, opponent want to minimize AI's score
	// alpha-beta pruning: 
	// 	- alpha: best score for ai so far
	// 	- beta: best score for opponent so far

  std::vector<Coord> moves = getPossibleMoves(grid);
  
  // base case scenarios:
  // - depth == 0
  // - already won / lost
  // - board is full
  if (depth == 0 || moves.empty() ) {
    // evaluate move
    float score = 1;

    return {score, last_move};
  }
  
  Stone opponent_colour = (current_colour == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;

  MinimaxResult best = 
    (isAiTurn) 
      ? MinimaxResult{ NEG_INFINITY, {} }
      : MinimaxResult{ POS_INFINITY, {} };
  
  for (Coord move : moves) {
    auto new_grid = grid;
    new_grid[move.y][move.x] = current_colour;
    
    MinimaxResult result = minimax(new_grid, move, opponent_colour, depth - 1, !isAiTurn, alpha, beta);

    if (isAiTurn) {
      if (result.score > best.score) {
        best = result;
      }

      alpha = std::max(alpha, best.score);
      if (beta <= alpha)
        break;

    } else {
      if (result.score < best.score) {
        best = result;
      }

      beta = std::min(beta, best.score);
      if (beta <= alpha)
        break;
    }
  }

  return best;
}