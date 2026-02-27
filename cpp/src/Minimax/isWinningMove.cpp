#include "Gomoku.hpp"

static const int DIRS[4][2] = {
  {1, 0},  // horizontal
  {0, 1},  // vertical
  {1, 1},  // diag '\'
  {1, -1}  // diag '/'
};

static int countInOneDirection(const Grid& grid, const Coord& coord, int dx, int dy, Stone stone) {
  int count = 0;
  int nx = coord.x + dx;
  int ny = coord.y + dy;
  
  while (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
         grid[ny][nx] == stone) {
    count++;
    nx += dx;
    ny += dy;
  }
  return count;
}

static bool isFiveInARow(const Grid& grid, const Coord& coord, Stone stone) {
  for (int i = 0; i < 4; i++) {
    int dx = DIRS[i][0], dy = DIRS[i][1];
    int a = countInOneDirection(grid, coord, dx, dy, stone);
    int b = countInOneDirection(grid, coord, -dx, -dy, stone);
    if (1 + a + b >= 5) return true;
  }
  return false;
}

bool Minimax::isWinningMove(const Grid& grid, const Coord& move, Stone stone) {
  return isFiveInARow(grid, move, stone);
}