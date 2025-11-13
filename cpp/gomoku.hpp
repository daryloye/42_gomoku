#ifndef GOMOKU_HPP
#define GOMOKU_HPP

#include <array>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>

constexpr int BOARD_SIZE = 19;
constexpr int CELL_SIZE = 40;
constexpr int OFFSET = 20;
constexpr int WIN_SIZE = BOARD_SIZE * CELL_SIZE + OFFSET;

enum class Player { EMPTY = 0, BLACK_STONE = 1, WHITE_STONE = 2 };

class GomokuBoard : public Fl_Window {
public:
	GomokuBoard();
	~GomokuBoard();

	void draw() override;
	int handle(int event) override;

private:
	std::array<std::array<Player, BOARD_SIZE>, BOARD_SIZE> grid;
	Player currentPlayer;

	Player getCell(int x, int y) const;
	void setCell(int x, int y, Player p);
	bool isValidMove(int x, int y) const;
	int screenToBoard(int coord);
};

#endif
