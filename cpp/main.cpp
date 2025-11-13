#include "gomoku.hpp"
#include <FL/fl_draw.H>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

GomokuBoard::GomokuBoard() : Fl_Window(WIN_SIZE, WIN_SIZE, "Gomoku"), currentPlayer(Player::BLACK_STONE) {
	for (int y = 0; y < BOARD_SIZE; y++)
		for (int x = 0; x < BOARD_SIZE; x++)
			grid[y][x] = Player::EMPTY;
	end();
}

GomokuBoard::~GomokuBoard() {}

Player GomokuBoard::getCell(int x, int y) const {
	if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) return Player::EMPTY;
	return grid[y][x];
}

void GomokuBoard::setCell(int x, int y, Player p) {
	if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) grid[y][x] = p;
}

bool GomokuBoard::isValidMove(int x, int y) const {
	return getCell(x, y) == Player::EMPTY;
}

int GomokuBoard::screenToBoard(int coord) {
	int bc = (coord - OFFSET) / CELL_SIZE;
	return (bc < 0 || bc >= BOARD_SIZE) ? -1 : bc;
}

void GomokuBoard::draw() {
	fl_rectf(0, 0, w(), h(), 220, 200, 150);

	fl_color(60, 60, 60);
	fl_line_style(FL_SOLID, 2);
	for (int i = 0; i < BOARD_SIZE; i++) {
		int pos = OFFSET + i * CELL_SIZE;
		fl_line(pos, OFFSET, pos, OFFSET + (BOARD_SIZE - 1) * CELL_SIZE);
		fl_line(OFFSET, pos, OFFSET + (BOARD_SIZE - 1) * CELL_SIZE, pos);
	}

	for (int yy = 0; yy < BOARD_SIZE; yy++) {
		for (int xx = 0; xx < BOARD_SIZE; xx++) {
			Player cell = getCell(xx, yy);
			if (cell == Player::EMPTY) continue;
			int sx = OFFSET + xx * CELL_SIZE;
			int sy = OFFSET + yy * CELL_SIZE;
			int r = CELL_SIZE / 2 - 2;
			if (cell == Player::BLACK_STONE) fl_color(0, 0, 0);
			else fl_color(255, 255, 255);
			fl_pie(sx - r, sy - r, r * 2, r * 2, 0, 360);
		}
	}
}

int GomokuBoard::handle(int event) {
	if (event == FL_PUSH && Fl::event_button() == FL_LEFT_MOUSE) {
		int xc = screenToBoard(Fl::event_x());
		int yc = screenToBoard(Fl::event_y());
		if (xc >= 0 && yc >= 0 && isValidMove(xc, yc)) {
			setCell(xc, yc, currentPlayer);
			currentPlayer = (currentPlayer == Player::BLACK_STONE) ? Player::WHITE_STONE : Player::BLACK_STONE;
			redraw();
		}
	} else if (event == FL_KEYDOWN && Fl::event_key() == 'r') {
		for (int yy = 0; yy < BOARD_SIZE; yy++)
			for (int xx = 0; xx < BOARD_SIZE; xx++)
				grid[yy][xx] = Player::EMPTY;
		currentPlayer = Player::BLACK_STONE;
		redraw();
	}
	return Fl_Window::handle(event);
}

static GomokuBoard* g_board = nullptr;

static void signal_handler(int sig __attribute__((unused))) {
	if (g_board) {
		delete g_board;
		g_board = nullptr;
	}
	exit(0);
}

int main(int argc, char* argv[]) {
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	g_board = new GomokuBoard();
	g_board->show(argc, argv);
	Fl::run();

	if (g_board) {
		delete g_board;
		g_board = nullptr;
	}

	return 0;
}
