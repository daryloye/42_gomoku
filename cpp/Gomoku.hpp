#ifndef GOMOKU_HPP
#define GOMOKU_HPP

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <limits>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <vector>

// https://www.fltk.org/doc-1.4/intro.html
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

constexpr int BOARD_SIZE = 19;
constexpr int CELL_SIZE = 40;
constexpr int OFFSET = 20;
constexpr int TEXT_MARGIN = 60;
constexpr int BOTTOM_MARGIN = 50;
constexpr int BUTTON_PANEL_WIDTH = 120;
constexpr int BOARD_WIDTH = BOARD_SIZE * CELL_SIZE;
constexpr int BOARD_HEIGHT = BOARD_SIZE * CELL_SIZE;
constexpr int WIN_WIDTH = BOARD_WIDTH + OFFSET + BUTTON_PANEL_WIDTH;
constexpr int WIN_HEIGHT = BOARD_HEIGHT + TEXT_MARGIN + OFFSET + BOTTOM_MARGIN;

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
  AI_VS_AI = 2,
};

typedef std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE> Grid;
typedef std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> CountGrid;

struct Coord {
  int x;
  int y;
};

struct MinimaxResult {
  int score;
  Coord move;
};

class Timer {
public:
  Timer();
  ~Timer();

  void resetAll();
  void resetTimer();
  void calculateTimeSpentOnMove(Stone currentPlayer);

  std::chrono::steady_clock::time_point moveStartTime;
  float lastMoveTime;
  float lastBlackMoveTime;
  float lastWhiteMoveTime;
  float totalBlackTime;
  float totalWhiteTime;
};

// https://www.fltk.org/doc-1.4/classFl__Window.html
class GomokuBoard : public Fl_Window {
public:
  GomokuBoard();
  ~GomokuBoard();

  void draw() override;
  int handle(int event) override;

private:
  Timer timer;
  std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE> grid;

  Stone currentPlayer;
  Stone winner;
  Coord previousOutlineCell;

  bool fontsInitialized = false;

  int blackMoveCount;
  int whiteMoveCount;

  GameMode gameMode = GameMode::TWO_PLAYER;
  Stone aiColor = Stone::WHITE;
  bool isAiThinking = false;
  std::chrono::steady_clock::time_point aiThinkStartTime;
  float aiThinkTime = 0.0f;

  int blackCaptured = 0;
  int whiteCaptured = 0;

  Coord illegalMoveCell = {-1, -1};
  bool hasIllegalMove = false;
  std::array<bool, 4> illegalDirections = {false, false, false, false};

  MinimaxResult suggestedMove = {-1000000000, {-1, -1}};
  Stone suggestionForPlayer = Stone::EMPTY;
  bool showSuggestion = false;

  CountGrid lastEvaluationHeatmap;
  bool showHeatmap = false;
  bool heatmapNeedsRedraw = false;

  void reset();
  Stone getStone(Coord cell) const;
  void setStone(Coord cell, Stone p);
  bool checkWin(Coord cell, Stone stone) const;
  void drawBoard();
  void drawUI();
  void drawModeButtons();
  void makeAIMove();
  bool clickedModeButton(int x, int y);
  void analyzeDoubleThree(Coord move, Stone colour,
                          std::array<bool, 4> &directions);
  void updateSuggestion();
};

// Win Rules
int count_x_in_a_row(Coord move, Stone colour, const Grid &grid);
bool hasPlayerWon(Coord move, Stone colour, const Grid &grid);
bool isThreatDetected(Coord move, Stone colour, const Grid &grid);
bool isValidMove(Coord cell, const Grid &grid);
int countCapturedPairs(Coord move, Stone colour, const Grid &grid);
bool createsDoubleThree(Coord move, Stone colour, const Grid &grid);
bool canOpponentBreakFiveByCapture(Coord move, Stone colour, const Grid &grid);
std::vector<Coord> getFiveInARowPositions(Coord move, Stone colour,
                                          const Grid &grid);

// Minimax
class Minimax {
public:
  Minimax(const Stone aiColour, const Stone opponentColour);
  ~Minimax();

  MinimaxResult minimax(const Grid &grid, Coord lastMove, int depth,
                        Stone currentColour, Stone prevColour,
                        int alpha = -1000000000, int beta = 1000000000);

  const CountGrid &getEvaluationHeatmap() const { return _evaluationCount; }
  void resetHeatmap() {
    for (int y = 0; y < BOARD_SIZE; y++)
      for (int x = 0; x < BOARD_SIZE; x++)
        _evaluationCount[y][x] = 0;
  }

private:
  static const int directions[8][2];
  const Stone _aiColour;
  const Stone _opponentColour;
  CountGrid _evaluationCount;

  bool hasOccupiedNeighbour(Coord cell, const Grid &grid);
  std::vector<Coord> getPossibleMoves(const Grid &grid);
  int evaluateMove(const Grid &grid);
  int countConsecutive(Coord pos, int dy, int dx, Stone colour, const Grid &grid);
  void evaluateDirection(Coord pos, int dy, int dx, Stone colour, const Grid &grid, int &score);
};

// Utils
Coord windowToBoardCoordinates(Coord windowCoords);
std::string stoneColourToString(Stone stone);
std::string coordToString(Coord coord);

#endif
