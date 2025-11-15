#ifndef GOMOKU_HPP
#define GOMOKU_HPP

#include <iostream>
#include <array>
#include <vector>
#include <limits>
#include <algorithm>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <cstdio>

// https://www.fltk.org/doc-1.4/intro.html
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>


// Config
constexpr int BOARD_SIZE = 19;
constexpr int CELL_SIZE = 40;
constexpr int OFFSET = 20;
constexpr int WIN_SIZE = BOARD_SIZE * CELL_SIZE;


// Types & Constants
constexpr float POS_INFINITY = std::numeric_limits<float>::infinity();
constexpr float NEG_INFINITY = -std::numeric_limits<float>::infinity();

enum class Stone
{
  EMPTY = 0,
  BLACK = 1,
  WHITE = 2,
  OUTLINE = 3,
};

struct Coord {
  int x;
  int y;
};

struct MinimaxResult {
  float score;
  Coord move;
};


// https://www.fltk.org/doc-1.4/classFl__Window.html
class GomokuBoard : public Fl_Window
{
	public:
		GomokuBoard();
		~GomokuBoard();

		void draw() override;
		int handle(int event) override;

	private:
		std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE> grid;

		Stone currentPlayer = Stone::BLACK;
		Stone winner = Stone::EMPTY;
		Coord previousOutlineCell = {-1, -1};

		Stone getStone(Coord cell) const;
		void setStone(Coord cell, Stone p);
		bool isValidMove(Coord cell) const;
		bool checkWin(Coord cell, Stone stone) const;
};


// Win Rules
bool hasPlayerWon(Coord move, Stone colour, const std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE>& grid);


// Minimax
MinimaxResult minimax(
  const std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE>& grid,
  Coord last_move,
  Stone current_colour,
  int depth,
  bool isAiTurn,
	float alpha = NEG_INFINITY,
	float beta = POS_INFINITY
);


// Utils
Coord windowToBoardCoordinates(Coord windowCoords);
std::string stoneColourToString(Stone stone);
std::string coordToString(Coord coord);

#endif
