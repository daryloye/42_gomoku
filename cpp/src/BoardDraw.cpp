#include "Gomoku.hpp"

void GomokuBoard::draw() {
  // https://www.fltk.org/doc-1.4/drawing.html#fl_rectf
  fl_rectf(0, 0, w(), h(), 220, 200, 150);

  drawBoard();
  drawModeButtons();
  drawUI();
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

  for (int i = 0; i < BOARD_SIZE; i++) {
    int xpos = boardStartX + i * CELL_SIZE;
    int ypos = boardStartY + i * CELL_SIZE;
    fl_line(xpos, boardStartY, xpos, boardEndY);
    fl_line(boardStartX, ypos, boardEndX, ypos);
  }

  int r = CELL_SIZE / 2 - 2;

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

    // Use different colors for black and white suggestions
    if (suggestionForPlayer == Stone::BLACK) {
      fl_color(80, 80, 80); // Dark gray for black
    } else {
      fl_color(220, 220, 220); // Light gray for white
    }

    fl_line_style(FL_SOLID, 3);
    fl_arc(sx - r - 5, sy - r - 5, (r + 5) * 2, (r + 5) * 2, 0, 360);
    fl_line_style(FL_SOLID, 2);
  }

  if (hasIllegalMove && illegalMoveCell.x >= 0 &&
      illegalMoveCell.x < BOARD_SIZE && illegalMoveCell.y >= 0 &&
      illegalMoveCell.y < BOARD_SIZE) {
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
    const int colors[4][3] = {
        {255, 100, 0}, {100, 255, 0}, {100, 0, 255}, {255, 0, 255}};

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
            if (count >= 1000000)
              countStr = std::to_string(count / 1000000) + "m";
            else if (count >= 1000)
              countStr = std::to_string(count / 1000) + "k";
            else
              countStr = std::to_string(count);
            int textX = sx - 4 * (int)countStr.length();
            int textY = sy - 8;
            BitmapFont::drawText(countStr, textX, textY, 1);
          }
        }
      }
    }
  }
}

void GomokuBoard::drawModeButtons() {
  fl_line_style(FL_SOLID, 2);
  int buttonX = OFFSET + BOARD_WIDTH + 10;
  int buttonY = 50;
  int buttonW = 70;
  int buttonH = 35;

  fl_color(0, 0, 0);
  BitmapFont::drawText("Mode:", buttonX, buttonY - 25, 2);

  // 2-Player button
  if (gameMode == GameMode::TWO_PLAYER)
    fl_color(150, 255, 150);
  else
    fl_color(100, 200, 100);
  fl_rectf(buttonX, buttonY, buttonW, buttonH);
  fl_color(0, 0, 0);
  fl_rect(buttonX, buttonY, buttonW, buttonH);
  BitmapFont::drawText("2-PLY", buttonX + 5, buttonY + 10, 1);

  // AI vs Human button
  if (gameMode == GameMode::AI_VS_HUMAN)
    fl_color(150, 200, 255);
  else
    fl_color(100, 150, 200);
  fl_rectf(buttonX, buttonY + buttonH + 5, buttonW, buttonH);
  fl_color(0, 0, 0);
  fl_rect(buttonX, buttonY + buttonH + 5, buttonW, buttonH);
  BitmapFont::drawText("AI", buttonX + 15, buttonY + buttonH + 15, 1);

  // Reset button
  fl_color(220, 100, 100);
  fl_rectf(buttonX, buttonY + 2 * (buttonH + 5), buttonW, buttonH);
  fl_color(0, 0, 0);
  fl_rect(buttonX, buttonY + 2 * (buttonH + 5), buttonW, buttonH);
  BitmapFont::drawText("RESET", buttonX + 8, buttonY + 2 * (buttonH + 5) + 10, 1);

  // Suggestion toggle button (only in Two Player mode)
  if (gameMode == GameMode::TWO_PLAYER && winner == Stone::EMPTY) {
    if (showSuggestion)
      fl_color(150, 255, 150);
    else
      fl_color(100, 200, 100);
    fl_rectf(buttonX, buttonY + 3 * (buttonH + 5), buttonW, buttonH);
    fl_color(0, 0, 0);
    fl_rect(buttonX, buttonY + 3 * (buttonH + 5), buttonW, buttonH);
    BitmapFont::drawText("HINT", buttonX + 10, buttonY + 3 * (buttonH + 5) + 10, 1);
  }

  // Heatmap toggle button (only in AI vs Human mode)
  if (gameMode == GameMode::AI_VS_HUMAN) {
    if (showHeatmap)
      fl_color(255, 200, 150);
    else
      fl_color(200, 150, 100);
    fl_rectf(buttonX, buttonY + 3 * (buttonH + 5), buttonW, buttonH);
    fl_color(0, 0, 0);
    fl_rect(buttonX, buttonY + 3 * (buttonH + 5), buttonW, buttonH);
    BitmapFont::drawText("HEAT", buttonX + 10, buttonY + 3 * (buttonH + 5) + 10, 1);
  }
}

void GomokuBoard::drawUI() {
  const bool gameOver = (winner != Stone::EMPTY);
  const char* headerText =
    gameOver
      ? ((winner == Stone::BLACK) ? "BLACK WINS!" : "WHITE WINS!")
      : ((currentPlayer == Stone::BLACK)  ? "Current: BLACK" 
                                          : "Current: WHITE");
  BitmapFont::drawText(headerText, OFFSET, 35, 2);

  const char *modeText = nullptr;
  switch(gameMode) {
    case GameMode::TWO_PLAYER: modeText = "Mode: 2-Player"; break;
    case GameMode::AI_VS_HUMAN: modeText = "Mode: AI vs Human"; break;
  }
  BitmapFont::drawText(modeText, OFFSET + 450, 35, 2);

  char blackTimeStr[80];
  char whiteTimeStr[80];
  float blackAvg =
      (blackMoveCount > 0) ? timer.totalBlackTime / blackMoveCount : 0.0f;
  float whiteAvg =
      (whiteMoveCount > 0) ? timer.totalWhiteTime / whiteMoveCount : 0.0f;
  snprintf(blackTimeStr, sizeof(blackTimeStr),
           "BLACK: avg %.1fms / last %.1fms | Captured: %d", blackAvg,
           timer.lastBlackMoveTime, blackCaptured);
  snprintf(whiteTimeStr, sizeof(whiteTimeStr),
           "WHITE: avg %.1fms / last %.1fms | Captured: %d", whiteAvg,
           timer.lastWhiteMoveTime, whiteCaptured);

  BitmapFont::drawText(whiteTimeStr, OFFSET, h() - TEXT_MARGIN + 15, 1);
  BitmapFont::drawText(blackTimeStr, OFFSET, h() - TEXT_MARGIN - 5, 1);

  const char *helpText = nullptr;
  switch (gameMode) {
    case GameMode::TWO_PLAYER: helpText = "Use buttons on the right to control the game"; break;
    case GameMode::AI_VS_HUMAN: helpText = "Use buttons on the right to control the game"; break;
  }
  BitmapFont::drawText(helpText, OFFSET, h() - 25, 1);
}
