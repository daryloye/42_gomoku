#include "Gomoku.hpp"
#include "BitmapFont.hpp"

GomokuBoard::GomokuBoard() : Fl_Window(WIN_WIDTH, WIN_HEIGHT, "Gomoku")
{
	reset();
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

bool GomokuBoard::isValidMove(Coord cell) const
{
  // check out of board area
  if (cell.x < 0 || cell.y < 0)
    return false;

  // check another stone present
  Stone stone = getStone(cell);
  if (stone == Stone::BLACK || stone == Stone::WHITE )
    return false;

  return true;
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
			Stone stone = getStone( {cellX, cellY} );

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
      } else if (stone == Stone::BLACK)
        fl_color(0, 0, 0);
			else
        fl_color(255, 255, 255);

      fl_pie(sx - r, sy - r, r * 2, r * 2, 0, 360);
		}
	}

	if (winner != Stone::EMPTY) {
		const char* winText = (winner == Stone::BLACK) ? "BLACK WINS!" : "WHITE WINS!";
		BitmapFont::drawText(winText, OFFSET, 10, 2);
	} else {
		const char* playerText = (currentPlayer == Stone::BLACK) ? "Current: BLACK" : "Current: WHITE";
		BitmapFont::drawText(playerText, OFFSET, 10, 2);
	}

	BitmapFont::drawText("Mode: 2-Player", OFFSET + 300, 10, 2);

	char blackTimeStr[50];
	char whiteTimeStr[50];
	float blackAvg = (blackMoveCount > 0) ? timer.totalBlackTime / blackMoveCount : 0.0f;
	float whiteAvg = (whiteMoveCount > 0) ? timer.totalWhiteTime / whiteMoveCount : 0.0f;
	snprintf(blackTimeStr, sizeof(blackTimeStr), "BLACK: %.1fms", blackAvg);
	snprintf(whiteTimeStr, sizeof(whiteTimeStr), "WHITE: %.1fms", whiteAvg);

	BitmapFont::drawText(blackTimeStr, OFFSET, h() - TEXT_MARGIN + 15, 1);
	BitmapFont::drawText(whiteTimeStr, OFFSET + 250, h() - TEXT_MARGIN + 15, 1);
	BitmapFont::drawText("Press 'R' to reset", OFFSET, h() - TEXT_MARGIN - 5, 1);
}

int GomokuBoard::handle(int event)
{
  // Show outline when mouse moves
  if (event == FL_MOVE) {
    Coord cell = windowToBoardCoordinates( {Fl::event_x(), Fl::event_y()} );
    if (!isValidMove(cell))
      return 1;
    
    setStone(previousOutlineCell, Stone::EMPTY);
    previousOutlineCell = cell;
      
    setStone(cell, Stone::OUTLINE);
    redraw();
  }

	// Left click to place stone
  if (event == FL_PUSH && Fl::event_button() == FL_LEFT_MOUSE) {
    Coord cell = windowToBoardCoordinates( {Fl::event_x(), Fl::event_y()} );
		if (!isValidMove(cell) || winner != Stone::EMPTY)
      return 1;
    
    setStone(previousOutlineCell, Stone::EMPTY);
    previousOutlineCell = { -1, -1 };

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
      // AI buggy kept on placing stone on top le fix?
      currentPlayer = (currentPlayer == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
    }

    // Reset timer for next move
    timer.resetTimer();
    redraw();
	}

  // Press r to reset game
  else if (event == FL_KEYDOWN && Fl::event_key() == 'r') {
		reset();
    redraw();
	}
	
  return Fl_Window::handle(event);
}
