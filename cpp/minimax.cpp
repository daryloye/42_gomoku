#include "Gomoku.hpp"

Minimax::Minimax() {}

Minimax::~Minimax() {}

MinimaxResult Minimax::minimax(
  const Grid& grid,
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

  if (depth == 0 || moves.empty() ) {
    float score = evaluateMove(last_move, current_colour, grid);
    score = (isAiTurn) ? -score : score;

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

const int Minimax::directions[8][2] = {
  {-1, -1},
  {-1, 0},
  {-1, 1},
  {0, -1},
  {0, 1},
  {1, -1},
  {1, 0},
  {1, 1}
};

bool Minimax::hasOccupiedNeighbour(Coord cell, const Grid& grid) {
  for (auto& d : directions) {
    int dy = d[0];
    int dx = d[1];

    if (cell.y + dy < 0 || cell.y + dy >= BOARD_SIZE || cell.x + dx < 0 || cell.x + dx >= BOARD_SIZE)
      continue;

    if (grid[cell.y + dy][cell.x + dx] == Stone::BLACK || grid[cell.y + dy][cell.x + dx] == Stone::WHITE) {
      return true;
    }
  }
  return false;
}

std::vector<Coord> Minimax::getPossibleMoves(const Grid& grid) {
  std::vector<Coord> ret = {};

  for (int y = 0; y < BOARD_SIZE; y++) {
    for (int x = 0; x < BOARD_SIZE; x++) {
      if (grid[y][x] == Stone::EMPTY && hasOccupiedNeighbour({y, x}, grid) && isValidMove({y, x}, grid)) {
        ret.push_back({x, y});
      }
    }
  }

  return ret;
}

float Minimax::evaluateMove(Coord move, Stone colour, const Grid& grid) {
  int count_in_a_row = count_x_in_a_row(move, colour, grid);
  switch (count_in_a_row) {
    case 0:
      return 0;
    case 1:
      return 1;
    case 2:
      return 10;
    case 3:
      return 100;
    case 4:
      return 1000;
    default:
      return POS_INFINITY;
  }
}

