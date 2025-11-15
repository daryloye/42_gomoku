#ifndef GOMOKUBOARD_HPP
#define GOMOKUBOARD_HPP

#include <array>
// https://www.fltk.org/doc-1.4/intro.html
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <iostream>

#include "utils.hpp"
#include "config.hpp"
#include "winRules.hpp"

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

#endif
