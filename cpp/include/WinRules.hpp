#ifndef WINRULES_HPP
#define WINRULES_HPP

int count_x_in_a_row(Coord move, Stone colour, const Grid &grid);
bool hasPlayerWon(Coord move, Stone colour, const Grid &grid);
bool isThreatDetected(Coord move, Stone colour, const Grid &grid);
bool isValidMove(Coord cell, const Grid &grid);
int countCapturedPairs(Coord move, Stone colour, const Grid &grid);
bool createsDoubleThree(Coord move, Stone colour, const Grid &grid);
bool canOpponentBreakFiveByCapture(Coord move, Stone colour, const Grid &grid);
std::vector<Coord> getFiveInARowPositions(Coord move, Stone colour,
                                          const Grid &grid);

#endif