#include "Gomoku.hpp"

Coord windowToBoardCoordinates(Coord windowCoords) {
  int board_x = (windowCoords.x - OFFSET + CELL_SIZE / 2) / CELL_SIZE;
  int board_y = (windowCoords.y - TEXT_MARGIN - OFFSET + CELL_SIZE / 2) / CELL_SIZE;

  if (board_x < 0 || board_x >= BOARD_SIZE)
    board_x = -1;

  if (board_y < 0 || board_y >= BOARD_SIZE)
    board_y = -1;

  return Coord{board_x, board_y};
};

std::ostream& operator<< (std::ostream& os, Stone stone) {
  switch (stone) {
    case Stone::EMPTY:    os << "EMPTY";    break;
    case Stone::BLACK:    os << "BLACK";    break;
    case Stone::WHITE:    os << "WHITE";    break;
    case Stone::OUTLINE:  os << "OUTLINE";  break;
  }
  return os;
};

std::ostream& operator<< (std::ostream& os, Coord coord) {
  os << "(" << coord.x << ", " << coord.y << ")";
  return os;
}