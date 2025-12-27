#ifndef UTILS_HPP
#define UTILS_HPP

Coord windowToBoardCoordinates(Coord windowCoords);
std::ostream& operator<< (std::ostream& os, Stone stone);
std::ostream& operator<< (std::ostream& os, Coord coord);

#endif