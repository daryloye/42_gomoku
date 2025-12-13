#include "Gomoku.hpp"
#include <set>

Minimax::Minimax(const Stone aiColour, const Stone opponentColour)
    : _aiColour(aiColour), _opponentColour(opponentColour) {
  resetHeatmap();
}

Minimax::~Minimax() {}

// how many different evaluation criteria is required i wonder...
MinimaxResult Minimax::minimax(const Grid &grid, Coord lastMove, int depth,
                               Stone currentColour, Stone prevColour, int alpha,
                               int beta) {
  if (lastMove.x >= 0 && lastMove.y >= 0 &&
      hasPlayerWon(lastMove, prevColour, grid))
    return {(prevColour == _aiColour) ? 1000000000 : -1000000000, lastMove};

  std::vector<Coord> moves = getPossibleMoves(grid);
  for (Coord move : moves)
    _evaluationCount[move.y][move.x]++;

  if (currentColour == _aiColour) {
    for (Coord move : moves) {
      auto tmp = grid;
      tmp[move.y][move.x] = _aiColour;
      if (hasPlayerWon(move, _aiColour, tmp))
        return {1000000000, move};
    }
  } else {
    for (Coord move : moves) {
      auto tmp = grid;
      tmp[move.y][move.x] = _opponentColour;
      if (hasPlayerWon(move, _opponentColour, tmp))
        return {-1000000000, move};
    }
  }

  Coord opponentFourThreat = {-1, -1};
  for (Coord move : moves) {
    auto tmp = grid;
    Stone opponent = (currentColour == _aiColour) ? _opponentColour : _aiColour;
    tmp[move.y][move.x] = opponent;
    if (count_x_in_a_row(move, opponent, tmp) >= 4) {
      opponentFourThreat = move;
      break;
    }
  }

  if (opponentFourThreat.x != -1) {
    return {(currentColour == _aiColour) ? 500000000 : -500000000,
            opponentFourThreat};
  }

  for (Coord move : moves) {
    auto tmp = grid;
    tmp[move.y][move.x] = currentColour;
    if (count_x_in_a_row(move, currentColour, tmp) >= 4) {
      return {(currentColour == _aiColour) ? 300000000 : -300000000, move};
    }
  }

  if (depth == 0 || moves.empty()) {
    int score = evaluateMove(grid);
    return {score, lastMove};
  }

  MinimaxResult best = (currentColour == _aiColour)
                           ? MinimaxResult{-1000000000, {}}
                           : MinimaxResult{1000000000, {}};

  for (Coord move : moves) {
    auto new_grid = grid;
    new_grid[move.y][move.x] = currentColour;

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

  int stoneCount = 0;
  for (int y = 0; y < BOARD_SIZE; y++) {
    for (int x = 0; x < BOARD_SIZE; x++) {
      if (grid[y][x] == Stone::BLACK || grid[y][x] == Stone::WHITE) {
        stoneCount++;
      }
    }
  }

  if (stoneCount == 0) {
    ret.push_back({BOARD_SIZE / 2, BOARD_SIZE / 2});
    return ret;
  }

  std::set<std::pair<int, int>> candidates;
  for (int y = 0; y < BOARD_SIZE; y++) {
    for (int x = 0; x < BOARD_SIZE; x++) {
      if (grid[y][x] != Stone::EMPTY) {
        for (auto &d : directions) {
          int ny = y + d[0];
          int nx = x + d[1];
          if (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE &&
              grid[ny][nx] == Stone::EMPTY) {
            candidates.insert({nx, ny});
          }
        }
      }
    }
  }

  std::vector<Coord> allMoves;
  for (const auto &c : candidates) {
    if (isValidMove({c.first, c.second}, grid)) {
      allMoves.push_back({c.first, c.second});
    }
  }

  std::sort(allMoves.begin(), allMoves.end(), [&](Coord a, Coord b) {
    auto tmpA_def = grid;
    auto tmpB_def = grid;

    int opponentThreatA = 0;
    int opponentThreatB = 0;

    Stone testOpponent = Stone::WHITE;
    for (int y = 0; y < BOARD_SIZE && testOpponent == Stone::WHITE; y++) {
      for (int x = 0; x < BOARD_SIZE; x++) {
        if (grid[y][x] == Stone::BLACK) {
          testOpponent = Stone::WHITE;
          break;
        } else if (grid[y][x] == Stone::WHITE) {
          testOpponent = Stone::BLACK;
          break;
        }
      }
    }

    tmpA_def[a.y][a.x] = testOpponent;
    tmpB_def[b.y][b.x] = testOpponent;
    opponentThreatA = count_x_in_a_row(a, testOpponent, tmpA_def);
    opponentThreatB = count_x_in_a_row(b, testOpponent, tmpB_def);

    auto tmpA_off = grid;
    auto tmpB_off = grid;
    Stone us = (testOpponent == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
    tmpA_off[a.y][a.x] = us;
    tmpB_off[b.y][b.x] = us;
    int ourThreatA = count_x_in_a_row(a, us, tmpA_off);
    int ourThreatB = count_x_in_a_row(b, us, tmpB_off);

    int scoreA = opponentThreatA * 2000 + ourThreatA;
    int scoreB = opponentThreatB * 2000 + ourThreatB;

    return scoreA > scoreB;
  });

  int maxMoves = 7;
  if (stoneCount <= 2) {
    maxMoves = 3;
  } else if (stoneCount <= 6) {
    maxMoves = 5;
  } else if (stoneCount <= 12) {
    maxMoves = 6;
  }

  int count = 0;
  for (const auto &move : allMoves) {
    if (count >= maxMoves)
      break;
    ret.push_back(move);
    count++;
  }

  return ret;
}

int Minimax::countConsecutive(Coord pos, int dy, int dx, Stone colour,
                              const Grid &grid) {
  int count = 0;
  int y = pos.y;
  int x = pos.x;

  while (y >= 0 && y < BOARD_SIZE && x >= 0 && x < BOARD_SIZE &&
         grid[y][x] == colour) {
    count++;
    y += dy;
    x += dx;
  }

  return count;
}

void Minimax::evaluateDirection(Coord pos, int dy, int dx, Stone colour,
                                const Grid &grid, int &score) {
  int forward =
      countConsecutive({pos.x + dx, pos.y + dy}, dy, dx, colour, grid);
  int backward =
      countConsecutive({pos.x - dx, pos.y - dy}, -dy, -dx, colour, grid);
  int total = forward + backward + 1;

  if (total < 2)
    return;

  int forwardY = pos.y + (forward + 1) * dy;
  int forwardX = pos.x + (forward + 1) * dx;
  int backwardY = pos.y - (backward + 1) * dy;
  int backwardX = pos.x - (backward + 1) * dx;

  bool forwardOpen =
      (forwardY >= 0 && forwardY < BOARD_SIZE && forwardX >= 0 &&
       forwardX < BOARD_SIZE && grid[forwardY][forwardX] == Stone::EMPTY);
  bool backwardOpen =
      (backwardY >= 0 && backwardY < BOARD_SIZE && backwardX >= 0 &&
       backwardX < BOARD_SIZE && grid[backwardY][backwardX] == Stone::EMPTY);

  int openEnds = (forwardOpen ? 1 : 0) + (backwardOpen ? 1 : 0);

  if (total >= 5) {
    score += 100000;
  } else if (total == 4) {
    if (openEnds == 2)
      score += 10000;
    else if (openEnds == 1)
      score += 1000;
    else
      score += 100;
  } else if (total == 3) {
    if (openEnds == 2)
      score += 2000;
    else if (openEnds == 1)
      score += 200;
    else
      score += 20;
  } else if (total == 2) {
    if (openEnds == 2)
      score += 100;
    else if (openEnds == 1)
      score += 20;
  }

  if (total == 1) {
    if (openEnds == 2) {
      score += 5;
    }
  }
}

int Minimax::evaluateMove(const Grid &grid) {
  int aiScore = 0;
  int oppScore = 0;

  for (int y = 0; y < BOARD_SIZE; y++) {
    for (int x = 0; x < BOARD_SIZE; x++) {
      if (grid[y][x] == _aiColour) {
        evaluateDirection({x, y}, 0, 1, _aiColour, grid, aiScore);
        evaluateDirection({x, y}, 1, 0, _aiColour, grid, aiScore);
        evaluateDirection({x, y}, 1, 1, _aiColour, grid, aiScore);
        evaluateDirection({x, y}, 1, -1, _aiColour, grid, aiScore);
      } else if (grid[y][x] == _opponentColour) {
        evaluateDirection({x, y}, 0, 1, _opponentColour, grid, oppScore);
        evaluateDirection({x, y}, 1, 0, _opponentColour, grid, oppScore);
        evaluateDirection({x, y}, 1, 1, _opponentColour, grid, oppScore);
        evaluateDirection({x, y}, 1, -1, _opponentColour, grid, oppScore);
      }
    }
  }

  return aiScore - (oppScore * 10);
}
