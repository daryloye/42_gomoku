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
                               int alpha, int beta) {

  if (lastMove.x > 0 && hasPlayerWon(lastMove, prevColour, grid)) {
    return {(prevColour == _aiColour) ? INT_MAX : INT_MIN, lastMove};
  }

  std::vector<Coord> moves = getPossibleMoves(grid);
  // for (Coord move : moves) {
  //   _evaluationCount[move.y][move.x]++;
  // }

  // AI seems to be prioritising making threats of 3-4 in a row, but does not go
  // for wins or prevent opp from making wins
  if (currentColour == _aiColour) {
    for (Coord move : moves) {
      auto tmp = grid;
      tmp[move.y][move.x] = _aiColour;
      if (hasPlayerWon(move, _aiColour, tmp))
        return {1.0e+10f, move};
      if (isThreatDetected(move, _aiColour, tmp))
        return {1.0e+9f, move};
    }
  } else {
    for (Coord move : moves) {
      auto tmp = grid;
      tmp[move.y][move.x] = _opponentColour;
      if (hasPlayerWon(move, _opponentColour, tmp))
        return {-1.0e+10f, move};
      if (isThreatDetected(move, _opponentColour, tmp))
        return {-1.0e+9f, move};
    }
  }

  if (depth == 0 || moves.empty()) {
    float score = evaluateMove(grid);
    return {score, lastMove};
  }

  MinimaxResult best = (currentColour == _aiColour)
                           ? MinimaxResult{-INFINITY, {}}
                           : MinimaxResult{INFINITY, {}};

  for (Coord move : moves) {
    auto new_grid = grid;
    new_grid[move.y][move.x] = currentColour;

    // Next level -> swap colours
    MinimaxResult result = minimax(new_grid, move, depth - 1, prevColour,
                                   currentColour, alpha, beta);

    if (currentColour == _aiColour) {
      if (result.score > best.score)
        best = result;
      alpha = std::max(alpha, best.score);
    } else {
      if (result.score < best.score)
        best = result;
      beta = std::min(beta, best.score);
    }

    if (beta <= alpha)
      break;
  }

  return best;
}

const int Minimax::directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                                       {0, 1},   {1, -1}, {1, 0},  {1, 1}};

bool Minimax::hasOccupiedNeighbour(Coord cell, const Grid &grid) {
  for (auto &d : directions) {
    int dy = d[0];
    int dx = d[1];

    if (cell.y + dy < 0 || cell.y + dy >= BOARD_SIZE || cell.x + dx < 0 ||
        cell.x + dx >= BOARD_SIZE)
      continue;

    if (grid[cell.y + dy][cell.x + dx] == Stone::BLACK ||
        grid[cell.y + dy][cell.x + dx] == Stone::WHITE) {
      return true;
    }
  }
  return false;
}

std::vector<Coord> Minimax::getPossibleMoves(const Grid &grid) {
  std::vector<Coord> ret = {};

  for (int y = 0; y < BOARD_SIZE; y++) {
    for (int x = 0; x < BOARD_SIZE; x++) {
      if (grid[y][x] == Stone::EMPTY && hasOccupiedNeighbour({x, y}, grid) &&
          isValidMove({x, y}, grid)) {
        ret.push_back({x, y});
      }
    }
  }

  return ret;
}

float Minimax::evaluateMove(const Grid &grid) {
  int aiBest = 0;
  int oppBest = 0;
  for (int y = 0; y < BOARD_SIZE; y++) {
    for (int x = 0; x < BOARD_SIZE; x++) {
      if (grid[y][x] == _aiColour) {
        int count_in_a_row = count_x_in_a_row({x, y}, _aiColour, grid);
        aiBest = std::max(aiBest, count_in_a_row);
      }
      if (grid[y][x] == _opponentColour) {
        int count_in_a_row = count_x_in_a_row({x, y}, _opponentColour, grid);
        oppBest = std::max(oppBest, count_in_a_row);
      }
    }
  }

  float aiScore = 0;
  switch (aiBest) {
  case 0:
    aiScore = 0;
    break;
  case 1:
    aiScore = 1;
    break;
  case 2:
    aiScore = 10;
    break;
  case 3:
    aiScore = 100;
    break;
  case 4:
    aiScore = 1000;
    break;
  default:
    aiScore = 10000;
    break;
  }

  float oppScore = 0;
  switch (oppBest) {
  case 0:
    oppScore = 0;
    break;
  case 1:
    oppScore = 1;
    break;
  case 2:
    oppScore = 10;
    break;
  case 3:
    oppScore = 100;
    break;
  case 4:
    oppScore = 1000;
    break;
  default:
    oppScore = 10000;
    break;
  }

  return (aiScore - 2 * oppScore);
}
