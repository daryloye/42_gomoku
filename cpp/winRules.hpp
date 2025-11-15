#ifndef WINRULES_HPP
#define WINRULES_HPP

#include <array>
#include "config.hpp"
#include "utils.hpp"

bool hasPlayerWon(Coord move, Stone colour, const std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE>& grid);

#endif