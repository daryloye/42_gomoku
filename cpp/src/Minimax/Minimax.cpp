#include "Gomoku.hpp"

Minimax::Minimax(const Stone aiColour, const Stone opponentColour)
    : _aiColour(aiColour), _opponentColour(opponentColour) {
  resetHeatmap();
}

Minimax::~Minimax() {}


static void updateAlphaBeta(const MinimaxResult& result, const Coord& move,
                              bool isCurrentColourAI, MinimaxResult& best,
                              float& alpha, float& beta)
{
  if (isCurrentColourAI) {
    if (result.score > best.score)
      best = {result.score, move};
    alpha = std::max(alpha, best.score);
  } else {
    if (result.score < best.score)
      best = {result.score, move};
    beta = std::min(beta, best.score);
  }
}

// Minimax algorithm: AI wants to maximize its score, opponent want to minimize
// AI's score alpha-beta pruning:
// 	- alpha: best score for ai so far
// 	- beta: best score for opponent so far
MinimaxResult Minimax::minimax(const Grid &grid, Coord lastMove, int depth,
                               Stone currentColour, Stone prevColour,
                               float alpha, float beta) {

  // Check if previous move has won
  if (hasPlayerWon(lastMove, prevColour, grid)) {
    std::cout << "player has won" << std::endl;
    return {(prevColour == _aiColour) ? WIN_SCORE : -WIN_SCORE, lastMove};
  }

  // Depth cutoff
  if (depth == 0) {
    return {evaluateMove(grid, lastMove), lastMove};
  }

  std::vector<Coord> moves = getPossibleMoves(grid);
  if (moves.empty()) {
    return {evaluateMove(grid, lastMove), lastMove};
  }
  
  MinimaxResult best = (currentColour == _aiColour)
                           ? MinimaxResult{NEG_INFINITY, {}}
                           : MinimaxResult{POS_INFINITY, {}};

  // Check for obvious winning moves
  for (Coord move : moves) {
    auto newGrid = grid;
    newGrid[move.y][move.x] = currentColour;
    if (hasPlayerWon(move, currentColour, newGrid)) {
      return {(currentColour == _aiColour) ? WIN_SCORE : -WIN_SCORE, move};
    }
  }

  // Check for obvious blocking moves (ie if the other colour made the move will it win)
  std::vector<Coord> blockingMoves;
  for (Coord move : moves) {
    auto newGrid = grid;
    newGrid[move.y][move.x] = prevColour;
    if (hasPlayerWon(move, prevColour, newGrid)) {
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

    updateAlphaBeta(result, move, (currentColour == _aiColour), best, alpha, beta);
    if (beta <= alpha)
      break;
  }

  return best;
}
