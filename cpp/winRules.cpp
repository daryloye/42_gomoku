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

    for (int yy = move.y + dy, xx = move.x + dx; grid[yy][xx] == colour; yy += dy, xx += dx) {
      count++;
    }

    for (int yy = move.y - dy, xx = move.x - dx; grid[yy][xx] == colour; yy -= dy, xx -= dx) {
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

bool isValidMove(Coord cell, const Grid& grid)
{
  // check out of board area
  if (cell.x < 0 || cell.x >= BOARD_SIZE || cell.y < 0 || cell.y >= BOARD_SIZE)
    return false;

  // check another stone present
  if (grid[cell.y][cell.x] == Stone::BLACK || grid[cell.y][cell.x] == Stone::WHITE )
    return false;

  return true;
}
