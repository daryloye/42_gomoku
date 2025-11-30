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
		for (int x = 0; x < BOARD_SIZE; x++) {
			grid[y][x] = Stone::EMPTY;
			lastEvaluationHeatmap[y][x] = 0;
		}

  currentPlayer = Stone::BLACK;
  winner = Stone::EMPTY;
  previousOutlineCell = { -1, -1 };

  blackMoveCount = 0;
  whiteMoveCount = 0;
  blackCaptured = 0;
  whiteCaptured = 0;
  illegalMoveCell = { -1, -1 };
  hasIllegalMove = false;
  illegalDirections = {false, false, false, false};
  isAiThinking = false;
  aiThinkTime = 0.0f;
  showSuggestion = false;
  showHeatmap = false;
  heatmapNeedsRedraw = false;
  timer.resetAll();

  selectedOpening = Opening::STANDARD;
  gameStarted = false;
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

void GomokuBoard::analyzeDoubleThree(Coord move, Stone colour, std::array<bool, 4>& directions)
{
	const int dirs[4][2] = {
		{1, 0},   // horizontal
		{0, 1},   // vertical
		{1, 1},   // diagonal down-right
		{1, -1}   // diagonal down-left
	};

	for (int d = 0; d < 4; d++) {
		int dx = dirs[d][0];
		int dy = dirs[d][1];

		int countForward = 0;
		int countBackward = 0;

		for (int i = 1; i < 4; i++) {
			Coord forward = {move.x + i*dx, move.y + i*dy};
			if (forward.x >= 0 && forward.x < BOARD_SIZE && forward.y >= 0 && forward.y < BOARD_SIZE &&
				grid[forward.y][forward.x] == colour)
				countForward++;
			else
				break;
		}

		for (int i = 1; i < 4; i++) {
			Coord backward = {move.x - i*dx, move.y - i*dy};
			if (backward.x >= 0 && backward.x < BOARD_SIZE && backward.y >= 0 && backward.y < BOARD_SIZE &&
				grid[backward.y][backward.x] == colour)
				countBackward++;
			else
				break;
		}

		int totalCount = 1 + countForward + countBackward;

		if (totalCount >= 3) {
			Coord endForward = {move.x + (countForward + 1)*dx, move.y + (countForward + 1)*dy};
			Coord endBackward = {move.x - (countBackward + 1)*dx, move.y - (countBackward + 1)*dy};

			bool forwardOpen = (endForward.x < 0 || endForward.x >= BOARD_SIZE || endForward.y < 0 || endForward.y >= BOARD_SIZE) ||
							   grid[endForward.y][endForward.x] == Stone::EMPTY;
			bool backwardOpen = (endBackward.x < 0 || endBackward.x >= BOARD_SIZE || endBackward.y < 0 || endBackward.y >= BOARD_SIZE) ||
							    grid[endBackward.y][endBackward.x] == Stone::EMPTY;

			if (totalCount == 3 && forwardOpen && backwardOpen)
				directions[d] = true;
		}
	}
}

void GomokuBoard::draw() {
	// https://www.fltk.org/doc-1.4/drawing.html#fl_rectf
	fl_rectf(0, 0, w(), h(), 220, 200, 150);

	drawBoard();
	drawModeButtons();
	drawOpeningButtons();
	drawUI();
}

void GomokuBoard::drawModeButtons() {
	fl_line_style(FL_SOLID, 2);
	int buttonX = OFFSET + BOARD_WIDTH + 10;
	int buttonY = 50;
	int buttonW = 70;
	int buttonH = 35;

	fl_color(0, 0, 0);
	BitmapFont::drawText("Mode:", buttonX, buttonY - 25, 2);

	if (gameMode == GameMode::TWO_PLAYER)
		fl_color(150, 255, 150);
	else
		fl_color(100, 200, 100);
	fl_rectf(buttonX, buttonY, buttonW, buttonH);
	fl_color(0, 0, 0);
	fl_rect(buttonX, buttonY, buttonW, buttonH);
	BitmapFont::drawText("2-PLY", buttonX + 5, buttonY + 10, 1);

	if (gameMode == GameMode::AI_VS_HUMAN)
		fl_color(150, 200, 255);
	else
		fl_color(100, 150, 200);
	fl_rectf(buttonX, buttonY + buttonH + 5, buttonW, buttonH);
	fl_color(0, 0, 0);
	fl_rect(buttonX, buttonY + buttonH + 5, buttonW, buttonH);
	BitmapFont::drawText("AI", buttonX + 15, buttonY + buttonH + 15, 1);
}

