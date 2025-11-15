#ifndef CONFIG_HPP
#define CONFIG_HPP

constexpr int BOARD_SIZE = 19;
constexpr int CELL_SIZE = 40;
constexpr int OFFSET = 20;
constexpr int WIN_SIZE = BOARD_SIZE * CELL_SIZE;

enum class Stone
{
  EMPTY = 0,
  BLACK = 1,
  WHITE = 2,
  OUTLINE = 3,
};



#endif
