#include "Gomoku.hpp"
#include "BitmapFont.hpp"

GomokuBoard::GomokuBoard() : Fl_Window(WIN_WIDTH, WIN_HEIGHT, "Gomoku")
{
	reset();
	resizable(nullptr); 
	end();
}

GomokuBoard::~GomokuBoard()
{}

void GomokuBoard::reset()
{
  for (int y = 0; y < BOARD_SIZE; y++)
		for (int x = 0; x < BOARD_SIZE; x++)
			grid[y][x] = Stone::EMPTY;

  currentPlayer = Stone::BLACK;
  winner = Stone::EMPTY;
  previousOutlineCell = { -1, -1 };

  blackMoveCount = 0;
  whiteMoveCount = 0;
  blackCaptured = 0;
  whiteCaptured = 0;
  isAiThinking = false;
  aiThinkTime = 0.0f;
  showSuggestion = false;
  timer.resetAll();
}

Stone GomokuBoard::getStone(Coord cell) const
{
	if (cell.x < 0 || cell.x >= BOARD_SIZE || cell.y < 0 || cell.y >= BOARD_SIZE)
    return (Stone::EMPTY);

  return (grid[cell.y][cell.x]);
}

void GomokuBoard::setStone(Coord cell, Stone stone)
{
	if (cell.x >= 0 && cell.x < BOARD_SIZE && cell.y >= 0 && cell.y < BOARD_SIZE)
    grid[cell.y][cell.x] = stone;
}

bool GomokuBoard::checkWin(Coord cell, Stone stone) const
{
  const int directions[4][2] = {
    {1, 0},
    {0, 1},
    {1, 1},
    {1, -1}
  };

  for (int d = 0; d < 4; d++) {
    int dx = directions[d][0];
    int dy = directions[d][1];
    int count = 1;

    for (int i = 1; i < 5; i++) {
      Coord next = {cell.x + i * dx, cell.y + i * dy};
      if (getStone(next) != stone)
        break;
      count++;
    }

    for (int i = 1; i < 5; i++) {
      Coord next = {cell.x - i * dx, cell.y - i * dy};
      if (getStone(next) != stone)
        break;
      count++;
    }

    if (count >= 5)
      return true;
  }

  return false;
}

void GomokuBoard::draw() {
	// https://www.fltk.org/doc-1.4/drawing.html#fl_rectf
	fl_rectf(0, 0, w(), h(), 220, 200, 150);

	drawBoard();
	drawModeButtons();
	drawUI();
}

void GomokuBoard::drawModeButtons() {
	// https://www.fltk.org/doc-1.4/group__fl__attributes.html#ga3bc05e2b989b509a932bce40a6bc42f5
	fl_line_style(FL_SOLID, 2);

	// Button 1: Two Players
	if (gameMode == GameMode::TWO_PLAYER)
		fl_color(100, 200, 100);
	else
		fl_color(150, 150, 150);
	fl_rectf(OFFSET + 50, 5, 180, 25);
	fl_color(0, 0, 0);
	fl_rect(OFFSET + 50, 5, 180, 25);
	BitmapFont::drawText("1P-2P", OFFSET + 85, 8, 1);

	// Button 2: AI vs Human
	if (gameMode == GameMode::AI_VS_HUMAN)
		fl_color(100, 150, 200);
	else
		fl_color(150, 150, 150);
	fl_rectf(OFFSET + 250, 5, 180, 25);
	fl_color(0, 0, 0);
	fl_rect(OFFSET + 250, 5, 180, 25);
	BitmapFont::drawText("AI vs P1", OFFSET + 275, 8, 1);
}

