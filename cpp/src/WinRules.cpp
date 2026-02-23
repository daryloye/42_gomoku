#include "Gomoku.hpp"

static const int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

int count_x_in_a_row(Coord move, Stone colour, const Grid &grid) {
  int best = 1;

  for (auto &d : directions) {
    int dy = d[0];
    int dx = d[1];

    int count = 1;

    for (int yy = move.y + dy, xx = move.x + dx;
         yy >= 0 && yy < BOARD_SIZE && xx >= 0 && xx < BOARD_SIZE &&
         grid[yy][xx] == colour;
         yy += dy, xx += dx) {
      count++;
    }

    for (int yy = move.y - dy, xx = move.x - dx;
         yy >= 0 && yy < BOARD_SIZE && xx >= 0 && xx < BOARD_SIZE &&
         grid[yy][xx] == colour;
         yy -= dy, xx -= dx) {
      count++;
    }

    best = std::max(count, best);
  }

  return best;
}

bool hasPlayerWon(Coord move, Stone colour, const Grid &grid) {
  return count_x_in_a_row(move, colour, grid) >= 5;
}

bool isThreatDetected(Coord move, Stone colour, const Grid &grid) {
  return count_x_in_a_row(move, colour, grid) >= 3;
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

  Stone opponent = (colour == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
  int capturedCount = 0;

  for (const auto &dir : directions) {
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

static bool isInBounds(Coord c) {
  return c.x >= 0 && c.x < BOARD_SIZE && c.y >= 0 && c.y < BOARD_SIZE;
}

bool createsDoubleThree(Coord move, Stone colour, const Grid &grid) {
  if (!isValidMove(move, grid))
    return false;

  int freeThreeCount = 0;

  for (const auto &dir : directions) {
    int dx = dir[0];
    int dy = dir[1];

    int countForward = 0;
    int countBackward = 0;

    for (int i = 1; i < 4; i++) {
      Coord forward = {move.x + i * dx, move.y + i * dy};
      if (isInBounds(forward) && grid[forward.y][forward.x] == colour)
        countForward++;
      else
        break;
    }

    for (int i = 1; i < 4; i++) {
      Coord backward = {move.x - i * dx, move.y - i * dy};
      if (isInBounds(backward) && grid[backward.y][backward.x] == colour)
        countBackward++;
      else
        break;
    }

    int totalCount = 1 + countForward + countBackward;

    if (totalCount >= 3) {
      Coord endForward = {move.x + (countForward + 1) * dx,
                          move.y + (countForward + 1) * dy};
      Coord endBackward = {move.x - (countBackward + 1) * dx,
                           move.y - (countBackward + 1) * dy};

      bool forwardOpen = !isInBounds(endForward) ||
                         grid[endForward.y][endForward.x] == Stone::EMPTY;
      bool backwardOpen = !isInBounds(endBackward) ||
                          grid[endBackward.y][endBackward.x] == Stone::EMPTY;

      if (totalCount == 3 && forwardOpen && backwardOpen)
        freeThreeCount++;
    }
  }

  return freeThreeCount >= 2;
}

bool wouldMoveIntoCapture(Coord cell, Stone colour, const Grid &grid) {
  Stone opponent = (colour == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
  const int dirs[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

  for (const auto &dir : dirs) {
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

  for (auto &d : directions) {
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

  Stone opponent = (colour == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;

  for (const Coord &pos : fivePositions) {
    for (const auto &dir : directions) {
      int dx = dir[0];
      int dy = dir[1];

      Coord far = {pos.x + dx, pos.y + dy};
      Coord outer = {pos.x + 2 * dx, pos.y + 2 * dy};
      Coord flank = {pos.x - dx, pos.y - dy};

      if (!isInBounds(far) || grid[far.y][far.x] != colour)
        continue;

      if (!isInBounds(flank) || !isInBounds(outer))
        continue;

      if (grid[flank.y][flank.x] == opponent &&
          grid[outer.y][outer.x] == Stone::EMPTY)
        return true;

      if (grid[flank.y][flank.x] == Stone::EMPTY &&
          grid[outer.y][outer.x] == opponent)
        return true;
    }
  }

  return false;
}
