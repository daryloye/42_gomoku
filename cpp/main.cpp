#include "Gomoku.hpp"

static GomokuBoard* g_board = nullptr;

static void signal_handler(int sig __attribute__((unused))) {
	if (g_board) {
		delete g_board;
		g_board = nullptr;
	}
	exit(0);
}

int main(int argc, char* argv[])
{
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	g_board = new GomokuBoard();
	g_board->show(argc, argv);
  // https://www.fltk.org/doc-1.4/classFl.html#a8e4e7e9e1c0a3e7f4f5f0f1f2f3f4f5
  Fl::run();

	if (g_board) {
		delete g_board;
		g_board = nullptr;
	}

	return 0;
}