void GomokuBoard::drawBoard() {
	// https://www.fltk.org/doc-1.4/group__fl__attributes.html#ga1927b6ff6e07c65d5efd79c8cb32c8e1
	fl_color(60, 60, 60);
	// https://www.fltk.org/doc-1.4/group__fl__attributes.html#ga3bc05e2b989b509a932bce40a6bc42f5
	fl_line_style(FL_SOLID, 2);

	int boardYOffset = TEXT_MARGIN;

	for (int i = 0; i < BOARD_SIZE; i++) {
		int xpos = OFFSET + i * CELL_SIZE;
		int ypos = boardYOffset + OFFSET + i * CELL_SIZE;
		// https://www.fltk.org/doc-1.4/drawing.html#fl_line
		fl_line(xpos, boardYOffset + OFFSET, xpos, boardYOffset + OFFSET + (BOARD_SIZE - 1) * CELL_SIZE);
		fl_line(OFFSET, ypos, OFFSET + (BOARD_SIZE - 1) * CELL_SIZE, ypos);
	}

	for (int cellY = 0; cellY < BOARD_SIZE; cellY++) {
		for (int cellX = 0; cellX < BOARD_SIZE; cellX++) {
			Stone stone = getStone({cellX, cellY});

			if (stone == Stone::EMPTY)
				continue;

			int sx = OFFSET + cellX * CELL_SIZE;
			int sy = boardYOffset + OFFSET + cellY * CELL_SIZE;
			int r = CELL_SIZE / 2 - 2;

			if (stone == Stone::OUTLINE) {
				if (currentPlayer == Stone::BLACK)
					fl_color(0, 0, 0);
				else
					fl_color(255, 255, 255);
			} else if (stone == Stone::BLACK) {
				fl_color(0, 0, 0);
			} else {
				fl_color(255, 255, 255);
			}

			fl_pie(sx - r, sy - r, r * 2, r * 2, 0, 360);
		}
	}

	if (showSuggestion && suggestedMove.move.x >= 0) {
		int sx = OFFSET + suggestedMove.move.x * CELL_SIZE;
		int sy = TEXT_MARGIN + OFFSET + suggestedMove.move.y * CELL_SIZE;
		int r = CELL_SIZE / 2 - 2;
		fl_color(200, 100, 0);
		fl_line_style(FL_SOLID, 3);
		fl_arc(sx - r - 5, sy - r - 5, (r + 5) * 2, (r + 5) * 2, 0, 360);
		fl_line_style(FL_SOLID, 2);
	}
}

void GomokuBoard::drawUI() {
	if (winner != Stone::EMPTY) {
		const char* winText = (winner == Stone::BLACK) ? "BLACK WINS!" : "WHITE WINS!";
		BitmapFont::drawText(winText, OFFSET, 35, 2);
	} else {
		const char* playerText = (currentPlayer == Stone::BLACK) ? "Current: BLACK" : "Current: WHITE";
		BitmapFont::drawText(playerText, OFFSET, 35, 2);
	}

	const char* modeText = (gameMode == GameMode::TWO_PLAYER) ? "Mode: 2-Player" : "Mode: AI vs Human";
	BitmapFont::drawText(modeText, OFFSET + 450, 35, 2);

	// Bottom timing and capture info
	char blackTimeStr[50];
	char whiteTimeStr[50];
	float blackAvg = (blackMoveCount > 0) ? timer.totalBlackTime / blackMoveCount : 0.0f;
	float whiteAvg = (whiteMoveCount > 0) ? timer.totalWhiteTime / whiteMoveCount : 0.0f;
	snprintf(blackTimeStr, sizeof(blackTimeStr), "BLACK: %.1fms | Captured: %d", blackAvg, blackCaptured);
	snprintf(whiteTimeStr, sizeof(whiteTimeStr), "WHITE: %.1fms | Captured: %d", whiteAvg, whiteCaptured);

	BitmapFont::drawText(blackTimeStr, OFFSET, h() - TEXT_MARGIN + 15, 1);
	BitmapFont::drawText(whiteTimeStr, OFFSET, h() - TEXT_MARGIN - 5, 1);
	BitmapFont::drawText("Press 'R' to reset | Press 'S' for suggestion", OFFSET + 300, h() - TEXT_MARGIN + 5, 1);
}

