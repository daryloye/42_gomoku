// #include "winRules.hpp"
// #include <iostream>

// static bool check_n_in_a_row(Coord move, Stone colour, const std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE>& grid, int n) {
//   for (int y = -1; y <= 1; y++) {
//     for (int x = -1; x <= 1; x++) {
//       std::cout << "checking: " << x << ", " << y << std::endl; 
//       if (y == 0 && x == 0)
//         continue;

//       int count = 0;
//       while (grid[move.y + y][move.x + x] == colour) {
//         std::cout << "while checking: " << x << ", " << y << std::endl; 
//         count++;
//       }
//       if (count == n)
//         return true;
//     }
//   }
//   return false;
// }

// bool hasPlayerWon(Coord move, Stone colour, const std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE>& grid) {
//   if (check_n_in_a_row(move, colour, grid, 5))
//     return true;

//   return false;
// }
