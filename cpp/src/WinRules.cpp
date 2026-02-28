#include "Gomoku.hpp"

static int countInOneDirection(const Grid& grid, const Coord& coord, int dx, int dy, Stone stone) {
  int count = 0;
  int nx = coord.x + dx;
  int ny = coord.y + dy;
  
  while (isInBounds({nx, ny}) && grid[ny][nx] == stone) {
    count++;
    nx += dx;
    ny += dy;
  }
  return count;
}

static bool isFiveInARow(const Grid& grid, const Coord& coord, Stone stone) {
  for (const auto &dir : DIRECTIONS) {
    int dx = dir[0];
    int dy = dir[1];
    int a = countInOneDirection(grid, coord, dx, dy, stone);
    int b = countInOneDirection(grid, coord, -dx, -dy, stone);
    if (1 + a + b >= 5) return true;
  }
  return false;
}



bool hasPlayerWon(const Coord& move, Stone stone, const Grid &grid) {
  return isFiveInARow(grid, move, stone);
}

bool isValidMove(Coord cell, const Grid &grid) {
  if (cell.x < 0 || cell.x >= BOARD_SIZE || cell.y < 0 || cell.y >= BOARD_SIZE)
    return false;

  return grid[cell.y][cell.x] != Stone::BLACK &&
         grid[cell.y][cell.x] != Stone::WHITE;
}

int countCapturedPairs(Coord move, Stone colour, const Grid &grid) {
  if (!isValidMove(move, grid))
    return 0;

  Stone opponent = opponentOf(colour);
  int capturedCount = 0;

  for (const auto &dir : DIRECTIONS) {
    int dx = dir[0];
    int dy = dir[1];

    int py1 = move.y + dy, px1 = move.x + dx;
    int py2 = move.y + 2 * dy, px2 = move.x + 2 * dx;
    int py3 = move.y + 3 * dy, px3 = move.x + 3 * dx;

    if (py1 >= 0 && py1 < BOARD_SIZE && px1 >= 0 && px1 < BOARD_SIZE &&
        py2 >= 0 && py2 < BOARD_SIZE && px2 >= 0 && px2 < BOARD_SIZE &&
        py3 >= 0 && py3 < BOARD_SIZE && px3 >= 0 && px3 < BOARD_SIZE) {
      if (grid[py1][px1] == opponent && grid[py2][px2] == opponent &&
          grid[py3][px3] == colour) {
        capturedCount++;
      }
    }

    int ny1 = move.y - dy, nx1 = move.x - dx;
    int ny2 = move.y - 2 * dy, nx2 = move.x - 2 * dx;
    int ny3 = move.y - 3 * dy, nx3 = move.x - 3 * dx;

    if (ny1 >= 0 && ny1 < BOARD_SIZE && nx1 >= 0 && nx1 < BOARD_SIZE &&
        ny2 >= 0 && ny2 < BOARD_SIZE && nx2 >= 0 && nx2 < BOARD_SIZE &&
        ny3 >= 0 && ny3 < BOARD_SIZE && nx3 >= 0 && nx3 < BOARD_SIZE) {
      if (grid[ny1][nx1] == opponent && grid[ny2][nx2] == opponent &&
          grid[ny3][nx3] == colour) {
        capturedCount++;
      }
    }
  }

  return capturedCount;
}



bool createsDoubleThree(Coord move, Stone colour, const Grid &grid) {
  if (!isValidMove(move, grid))
    return false;

  Grid tmp = grid;
  tmp[move.y][move.x] = colour;

  int freeThreeCount = 0;

  for (const auto &dir : DIRECTIONS) {
    int dx = dir[0];
    int dy = dir[1];

    bool foundInDirection = false;
    for (int k = 1; k <= 4 && !foundInDirection; k++) {
      // c0 is k steps behind move; c5 is (5-k) steps ahead.
      Coord c0 = {move.x - k * dx, move.y - k * dy};
      Coord c5 = {move.x + (5 - k) * dx, move.y + (5 - k) * dy};

      if (!isInBounds(c0) || tmp[c0.y][c0.x] != Stone::EMPTY)
        continue;
      if (!isInBounds(c5) || tmp[c5.y][c5.x] != Stone::EMPTY)
        continue;

      int colourCount = 0;
      int emptyCount = 0;
      bool blocked = false;

      for (int i = 1; i <= 4; i++) {
        Coord ci = {move.x + (i - k) * dx, move.y + (i - k) * dy};
        if (!isInBounds(ci)) { blocked = true; break; }
        Stone s = tmp[ci.y][ci.x];
        if (s == colour)
          colourCount++;
        else if (s == Stone::EMPTY)
          emptyCount++;
        else {
          blocked = true;
          break;
        }
      }

      if (!blocked && colourCount == 3 && emptyCount == 1)
        foundInDirection = true;
    }

    if (foundInDirection)
      freeThreeCount++;
  }

  return freeThreeCount >= 2;
}