int GomokuBoard::handle(int event)
{
	// Check for mode button clicks
	if (event == FL_PUSH && Fl::event_button() == FL_LEFT_MOUSE) {
		if (clickedModeButton(Fl::event_x(), Fl::event_y())) {
			reset();
			redraw();
			return 1;
		}
	}

	// Keyboard shortcuts for mode selection
	if (event == FL_KEYDOWN) {
		int key = Fl::event_key();
		if (key == '1') {
			gameMode = GameMode::TWO_PLAYER;
			reset();
			redraw();
			return 1;
		} else if (key == '2') {
			gameMode = GameMode::AI_VS_HUMAN;
			reset();
			aiColor = Stone::WHITE;  // AI plays as WHITE
			redraw();
			return 1;
		}
	}

	// Show outline when mouse moves
	if (event == FL_MOVE) {
		Coord cell = windowToBoardCoordinates({Fl::event_x(), Fl::event_y()});
		if (!isValidMove(cell, grid))
			return 1;

		setStone(previousOutlineCell, Stone::EMPTY);
		previousOutlineCell = cell;

		setStone(cell, Stone::OUTLINE);
		redraw();
	}

	// Left click to place stone
	if (event == FL_PUSH && Fl::event_button() == FL_LEFT_MOUSE) {
		// In AI mode, only allow human to click during their turn
		if (gameMode == GameMode::AI_VS_HUMAN && currentPlayer == aiColor)
			return 1;

		if (isAiThinking)
			return 1;

		Coord cell = windowToBoardCoordinates({Fl::event_x(), Fl::event_y()});
		if (!isValidMove(cell, grid) || winner != Stone::EMPTY)
			return 1;

		setStone(previousOutlineCell, Stone::EMPTY);
		previousOutlineCell = {-1, -1};

		// Calculate time spent on this move
		timer.calculateTimeSpentOnMove(currentPlayer);

		if (currentPlayer == Stone::BLACK) {
			blackMoveCount++;
		} else {
			whiteMoveCount++;
		}

		setStone(cell, currentPlayer);

		if (checkWin(cell, currentPlayer)) {
			winner = currentPlayer;
		} else {
			currentPlayer = (currentPlayer == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;

			if (gameMode == GameMode::AI_VS_HUMAN && currentPlayer == aiColor) {
				isAiThinking = true;
				aiThinkStartTime = std::chrono::steady_clock::now();
				Fl::add_timeout(0.01, [](void* v) {
					GomokuBoard* board = (GomokuBoard*)v;
					if (board->isAiThinking) {
						board->makeAIMove();
					}
				}, this);
			}
		}

		// Reset timer for next move
		timer.resetTimer();
		redraw();
	}

	// Additional keyboard shortcuts
	if (event == FL_KEYDOWN) {
		int key = Fl::event_key();

		if (key == 'r') {
			reset();
			redraw();
			return 1;
		}

		// Press 'S' for move suggestion (hotseat feature)
		if (key == 's' || key == 'S') {
			if (gameMode == GameMode::TWO_PLAYER && winner == Stone::EMPTY) {
				showSuggestion = !showSuggestion;
				// TODO: Calculate suggested move using minimax
				redraw();
			}
			return 1;
		}
	}

	return Fl_Window::handle(event);
}

void GomokuBoard::makeAIMove() {
	if (currentPlayer != aiColor || gameMode != GameMode::AI_VS_HUMAN) {
		isAiThinking = false;
		return;
	}

	Minimax m;
	Coord lastMove = {-1, -1};

	MinimaxResult aiResult = m.minimax(grid, lastMove, currentPlayer, 3, true);

	setStone(aiResult.move, currentPlayer);

	if (currentPlayer == Stone::BLACK) {
		blackMoveCount++;
	} else {
		whiteMoveCount++;
	}

	if (checkWin(aiResult.move, currentPlayer)) {
		winner = currentPlayer;
	} else {
		currentPlayer = (currentPlayer == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
	}

	auto now = std::chrono::steady_clock::now();
	auto thinkDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now - aiThinkStartTime);
	aiThinkTime = thinkDuration.count();

	timer.calculateTimeSpentOnMove(aiColor);

	timer.resetTimer();

	isAiThinking = false;
	redraw();
}

bool GomokuBoard::clickedModeButton(int x, int y) {
	if (x >= OFFSET + 50 && x <= OFFSET + 50 + 180 && y >= 5 && y <= 5 + 25) {
		if (gameMode != GameMode::TWO_PLAYER) {
			gameMode = GameMode::TWO_PLAYER;
			return true;
		}
		return false;
	}

	if (x >= OFFSET + 250 && x <= OFFSET + 250 + 180 && y >= 5 && y <= 5 + 25) {
		if (gameMode != GameMode::AI_VS_HUMAN) {
			gameMode = GameMode::AI_VS_HUMAN;
			aiColor = Stone::WHITE;
			return true;
		}
		return false;
	}

	return false;
}
