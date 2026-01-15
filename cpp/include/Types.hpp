#ifndef TYPES_HPP
#define TYPES_HPP

#include "Config.hpp"

constexpr float POS_INFINITY = std::numeric_limits<float>::infinity();
constexpr float NEG_INFINITY = -std::numeric_limits<float>::infinity();

enum class Stone {
  EMPTY = 0,
  BLACK = 1,
  WHITE = 2,
  OUTLINE = 3,
};

enum class GameMode {
  TWO_PLAYER = 0,
  AI_VS_HUMAN = 1,
};

struct GameRules {
  bool capturesEnabled = true;
  bool doubleThreeEnabled = true;
};

struct Coord {
  int x;
  int y;
};

typedef std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> CountGrid;
typedef std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE> Grid;

struct MinimaxResult {
  float score;
  Coord move;
};

struct MoveRecord {
  Coord move;
  Stone player;
  Grid boardState;
  int blackCapturedBefore;
  int whiteCapturedBefore;
  float blackTimeAtMove;
  float whiteTimeAtMove;
};

#endif
