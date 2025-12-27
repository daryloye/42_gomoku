#include "Gomoku.hpp"

static GomokuBoard* g_board = nullptr;

void deleteBoard(void) {
	if (g_board)
	{
		delete g_board;
		g_board = nullptr;
	}
}

static void signal_handler(int sig __attribute__((unused)))
{
	deleteBoard();
	exit(0);
}

int main(int argc, char* argv[])
{
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	g_board = new GomokuBoard();
	g_board->show(argc, argv);
	Fl::run();
	deleteBoard();
	return (0);
}