bool wouldMoveIntoCapture(Coord cell, Stone colour, const Grid &grid) {
  Stone opponent = opponentOf(colour);

  for (const auto &dir : DIRECTIONS) {
    int dx = dir[0];
    int dy = dir[1];

    {
      Coord b1 = {cell.x - dx, cell.y - dy};
      Coord f1 = {cell.x + dx, cell.y + dy};
      Coord f2 = {cell.x + 2 * dx, cell.y + 2 * dy};
      if (isInBounds(b1) && isInBounds(f1) && isInBounds(f2) &&
          grid[b1.y][b1.x] == opponent && grid[f1.y][f1.x] == colour &&
          grid[f2.y][f2.x] == opponent)
        return true;
    }

    {
      Coord b2 = {cell.x - 2 * dx, cell.y - 2 * dy};
      Coord b1 = {cell.x - dx, cell.y - dy};
      Coord f1 = {cell.x + dx, cell.y + dy};
      if (isInBounds(b2) && isInBounds(b1) && isInBounds(f1) &&
          grid[b2.y][b2.x] == opponent && grid[b1.y][b1.x] == colour &&
          grid[f1.y][f1.x] == opponent)
        return true;
    }
  }

  return false;
}

std::vector<Coord> getFiveInARowPositions(Coord move, Stone colour,
                                          const Grid &grid) {
  std::vector<Coord> positions;

  for (auto &d : DIRECTIONS) {
    int dy = d[0];
    int dx = d[1];
    std::vector<Coord> linePositions;

    linePositions.push_back(move);

    for (int yy = move.y + dy, xx = move.x + dx;
         yy >= 0 && yy < BOARD_SIZE && xx >= 0 && xx < BOARD_SIZE &&
         grid[yy][xx] == colour;
         yy += dy, xx += dx) {
      linePositions.push_back({xx, yy});
    }

    for (int yy = move.y - dy, xx = move.x - dx;
         yy >= 0 && yy < BOARD_SIZE && xx >= 0 && xx < BOARD_SIZE &&
         grid[yy][xx] == colour;
         yy -= dy, xx -= dx) {
      linePositions.push_back({xx, yy});
    }

    if (linePositions.size() >= 5) {
      return linePositions;
    }
  }

  return positions;
}

bool canOpponentBreakFiveByCapture(Coord move, Stone colour, const Grid &grid) {
  std::vector<Coord> fivePositions = getFiveInARowPositions(move, colour, grid);

  if (fivePositions.empty()) {
    return false;
  }

  Stone opponent = opponentOf(colour);

  for (const Coord &pos : fivePositions) {
    for (const auto &dir : DIRECTIONS) {
      int dx = dir[0];
      int dy = dir[1];

      Coord far = {pos.x + dx, pos.y + dy};
      Coord outer = {pos.x + 2 * dx, pos.y + 2 * dy};
      Coord flank = {pos.x - dx, pos.y - dy};

      if (!isInBounds(far) || grid[far.y][far.x] != colour)
        continue;

      if (!isInBounds(flank) || !isInBounds(outer))
        continue;

      bool canCapture = (grid[flank.y][flank.x] == opponent &&
                         grid[outer.y][outer.x] == Stone::EMPTY) ||
                        (grid[flank.y][flank.x] == Stone::EMPTY &&
                         grid[outer.y][outer.x] == opponent);

      if (!canCapture)
        continue;
      Grid tmpGrid = grid;
      tmpGrid[pos.y][pos.x] = Stone::EMPTY;
      tmpGrid[far.y][far.x] = Stone::EMPTY;

      bool fiveStillExists = std::any_of(
          fivePositions.begin(), fivePositions.end(), [&](const Coord &check) {
            return check != pos && check != far &&
                   hasPlayerWon(check, colour, tmpGrid);
          });

      if (!fiveStillExists)
        return true;
    }
  }

  return false;
}
