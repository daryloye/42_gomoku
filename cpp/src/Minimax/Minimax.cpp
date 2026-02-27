#include "Gomoku.hpp"

Minimax::Minimax(const Stone aiColour, const Stone opponentColour)
    : _aiColour(aiColour), _opponentColour(opponentColour) {
  resetHeatmap();
}

Minimax::~Minimax() {}

// Minimax algorithm: AI wants to maximize its score, opponent want to minimize
// AI's score alpha-beta pruning:
// 	- alpha: best score for ai so far
// 	- beta: best score for opponent so far
MinimaxResult Minimax::minimax(const Grid &grid, Coord lastMove, int depth,
                               Stone currentColour, Stone prevColour,
                               float alpha, float beta) {

  // Check if previous move has won
  if (isWinningMove(grid, lastMove, prevColour)) {
    std::cout << "player has won" << std::endl;
    return {(prevColour == _aiColour) ? WIN_SCORE : -WIN_SCORE, lastMove};
  }

  // Depth cutoff
  if (depth == 0) {
    return {evaluateMove(grid, lastMove), lastMove};
  }

  std::vector<Coord> moves = getPossibleMoves(grid);
  if (moves.empty()) {
    std::cout << "empty moves" << std::endl;
    return {evaluateMove(grid, lastMove), lastMove};
  }
  
  MinimaxResult best = (currentColour == _aiColour)
                           ? MinimaxResult{NEG_INFINITY, {}}
                           : MinimaxResult{POS_INFINITY, {}};

  // Check for obvious winning moves
  for (Coord move : moves) {

    
    auto newGrid = grid;
    newGrid[move.y][move.x] = currentColour;
    if (isWinningMove(newGrid, move, currentColour)) {
      std::cout << "found a winning move" << std::endl;
      return {(currentColour == _aiColour) ? WIN_SCORE : -WIN_SCORE, move};
    }
  }
  // std::cout << "Thinking about " << std::endl;

  // Check for obvious blocking moves (ie if the other colour made the move will it win)
  std::vector<Coord> blockingMoves;
  for (Coord move : moves) {

    auto newGrid = grid;
    newGrid[move.y][move.x] = prevColour;
    if (isWinningMove(newGrid, move, prevColour)) {
      // std::cout << depth << " need to block" << move << std::endl;
      blockingMoves.push_back(move);
    }
  }

  // Focus on evaluating only immediate threats if they exist
  if (!blockingMoves.empty()) {
    moves = blockingMoves;
  }

  // Next level -> swap colours
  for (Coord move : moves) {

    auto newGrid = grid;
    newGrid[move.y][move.x] = currentColour;
    MinimaxResult result = minimax(newGrid, move, depth - 1, prevColour,
                                   currentColour, alpha, beta);

    updateAlphaBeta(result, move, currentColour, best, alpha, beta);
    if (beta <= alpha)
      break;
  }

  return best;
}

void Minimax::updateAlphaBeta(const MinimaxResult& result, const Coord& move,
                              Stone currentColour, MinimaxResult& best,
                              float& alpha, float& beta)
{
  if (currentColour == _aiColour) {
    if (result.score > best.score)
      best = {result.score, move};
    alpha = std::max(alpha, best.score);
  } else {
    if (result.score < best.score)
      best = {result.score, move};
    beta = std::min(beta, best.score);
  }
}
