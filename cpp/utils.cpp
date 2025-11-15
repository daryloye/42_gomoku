#include "Gomoku.hpp"

Coord windowToBoardCoordinates(Coord windowCoords) {
  int board_x = (windowCoords.x - OFFSET + CELL_SIZE / 2) / CELL_SIZE;
  int board_y = (windowCoords.y - OFFSET + CELL_SIZE / 2) / CELL_SIZE;
  
  if (board_x < 0 || board_x >= BOARD_SIZE)
    board_x = -1;

  if (board_y < 0 || board_y >= BOARD_SIZE)
    board_y = -1;

  return Coord{board_x, board_y};
};

std::string stoneColourToString(Stone stone) {
  switch (stone) {
    case Stone::EMPTY:
      return "EMPTY";
    case Stone::BLACK:
      return "BLACK";
    case Stone::WHITE:
      return "WHITE";
    case Stone::OUTLINE:
      return "OUTLINE";
  }
  return "";
};

std::string coordToString(Coord coord) {
  char buffer[100];
  sprintf(buffer, "(%d, %d)", coord.x, coord.y);
  return buffer;
}