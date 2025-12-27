#include "Gomoku.hpp"
#include <exception>
#include <new>

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
	try {
		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);

		g_board = new GomokuBoard();
		if (!g_board) {
			std::cerr << "Error: Failed to allocate memory for game board" << std::endl;
			return 1;
		}

		g_board->show(argc, argv);
		Fl::run();
		deleteBoard();
		return 0;
	}
	catch (const std::bad_alloc& e) {
		std::cerr << "Error: Out of memory - " << e.what() << std::endl;
		deleteBoard();
		return 1;
	}
	catch (const std::exception& e) {
		std::cerr << "Error: Exception occurred - " << e.what() << std::endl;
		deleteBoard();
		return 1;
	}
	catch (...) {
		std::cerr << "Error: Unknown exception occurred" << std::endl;
		deleteBoard();
		return 1;
	}
}
