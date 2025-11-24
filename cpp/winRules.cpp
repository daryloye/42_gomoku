#include "Gomoku.hpp"

static const int directions[4][2] = {
    {1, 0},
    {0, 1},
    {1, 1},
    {1, -1}
  };

int count_x_in_a_row(Coord move, Stone colour, const Grid& grid) {
  int best = 1;

  for (auto& d : directions) {
    int dy = d[0];
    int dx = d[1];

    int count = 1;

    for (int yy = move.y + dy, xx = move.x + dx;
         yy >= 0 && yy < BOARD_SIZE && xx >= 0 && xx < BOARD_SIZE && grid[yy][xx] == colour;
         yy += dy, xx += dx) {
      count++;
    }

    for (int yy = move.y - dy, xx = move.x - dx;
         yy >= 0 && yy < BOARD_SIZE && xx >= 0 && xx < BOARD_SIZE && grid[yy][xx] == colour;
         yy -= dy, xx -= dx) {
      count++;
    }

    best = std::max(count, best);
  }

  return best;
}

bool hasPlayerWon(Coord move, Stone colour, const Grid& grid) {
  if (count_x_in_a_row(move, colour, grid) >= 5)
    return true;

  return false;
}

bool isThreatDetected(Coord move, Stone colour, const Grid& grid) {
  if (count_x_in_a_row(move, colour, grid) >= 3)
    return true;

  return false;
}

bool isValidMove(Coord cell, const Grid& grid)
{
  if (cell.x < 0 || cell.x >= BOARD_SIZE || cell.y < 0 || cell.y >= BOARD_SIZE)
    return false;

  if (grid[cell.y][cell.x] == Stone::BLACK || grid[cell.y][cell.x] == Stone::WHITE )
    return false;

  return true;
}

int countCapturedPairs(Coord move, Stone colour, const Grid& grid)
{
  if (!isValidMove(move, grid))
    return 0;

  Stone opponent = (colour == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
  int capturedCount = 0;

  const int directions[4][2] = {
    {1, 0},   // horizontal
    {0, 1},   // vertical
    {1, 1},   // diagonal down-right
    {1, -1}   // diagonal down-left
  };

  for (const auto& dir : directions) {
    int dx = dir[0];
    int dy = dir[1];

    if (grid[move.y + dy][move.x + dx] == opponent &&
        grid[move.y + 2*dy][move.x + 2*dx] == opponent &&
        grid[move.y + 3*dy][move.x + 3*dx] == colour) {
      capturedCount++;
    }

    if (grid[move.y - dy][move.x - dx] == opponent &&
        grid[move.y - 2*dy][move.x - 2*dx] == opponent &&
        grid[move.y - 3*dy][move.x - 3*dx] == colour) {
      capturedCount++;
    }
  }

  return capturedCount;
}

static bool isInBounds(Coord c)
{
  return c.x >= 0 && c.x < BOARD_SIZE && c.y >= 0 && c.y < BOARD_SIZE;
}

bool createsDoubleThree(Coord move, Stone colour, const Grid& grid)
{
  if (!isValidMove(move, grid))
    return false;

  int freeThreeCount = 0;
  const int directions[4][2] = {
    {1, 0},   // horizontal
    {0, 1},   // vertical
    {1, 1},   // diagonal down-right
    {1, -1}   // diagonal down-left
  };

  for (const auto& dir : directions) {
    int dx = dir[0];
    int dy = dir[1];

    int countForward = 0;
    int countBackward = 0;

    for (int i = 1; i < 4; i++) {
      Coord forward = {move.x + i*dx, move.y + i*dy};
      if (isInBounds(forward) && grid[forward.y][forward.x] == colour)
        countForward++;
      else
        break;
    }

    for (int i = 1; i < 4; i++) {
      Coord backward = {move.x - i*dx, move.y - i*dy};
      if (isInBounds(backward) && grid[backward.y][backward.x] == colour)
        countBackward++;
      else
        break;
    }

    int totalCount = 1 + countForward + countBackward;

    if (totalCount >= 3) {
      Coord endForward = {move.x + (countForward + 1)*dx, move.y + (countForward + 1)*dy};
      Coord endBackward = {move.x - (countBackward + 1)*dx, move.y - (countBackward + 1)*dy};

      bool forwardOpen = !isInBounds(endForward) || grid[endForward.y][endForward.x] == Stone::EMPTY;
      bool backwardOpen = !isInBounds(endBackward) || grid[endBackward.y][endBackward.x] == Stone::EMPTY;

      if (totalCount == 3 && forwardOpen && backwardOpen)
        freeThreeCount++;
    }
  }

  return freeThreeCount >= 2;
}
