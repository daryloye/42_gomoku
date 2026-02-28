#ifndef WINRULES_HPP
#define WINRULES_HPP

bool hasPlayerWon(const Coord& move, Stone stone, const Grid &grid);
bool isValidMove(Coord cell, const Grid &grid);
int countCapturedPairs(Coord move, Stone colour, const Grid &grid);
bool createsDoubleThree(Coord move, Stone colour, const Grid &grid);
bool canOpponentBreakFiveByCapture(Coord move, Stone colour, const Grid &grid);
bool wouldMoveIntoCapture(Coord cell, Stone colour, const Grid &grid);
std::vector<Coord> getFiveInARowPositions(Coord move, Stone colour,
                                          const Grid &grid);

#endif