void GomokuBoard::drawBoard() {
	// https://www.fltk.org/doc-1.4/group__fl__attributes.html#ga1927b6ff6e07c65d5efd79c8cb32c8e1
	fl_color(60, 60, 60);
	// https://www.fltk.org/doc-1.4/group__fl__attributes.html#ga3bc05e2b989b509a932bce40a6bc42f5
	fl_line_style(FL_SOLID, 2);

	int boardYOffset = TEXT_MARGIN;
	int boardStartX = OFFSET;
	int boardStartY = boardYOffset + OFFSET;
	int boardEndX = OFFSET + (BOARD_SIZE - 1) * CELL_SIZE;
	int boardEndY = boardYOffset + OFFSET + (BOARD_SIZE - 1) * CELL_SIZE;

	// Draw grid lines
	for (int i = 0; i < BOARD_SIZE; i++) {
		int xpos = boardStartX + i * CELL_SIZE;
		int ypos = boardStartY + i * CELL_SIZE;
		fl_line(xpos, boardStartY, xpos, boardEndY);
		fl_line(boardStartX, ypos, boardEndX, ypos);
	}

	int r = CELL_SIZE / 2 - 2;

	// Draw black stones first
	fl_color(0, 0, 0);
	for (int cellY = 0; cellY < BOARD_SIZE; cellY++) {
		for (int cellX = 0; cellX < BOARD_SIZE; cellX++) {
			Stone stone = getStone({cellX, cellY});
			if (stone == Stone::BLACK) {
				int sx = boardStartX + cellX * CELL_SIZE;
				int sy = boardStartY + cellY * CELL_SIZE;
				fl_pie(sx - r, sy - r, r * 2, r * 2, 0, 360);
			}
		}
	}

	// Draw white stones
	fl_color(255, 255, 255);
	for (int cellY = 0; cellY < BOARD_SIZE; cellY++) {
		for (int cellX = 0; cellX < BOARD_SIZE; cellX++) {
			Stone stone = getStone({cellX, cellY});
			if (stone == Stone::WHITE) {
				int sx = boardStartX + cellX * CELL_SIZE;
				int sy = boardStartY + cellY * CELL_SIZE;
				fl_pie(sx - r, sy - r, r * 2, r * 2, 0, 360);
			}
		}
	}

	// Draw outline stones
	if (currentPlayer == Stone::BLACK)
		fl_color(0, 0, 0);
	else
		fl_color(255, 255, 255);

	for (int cellY = 0; cellY < BOARD_SIZE; cellY++) {
		for (int cellX = 0; cellX < BOARD_SIZE; cellX++) {
			Stone stone = getStone({cellX, cellY});
			if (stone == Stone::OUTLINE) {
				int sx = boardStartX + cellX * CELL_SIZE;
				int sy = boardStartY + cellY * CELL_SIZE;
				fl_pie(sx - r, sy - r, r * 2, r * 2, 0, 360);
			}
		}
	}

	if (showSuggestion && suggestedMove.move.x >= 0) {
		int sx = boardStartX + suggestedMove.move.x * CELL_SIZE;
		int sy = boardStartY + suggestedMove.move.y * CELL_SIZE;
		fl_color(200, 100, 0);
		fl_line_style(FL_SOLID, 3);
		fl_arc(sx - r - 5, sy - r - 5, (r + 5) * 2, (r + 5) * 2, 0, 360);
		fl_line_style(FL_SOLID, 2);
	}

	if (hasIllegalMove && illegalMoveCell.x >= 0 && illegalMoveCell.x < BOARD_SIZE && illegalMoveCell.y >= 0 && illegalMoveCell.y < BOARD_SIZE) {
		int sx = boardStartX + illegalMoveCell.x * CELL_SIZE;
		int sy = boardStartY + illegalMoveCell.y * CELL_SIZE;

		fl_color(255, 0, 0);
		fl_line_style(FL_SOLID, 4);
		int offsetX = 10;
		int offsetY = 10;
		fl_line(sx - offsetX, sy - offsetY, sx + offsetX, sy + offsetY);
		fl_line(sx + offsetX, sy - offsetY, sx - offsetX, sy + offsetY);
		fl_line_style(FL_SOLID, 2);

		const int dirs[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
		const int colors[4][3] = {{255, 100, 0}, {100, 255, 0}, {100, 0, 255}, {255, 0, 255}};

		for (int d = 0; d < 4; d++) {
			if (illegalDirections[d]) {
				int dx = dirs[d][0];
				int dy = dirs[d][1];
				fl_color(colors[d][0], colors[d][1], colors[d][2]);
				fl_line_style(FL_SOLID, 3);

				int x1 = sx - 3 * CELL_SIZE * dx;
				int y1 = sy - 3 * CELL_SIZE * dy;
				int x2 = sx + 3 * CELL_SIZE * dx;
				int y2 = sy + 3 * CELL_SIZE * dy;
				fl_line(x1, y1, x2, y2);
			}
		}
		fl_line_style(FL_SOLID, 2);
	}

	if (showHeatmap) {
		int maxCount = 0;
		for (int y = 0; y < BOARD_SIZE; y++) {
			for (int x = 0; x < BOARD_SIZE; x++) {
				maxCount = std::max(maxCount, (int)lastEvaluationHeatmap[y][x]);
			}
		}

		if (maxCount > 0) {
			for (int cellY = 0; cellY < BOARD_SIZE; cellY++) {
				for (int cellX = 0; cellX < BOARD_SIZE; cellX++) {
					int count = lastEvaluationHeatmap[cellY][cellX];
					if (count > 0 && getStone({cellX, cellY}) == Stone::EMPTY) {
						int sx = boardStartX + cellX * CELL_SIZE;
						int sy = boardStartY + cellY * CELL_SIZE;

						float intensity = (float)count / maxCount;

						int red = (int)(100 + 155 * intensity);
						int green = (int)(150 + 50 * (1.0f - intensity));
						int blue = (int)(200 + 55 * (1.0f - intensity));

						fl_color(red, green, blue);
						fl_pie(sx - r, sy - r, r * 2, r * 2, 0, 360);
					}
				}
			}
		}

		if (maxCount > 0) {
			for (int cellY = 0; cellY < BOARD_SIZE; cellY++) {
				for (int cellX = 0; cellX < BOARD_SIZE; cellX++) {
					int count = lastEvaluationHeatmap[cellY][cellX];
					if (count > 0 && getStone({cellX, cellY}) == Stone::EMPTY) {
						int sx = boardStartX + cellX * CELL_SIZE;
						int sy = boardStartY + cellY * CELL_SIZE;

						fl_color(255, 255, 255);
						std::string countStr;
						if (count >= 1000000) countStr = std::to_string(count / 1000000) + "m";
						else if (count >= 1000) countStr = std::to_string(count / 1000) + "k";
						else countStr = std::to_string(count);
						int textX = sx - 4 * (int)countStr.length();
						int textY = sy - 8;
						BitmapFont::drawText(countStr, textX, textY, 1);
					}
				}
			}
		}
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

	char blackTimeStr[80];
	char whiteTimeStr[80];
	float blackAvg = (blackMoveCount > 0) ? timer.totalBlackTime / blackMoveCount : 0.0f;
	float whiteAvg = (whiteMoveCount > 0) ? timer.totalWhiteTime / whiteMoveCount : 0.0f;
	snprintf(blackTimeStr, sizeof(blackTimeStr), "BLACK: avg %.1fms / last %.1fms | Captured: %d", blackAvg, timer.lastBlackMoveTime, blackCaptured);
	snprintf(whiteTimeStr, sizeof(whiteTimeStr), "WHITE: avg %.1fms / last %.1fms | Captured: %d", whiteAvg, timer.lastWhiteMoveTime, whiteCaptured);

	BitmapFont::drawText(whiteTimeStr, OFFSET, h() - TEXT_MARGIN + 15, 1);
	BitmapFont::drawText(blackTimeStr, OFFSET, h() - TEXT_MARGIN - 5, 1);

	const char* helpText = (gameMode == GameMode::AI_VS_HUMAN)
		? "Press 'R' to reset | Press 'H' for heatmap"
		: "Press 'R' to reset | Press 'S' for suggestion";
	BitmapFont::drawText(helpText, OFFSET, h() - 25, 1);
}

void GomokuBoard::drawOpeningButtons() {
	// Opening buttons below mode buttons on right side
	fl_line_style(FL_SOLID, 2);
	int buttonX = OFFSET + BOARD_WIDTH + 10;
	int buttonY = 175;  // Below mode buttons
	int buttonW = 70;
	int buttonH = 35;

	// Title
	fl_color(0, 0, 0);
	BitmapFont::drawText("Open:", buttonX, buttonY - 25, 2);

	// Button: Standard
	if (selectedOpening == Opening::STANDARD)
		fl_color(150, 255, 150);
	else
		fl_color(100, 200, 100);
	fl_rectf(buttonX, buttonY, buttonW, buttonH);
	fl_color(0, 0, 0);
	fl_rect(buttonX, buttonY, buttonW, buttonH);
	BitmapFont::drawText("STD", buttonX + 12, buttonY + 10, 1);

	// Button: Pro
	if (selectedOpening == Opening::PRO)
		fl_color(150, 200, 255);
	else
		fl_color(100, 150, 200);
	fl_rectf(buttonX, buttonY + buttonH + 5, buttonW, buttonH);
	fl_color(0, 0, 0);
	fl_rect(buttonX, buttonY + buttonH + 5, buttonW, buttonH);
	BitmapFont::drawText("PRO", buttonX + 12, buttonY + buttonH + 15, 1);

	// Button: Swap
	if (selectedOpening == Opening::SWAP)
		fl_color(255, 200, 150);
	else
		fl_color(200, 150, 100);
	fl_rectf(buttonX, buttonY + 2*(buttonH + 5), buttonW, buttonH);
	fl_color(0, 0, 0);
	fl_rect(buttonX, buttonY + 2*(buttonH + 5), buttonW, buttonH);
	BitmapFont::drawText("SWAP", buttonX + 8, buttonY + 2*(buttonH + 5) + 10, 1);

	// Button: Swap2
	if (selectedOpening == Opening::SWAP2)
		fl_color(255, 150, 200);
	else
		fl_color(200, 100, 150);
	fl_rectf(buttonX, buttonY + 3*(buttonH + 5), buttonW, buttonH);
	fl_color(0, 0, 0);
	fl_rect(buttonX, buttonY + 3*(buttonH + 5), buttonW, buttonH);
	BitmapFont::drawText("SW2", buttonX + 12, buttonY + 3*(buttonH + 5) + 10, 1);
}


int GomokuBoard::handle(int event)
{


	if (event == FL_PUSH && Fl::event_button() == FL_LEFT_MOUSE) {
		if (clickedModeButton(Fl::event_x(), Fl::event_y())) {
			reset();
			redraw();
			return 1;
		}
		if (clickedOpeningButton(Fl::event_x(), Fl::event_y())) {
			redraw();
			return 1;
		}
	}

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
		if (!isValidMove(cell, grid) || winner != Stone::EMPTY) {
			if (cell.x >= 0 && cell.x < BOARD_SIZE && cell.y >= 0 && cell.y < BOARD_SIZE) {
				illegalMoveCell = cell;
				hasIllegalMove = true;
				redraw();
			}
			return 1;
		}

		if (createsDoubleThree(cell, currentPlayer, grid)) {
			illegalMoveCell = cell;
			hasIllegalMove = true;
			illegalDirections = {false, false, false, false};
			analyzeDoubleThree(cell, currentPlayer, illegalDirections);
			redraw();
			return 1;
		}

		setStone(previousOutlineCell, Stone::EMPTY);
		previousOutlineCell = {-1, -1};
		hasIllegalMove = false;
		illegalMoveCell = {-1, -1};
		illegalDirections = {false, false, false, false};

		timer.calculateTimeSpentOnMove(currentPlayer);

		if (currentPlayer == Stone::BLACK) blackMoveCount++;
		else whiteMoveCount++;

		setStone(cell, currentPlayer);

		// Handle captures
		int capturedPairs = countCapturedPairs(cell, currentPlayer, grid);
		if (currentPlayer == Stone::BLACK) {
			blackCaptured += capturedPairs;
		} else {
			whiteCaptured += capturedPairs;
		}

		// Remove captured stones
		if (capturedPairs > 0) {
			Stone opponent = (currentPlayer == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
			const int directions[4][2] = {
				{1, 0}, {0, 1}, {1, 1}, {1, -1}
			};

			for (const auto& dir : directions) {
				int dx = dir[0];
				int dy = dir[1];

				// Check positive direction
				if (grid[cell.y + dy][cell.x + dx] == opponent &&
					grid[cell.y + 2*dy][cell.x + 2*dx] == opponent &&
					grid[cell.y + 3*dy][cell.x + 3*dx] == currentPlayer) {
					grid[cell.y + dy][cell.x + dx] = Stone::EMPTY;
					grid[cell.y + 2*dy][cell.x + 2*dx] = Stone::EMPTY;
				}

				// Check negative direction
				if (grid[cell.y - dy][cell.x - dx] == opponent &&
					grid[cell.y - 2*dy][cell.x - 2*dx] == opponent &&
					grid[cell.y - 3*dy][cell.x - 3*dx] == currentPlayer) {
					grid[cell.y - dy][cell.x - dx] = Stone::EMPTY;
					grid[cell.y - 2*dy][cell.x - 2*dx] = Stone::EMPTY;
				}
			}
		}

		bool winByAlignment = checkWin(cell, currentPlayer);
		bool winByCapture = (currentPlayer == Stone::BLACK) ? (blackCaptured >= 10) : (whiteCaptured >= 10);

		if (winByAlignment || winByCapture) {
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

		// Press 'H' to toggle AI evaluation heatmap
		if (key == 'h' || key == 'H') {
			if (gameMode == GameMode::AI_VS_HUMAN) {
				showHeatmap = !showHeatmap;
				if (showHeatmap) {
					heatmapNeedsRedraw = true;
				}
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

  Stone playerColor = Stone::BLACK;
	Minimax m(aiColor, playerColor);
	Coord lastMove = {-1, -1};

	// currently only till depth 6 then below 500ms damm
	MinimaxResult aiResult = m.minimax(grid, lastMove, 7, aiColor, playerColor, true);

	lastEvaluationHeatmap = m.getEvaluationHeatmap();
	heatmapNeedsRedraw = true;

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
	int buttonX = OFFSET + BOARD_WIDTH + 10;
	int buttonY = 50;
	int buttonW = 70;
	int buttonH = 35;

	// Button: 2-Player (y: 50-85)
	if (x >= buttonX && x <= buttonX + buttonW && y >= buttonY && y <= buttonY + buttonH) {
		if (gameMode != GameMode::TWO_PLAYER) {
			gameMode = GameMode::TWO_PLAYER;
			return true;
		}
		return false;
	}
	// Button: AI vs Human (y: 90-125)
	if (x >= buttonX && x <= buttonX + buttonW && y >= buttonY + buttonH + 5 && y <= buttonY + 2*buttonH + 5) {
		if (gameMode != GameMode::AI_VS_HUMAN) {
			gameMode = GameMode::AI_VS_HUMAN;
			aiColor = Stone::WHITE;
			return true;
		}
		return false;
	}

	return false;
}

void GomokuBoard::initializeOpening(Opening opening) {
	selectedOpening = opening;
	gameStarted = true;
	
	// Place opening stones if needed
	if (opening == Opening::PRO) {
		// Place stones at opposite corners
		grid[3][3] = Stone::BLACK;
		grid[15][15] = Stone::WHITE;
		blackMoveCount = 1;
		whiteMoveCount = 1;
	}
}

bool GomokuBoard::clickedOpeningButton(int x, int y) {
	int buttonX = OFFSET + BOARD_WIDTH + 10;
	int buttonY = 175;
	int buttonW = 70;
	int buttonH = 35;

	if (x >= buttonX && x <= buttonX + buttonW && y >= buttonY && y <= buttonY + buttonH) {
		selectedOpening = Opening::STANDARD;
		redraw();
		return true;
	}
	if (x >= buttonX && x <= buttonX + buttonW && y >= buttonY + buttonH + 5 && y <= buttonY + 2*buttonH + 5) {
		selectedOpening = Opening::PRO;
		redraw();
		return true;
	}
	if (x >= buttonX && x <= buttonX + buttonW && y >= buttonY + 2*(buttonH + 5) && y <= buttonY + 3*(buttonH + 5) - 5) {
		selectedOpening = Opening::SWAP;
		redraw();
		return true;
	}
	if (x >= buttonX && x <= buttonX + buttonW && y >= buttonY + 3*(buttonH + 5) && y <= buttonY + 4*(buttonH + 5) - 5) {
		selectedOpening = Opening::SWAP2;
		redraw();
		return true;
	}
	return false;
}
