#ifndef GOMOKU_HPP
#define GOMOKU_HPP

#include <iostream>
#include <array>
#include <vector>
#include <limits>
#include <algorithm>
#include <string>
#include <chrono>
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
constexpr int TEXT_MARGIN = 40;
constexpr int BOARD_WIDTH = BOARD_SIZE * CELL_SIZE;
constexpr int BOARD_HEIGHT = BOARD_SIZE * CELL_SIZE;
constexpr int WIN_WIDTH = BOARD_WIDTH + OFFSET;
constexpr int WIN_HEIGHT = BOARD_HEIGHT + OFFSET * 2 + TEXT_MARGIN;


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

enum class GameMode
{
  TWO_PLAYER = 0,
  AI_VS_HUMAN = 1,
};

typedef std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE> Grid;

struct Coord {
  int x;
  int y;
};

struct MinimaxResult {
  float score;
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
		float totalBlackTime;
		float totalWhiteTime;
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

		MinimaxResult suggestedMove = {NEG_INFINITY, {-1, -1}};
		bool showSuggestion = false;

		void reset();
		Stone getStone(Coord cell) const;
		void setStone(Coord cell, Stone p);
		bool checkWin(Coord cell, Stone stone) const;
		void drawBoard();
		void drawUI();
		void drawModeButtons();
		void makeAIMove();
		bool clickedModeButton(int x, int y);
};

// Win Rules
int count_x_in_a_row(Coord move, Stone colour, const Grid& grid);
bool hasPlayerWon(Coord move, Stone colour, const std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE>& grid);
bool isValidMove(Coord cell, const Grid& grid);

// Minimax
class Minimax {
	public:
		Minimax();
		~Minimax();

		MinimaxResult minimax(
			const Grid& grid,
			Coord last_move,
			Stone current_colour,
			int depth,
			bool isAiTurn,
			float alpha = NEG_INFINITY,
			float beta = POS_INFINITY
		);
	
	private:
		static const int directions[8][2];

		bool hasOccupiedNeighbour(Coord cell, const Grid& grid);
		std::vector<Coord> getPossibleMoves(const Grid& grid);
		float evaluateMove(Coord move, Stone colour, const Grid& grid);
};

// Utils
Coord windowToBoardCoordinates(Coord windowCoords);
std::string stoneColourToString(Stone stone);
std::string coordToString(Coord coord);

#endif
