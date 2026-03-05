#ifndef UTILS_HPP
#define UTILS_HPP

Coord windowToBoardCoordinates(Coord windowCoords);
std::ostream& operator<< (std::ostream& os, Stone stone);
std::ostream& operator<< (std::ostream& os, Coord coord);

constexpr int DIRECTIONS[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
Stone opponentOf(Stone colour);
bool isInBounds(Coord c);

#endif