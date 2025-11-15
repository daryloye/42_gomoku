#include "GomokuBoard.hpp"

GomokuBoard::GomokuBoard() : Fl_Window(WIN_SIZE, WIN_SIZE, "Gomoku")
{
	for (int y = 0; y < BOARD_SIZE; y++)
		for (int x = 0; x < BOARD_SIZE; x++)
			grid[y][x] = Stone::EMPTY;

  end();
}

GomokuBoard::~GomokuBoard()
{}

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

void GomokuBoard::draw() {
	// https://www.fltk.org/doc-1.4/drawing.html#fl_rectf
  fl_rectf(0, 0, w(), h(), 220, 200, 150);
  // https://www.fltk.org/doc-1.4/group__fl__attributes.html#ga1927b6ff6e07c65d5efd79c8cb32c8e1
	fl_color(60, 60, 60);
  // https://www.fltk.org/doc-1.4/group__fl__attributes.html#ga3bc05e2b989b509a932bce40a6bc42f5
  fl_line_style(FL_SOLID, 2);
	for (int i = 0; i < BOARD_SIZE; i++) {
		int pos = OFFSET + i * CELL_SIZE;
    // https://www.fltk.org/doc-1.4/drawing.html#fl_line
		fl_line(pos, OFFSET, pos, OFFSET + (BOARD_SIZE - 1) * CELL_SIZE);
		fl_line(OFFSET, pos, OFFSET + (BOARD_SIZE - 1) * CELL_SIZE, pos);
	}

	for (int cellY = 0; cellY < BOARD_SIZE; cellY++) {
		for (int cellX = 0; cellX < BOARD_SIZE; cellX++) {
			Stone stone = getStone( {cellX, cellY} );
			
      if (stone == Stone::EMPTY)
        continue;

			int sx = OFFSET + cellX * CELL_SIZE;
			int sy = OFFSET + cellY * CELL_SIZE;
			int r = CELL_SIZE / 2 - 2;
			
      if (stone == Stone::OUTLINE)
        fl_color(150, 150, 150);
      else if (stone == Stone::BLACK) 
        fl_color(0, 0, 0);
			else 
        fl_color(255, 255, 255);
			
      fl_pie(sx - r, sy - r, r * 2, r * 2, 0, 360);
		}
	}
}

int GomokuBoard::handle(int event)
{
  // Show outline when mouse moves
  if (event == FL_MOVE) {
    Coord cell = windowToBoardCoordinates( {Fl::event_x(), Fl::event_y()} );
    if (isValidMove(cell)) {
      setStone(previousOutlineCell, Stone::EMPTY);
      previousOutlineCell = cell;
      
      setStone(cell, Stone::OUTLINE);
      redraw();
    }
  }

	// Left click to place stone
  if (event == FL_PUSH && Fl::event_button() == FL_LEFT_MOUSE) {
    Coord cell = windowToBoardCoordinates( {Fl::event_x(), Fl::event_y()} );
		if (isValidMove(cell)) {
      setStone(previousOutlineCell, Stone::EMPTY);
      previousOutlineCell = { -1, -1 };

      setStone(cell, currentPlayer);
			currentPlayer = (currentPlayer == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
      redraw();
		}
	}
  
  // Press R to reload board
  else if (event == FL_KEYDOWN && Fl::event_key() == 'r') {
		for (int cellY = 0; cellY < BOARD_SIZE; cellY++)
			for (int cellX = 0; cellX < BOARD_SIZE; cellX++)
				grid[cellY][cellX] = Stone::EMPTY;
		
    currentPlayer = Stone::BLACK;
    previousOutlineCell = { -1, -1 };
    redraw();
	}
	
  return Fl_Window::handle(event);
}