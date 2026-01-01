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

enum class OpeningRule {
  STANDARD = 0,
  PRO = 1,
  SWAP = 2,
  SWAP2 = 3
};

struct GameRules {
  bool capturesEnabled = true;
  bool doubleThreeEnabled = true;
  OpeningRule openingRule = OpeningRule::STANDARD;

  bool swapOffered = false;
  bool swap2FirstPhase = false;
  int swap2StonesPlaced = 0;
};

struct Coord {
  int x;
  int y;
};

struct MinimaxResult {
  float score;
  Coord move;
};


typedef std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> CountGrid;
typedef std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE> Grid;

#endif
