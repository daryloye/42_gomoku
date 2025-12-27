#ifndef TYPES_HPP
#define TYPES_HPP

#include "Config.hpp"

constexpr int INT_MAX = std::numeric_limits<int>::max();
constexpr int INT_MIN = std::numeric_limits<int>::min();

enum class Stone {
  EMPTY = 0,
  BLACK = 1,
  WHITE = 2,
  OUTLINE = 3,
};

enum class GameMode {
  TWO_PLAYER = 0,
  AI_VS_HUMAN = 1,
  AI_VS_AI = 2,
};

struct Coord {
  int x;
  int y;
};

struct MinimaxResult {
  int score;
  Coord move;
};


typedef std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> CountGrid;
typedef std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE> Grid;

#endif