#include "Gomoku.hpp"

static bool isBoardEmpty(const Grid& grid) {
  for (int y = 0; y < BOARD_SIZE; y++) {
    for (int x = 0; x < BOARD_SIZE; x++) {
      if (grid[y][x] != Stone::EMPTY)
        return false;
    }
  }
  return true;
}

std::vector<Coord> Minimax::getPossibleMoves(const Grid& grid) {
  // check if the board is empty
  if (isBoardEmpty(grid)) {
    std::vector<Coord> ret;
    ret.push_back({BOARD_SIZE / 2, BOARD_SIZE / 2});
    return ret;
  }

  // const int R = 2;
  const int R = 1;
  bool marked[BOARD_SIZE][BOARD_SIZE] = {false};
  std::vector<Coord> nearMoves;   // R = 1
  // std::vector<Coord> farMoves;    // R = 2

  // scan for occupied cells, then add neighbouring cells to the list
  for (int y = 0; y < BOARD_SIZE; y++) {
    for (int x = 0; x < BOARD_SIZE; x++) {
      
      if (grid[y][x] == Stone::EMPTY)
        continue;

      for (int dy = -R; dy <= R; dy++) {
        for (int dx = -R; dx <= R; dx++) {
          if (dx == 0 && dy == 0)
            continue;

          int ny = y + dy;
          int nx = x + dx;

          // check out of board limits
          if (ny < 0 || ny >= BOARD_SIZE || nx < 0 || nx >= BOARD_SIZE)
            continue;
          
          // check space is occupied
          if (grid[ny][nx] != Stone::EMPTY)
            continue;

          // check space is already in list
          if (marked[ny][nx])
            continue;

          marked[ny][nx] = true;

          // int dist = std::max(std::abs(dx), std::abs(dy));
          // if (dist == 1)
          nearMoves.push_back({nx, ny});
          // else
            // farMoves.push_back({nx, ny});
        }
      }
    }
  }

  // insert R=1 moves before R=2 to optimise for pruning
  // nearMoves.insert(nearMoves.end(), farMoves.begin(), farMoves.end());
  // std::cout << "returning moves" << std::endl;
  return nearMoves;
}
