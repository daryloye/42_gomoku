#include "Gomoku.hpp"

GomokuBoard::GomokuBoard() : Fl_Window(WIN_WIDTH, WIN_HEIGHT, "Gomoku") {
  reset();
  resizable(nullptr);
  end();
}

GomokuBoard::~GomokuBoard() {}

void GomokuBoard::reset() {
  for (int y = 0; y < BOARD_SIZE; y++)
    for (int x = 0; x < BOARD_SIZE; x++) {
      grid[y][x] = Stone::EMPTY;
      lastEvaluationHeatmap[y][x] = 0;
    }

  currentPlayer = Stone::BLACK;
  winner = Stone::EMPTY;
  previousOutlineCell = {-1, -1};

  blackMoveCount = 0;
  whiteMoveCount = 0;
  blackCaptured = 0;
  whiteCaptured = 0;
  illegalMoveCell = {-1, -1};
  hasIllegalMove = false;
  illegalDirections = {false, false, false, false};
  isAiThinking = false;
  aiThinkTime = 0.0f;
  showSuggestion = false;
  suggestionForPlayer = Stone::EMPTY;
  showHeatmap = false;
  heatmapNeedsRedraw = false;
  timer.resetAll();
}

Stone GomokuBoard::getStone(Coord cell) const {
  if (cell.x < 0 || cell.x >= BOARD_SIZE || cell.y < 0 || cell.y >= BOARD_SIZE)
    return (Stone::EMPTY);

  return (grid[cell.y][cell.x]);
}

void GomokuBoard::setStone(Coord cell, Stone stone) {
  if (cell.x >= 0 && cell.x < BOARD_SIZE && cell.y >= 0 && cell.y < BOARD_SIZE)
    grid[cell.y][cell.x] = stone;
}

bool GomokuBoard::checkWin(Coord cell, Stone stone) const {
  const int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

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

void GomokuBoard::analyzeDoubleThree(Coord move, Stone colour,
                                     std::array<bool, 4> &directions) {
  const int dirs[4][2] = {
      {1, 0}, // horizontal
      {0, 1}, // vertical
      {1, 1}, // diagonal down-right
      {1, -1} // diagonal down-left
  };

  for (int d = 0; d < 4; d++) {
    int dx = dirs[d][0];
    int dy = dirs[d][1];

    int countForward = 0;
    int countBackward = 0;

    for (int i = 1; i < 4; i++) {
      Coord forward = {move.x + i * dx, move.y + i * dy};
      if (forward.x >= 0 && forward.x < BOARD_SIZE && forward.y >= 0 &&
          forward.y < BOARD_SIZE && grid[forward.y][forward.x] == colour)
        countForward++;
      else
        break;
    }

    for (int i = 1; i < 4; i++) {
      Coord backward = {move.x - i * dx, move.y - i * dy};
      if (backward.x >= 0 && backward.x < BOARD_SIZE && backward.y >= 0 &&
          backward.y < BOARD_SIZE && grid[backward.y][backward.x] == colour)
        countBackward++;
      else
        break;
    }

    int totalCount = 1 + countForward + countBackward;

    if (totalCount >= 3) {
      Coord endForward = {move.x + (countForward + 1) * dx,
                          move.y + (countForward + 1) * dy};
      Coord endBackward = {move.x - (countBackward + 1) * dx,
                           move.y - (countBackward + 1) * dy};

      bool forwardOpen = (endForward.x < 0 || endForward.x >= BOARD_SIZE ||
                          endForward.y < 0 || endForward.y >= BOARD_SIZE) ||
                         grid[endForward.y][endForward.x] == Stone::EMPTY;
      bool backwardOpen = (endBackward.x < 0 || endBackward.x >= BOARD_SIZE ||
                           endBackward.y < 0 || endBackward.y >= BOARD_SIZE) ||
                          grid[endBackward.y][endBackward.x] == Stone::EMPTY;

      if (totalCount == 3 && forwardOpen && backwardOpen)
        directions[d] = true;
    }
  }
}

int GomokuBoard::handle(int event) {
  if (event == FL_PUSH && Fl::event_button() == FL_LEFT_MOUSE) {
    if (clickedModeButton(Fl::event_x(), Fl::event_y())) {
      reset();
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
      aiColor = Stone::WHITE;
      redraw();
      return 1;
    }
  }

  // Show outline when mouse moves
  if (event == FL_MOVE) {
    // Don't show outline if game is over
    if (winner != Stone::EMPTY) {
      return 1;
    }

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
    // In AI vs Human mode, only allow human to click during their turn
    if (gameMode == GameMode::AI_VS_HUMAN && currentPlayer == aiColor)
      return 1;

    if (isAiThinking)
      return 1;

    Coord cell = windowToBoardCoordinates({Fl::event_x(), Fl::event_y()});
    if (!isValidMove(cell, grid) || winner != Stone::EMPTY) {
      if (cell.x >= 0 && cell.x < BOARD_SIZE && cell.y >= 0 &&
          cell.y < BOARD_SIZE) {
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

    if (currentPlayer == Stone::BLACK)
      blackMoveCount++;
    else
      whiteMoveCount++;

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
      Stone opponent =
          (currentPlayer == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
      const int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

      for (const auto &dir : directions) {
        int dx = dir[0];
        int dy = dir[1];

        // Check positive direction
        int py1 = cell.y + dy, px1 = cell.x + dx;
        int py2 = cell.y + 2 * dy, px2 = cell.x + 2 * dx;
        int py3 = cell.y + 3 * dy, px3 = cell.x + 3 * dx;
        if (py1 >= 0 && py1 < BOARD_SIZE && px1 >= 0 && px1 < BOARD_SIZE &&
            py2 >= 0 && py2 < BOARD_SIZE && px2 >= 0 && px2 < BOARD_SIZE &&
            py3 >= 0 && py3 < BOARD_SIZE && px3 >= 0 && px3 < BOARD_SIZE &&
            grid[py1][px1] == opponent && grid[py2][px2] == opponent &&
            grid[py3][px3] == currentPlayer) {
          grid[py1][px1] = Stone::EMPTY;
          grid[py2][px2] = Stone::EMPTY;
        }

        // Check negative direction
        int ny1 = cell.y - dy, nx1 = cell.x - dx;
        int ny2 = cell.y - 2 * dy, nx2 = cell.x - 2 * dx;
        int ny3 = cell.y - 3 * dy, nx3 = cell.x - 3 * dx;
        if (ny1 >= 0 && ny1 < BOARD_SIZE && nx1 >= 0 && nx1 < BOARD_SIZE &&
            ny2 >= 0 && ny2 < BOARD_SIZE && nx2 >= 0 && nx2 < BOARD_SIZE &&
            ny3 >= 0 && ny3 < BOARD_SIZE && nx3 >= 0 && nx3 < BOARD_SIZE &&
            grid[ny1][nx1] == opponent && grid[ny2][nx2] == opponent &&
            grid[ny3][nx3] == currentPlayer) {
          grid[ny1][nx1] = Stone::EMPTY;
          grid[ny2][nx2] = Stone::EMPTY;
        }
      }
    }

    // Check win conditions according to endgame capture rules
    bool hasFiveInRow = checkWin(cell, currentPlayer);
    bool opponentCanBreakFive = false;

    if (hasFiveInRow) {
      opponentCanBreakFive =
          canOpponentBreakFiveByCapture(cell, currentPlayer, grid);
    }

    bool winByAlignment = hasFiveInRow && !opponentCanBreakFive;

    bool winByCapture = (currentPlayer == Stone::BLACK) ? (blackCaptured >= 10)
                                                        : (whiteCaptured >= 10);

    int opponentLostStones =
        (currentPlayer == Stone::BLACK) ? whiteCaptured : blackCaptured;
    bool winByCaptureThreat = false;

    if (opponentLostStones >= 8) {
      for (int y = 0; y < BOARD_SIZE && !winByCaptureThreat; y++) {
        for (int x = 0; x < BOARD_SIZE && !winByCaptureThreat; x++) {
          if (grid[y][x] == Stone::EMPTY) {
            Coord testMove = {x, y};
            if (countCapturedPairs(testMove, currentPlayer, grid) > 0) {
              winByCaptureThreat = true;
            }
          }
        }
      }
    }

    if (winByAlignment || winByCapture || winByCaptureThreat) {
      winner = currentPlayer;
      // Clear outline stone when game ends
      setStone(previousOutlineCell, Stone::EMPTY);
      previousOutlineCell = {-1, -1};
    } else {
      currentPlayer =
          (currentPlayer == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;

      if (gameMode == GameMode::AI_VS_HUMAN && currentPlayer == aiColor) {
        isAiThinking = true;
        aiThinkStartTime = std::chrono::steady_clock::now();
        Fl::add_timeout(
            0.01,
            [](void *v) {
              GomokuBoard *board = (GomokuBoard *)v;
              board->makeAIMove();
            },
            this);
      }

      // Auto-update suggestion for next turn if suggestion is enabled
      if (showSuggestion) {
        updateSuggestion();
      }
    }

    timer.resetTimer();
    redraw();
  }

  if (event == FL_KEYDOWN) {
    int key = Fl::event_key();

    if (key == 'r') {
      reset();
      redraw();
      return 1;
    }

    if (key == 's' || key == 'S') {
      if (gameMode == GameMode::TWO_PLAYER && winner == Stone::EMPTY) {
        showSuggestion = !showSuggestion;
        if (showSuggestion) {
          updateSuggestion();
        }
        redraw();
      }
      return 1;
    }

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
  try {
    if (gameMode == GameMode::AI_VS_HUMAN && currentPlayer != aiColor) {
      isAiThinking = false;
      return;
    }

    if (gameMode == GameMode::TWO_PLAYER) {
      isAiThinking = false;
      return;
    }

    Stone aiPlayer = currentPlayer;
    Stone opponentColor =
        (currentPlayer == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;

    Minimax m(currentPlayer, opponentColor);
    Coord lastMove = {-1, -1};

    MinimaxResult aiResult =
        m.minimax(grid, lastMove, 10, currentPlayer, opponentColor);

  lastEvaluationHeatmap = m.getEvaluationHeatmap();
  heatmapNeedsRedraw = true;

  setStone(aiResult.move, currentPlayer);

  if (currentPlayer == Stone::BLACK) {
    blackMoveCount++;
  } else {
    whiteMoveCount++;
  }

  int capturedPairs = countCapturedPairs(aiResult.move, currentPlayer, grid);
  if (currentPlayer == Stone::BLACK) {
    blackCaptured += capturedPairs;
  } else {
    whiteCaptured += capturedPairs;
  }

  if (capturedPairs > 0) {
    Stone opponent =
        (currentPlayer == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
    const int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

    for (const auto &dir : directions) {
      int dx = dir[0];
      int dy = dir[1];

      int py1 = aiResult.move.y + dy, px1 = aiResult.move.x + dx;
      int py2 = aiResult.move.y + 2 * dy, px2 = aiResult.move.x + 2 * dx;
      int py3 = aiResult.move.y + 3 * dy, px3 = aiResult.move.x + 3 * dx;
      if (py1 >= 0 && py1 < BOARD_SIZE && px1 >= 0 && px1 < BOARD_SIZE &&
          py2 >= 0 && py2 < BOARD_SIZE && px2 >= 0 && px2 < BOARD_SIZE &&
          py3 >= 0 && py3 < BOARD_SIZE && px3 >= 0 && px3 < BOARD_SIZE &&
          grid[py1][px1] == opponent && grid[py2][px2] == opponent &&
          grid[py3][px3] == currentPlayer) {
        grid[py1][px1] = Stone::EMPTY;
        grid[py2][px2] = Stone::EMPTY;
      }

      int ny1 = aiResult.move.y - dy, nx1 = aiResult.move.x - dx;
      int ny2 = aiResult.move.y - 2 * dy, nx2 = aiResult.move.x - 2 * dx;
      int ny3 = aiResult.move.y - 3 * dy, nx3 = aiResult.move.x - 3 * dx;
      if (ny1 >= 0 && ny1 < BOARD_SIZE && nx1 >= 0 && nx1 < BOARD_SIZE &&
          ny2 >= 0 && ny2 < BOARD_SIZE && nx2 >= 0 && nx2 < BOARD_SIZE &&
          ny3 >= 0 && ny3 < BOARD_SIZE && nx3 >= 0 && nx3 < BOARD_SIZE &&
          grid[ny1][nx1] == opponent && grid[ny2][nx2] == opponent &&
          grid[ny3][nx3] == currentPlayer) {
        grid[ny1][nx1] = Stone::EMPTY;
        grid[ny2][nx2] = Stone::EMPTY;
      }
    }
  }

  bool hasFiveInRow = checkWin(aiResult.move, currentPlayer);
  bool opponentCanBreakFive = false;

  if (hasFiveInRow) {
    opponentCanBreakFive =
        canOpponentBreakFiveByCapture(aiResult.move, currentPlayer, grid);
  }

  bool winByAlignment = hasFiveInRow && !opponentCanBreakFive;
  bool winByCapture = (currentPlayer == Stone::BLACK) ? (blackCaptured >= 10)
                                                      : (whiteCaptured >= 10);

  int opponentLostStones =
      (currentPlayer == Stone::BLACK) ? whiteCaptured : blackCaptured;
  bool winByCaptureThreat = false;

  if (opponentLostStones >= 8) {
    for (int y = 0; y < BOARD_SIZE && !winByCaptureThreat; y++) {
      for (int x = 0; x < BOARD_SIZE && !winByCaptureThreat; x++) {
        if (grid[y][x] == Stone::EMPTY) {
          Coord testMove = {x, y};
          if (countCapturedPairs(testMove, currentPlayer, grid) > 0) {
            winByCaptureThreat = true;
          }
        }
      }
    }
  }

  if (winByAlignment || winByCapture || winByCaptureThreat) {
    winner = currentPlayer;
    setStone(previousOutlineCell, Stone::EMPTY);
    previousOutlineCell = {-1, -1};
  } else {
    currentPlayer =
        (currentPlayer == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
  }

  auto now = std::chrono::steady_clock::now();
  auto thinkDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
      now - aiThinkStartTime);
  aiThinkTime = thinkDuration.count();

  timer.calculateTimeSpentOnMove(aiColor);

  timer.resetTimer();

  // Print AI move statistics
  int totalNodesEvaluated = 0;
  for (int y = 0; y < BOARD_SIZE; y++) {
    for (int x = 0; x < BOARD_SIZE; x++) {
      totalNodesEvaluated += lastEvaluationHeatmap[y][x];
    }
  }

  std::cout << "AI (" << (aiPlayer == Stone::BLACK ? "BLACK" : "WHITE")
            << "): " << aiResult.move << " | Score: " << aiResult.score
            << " | Time: " << aiThinkTime << "ms | Nodes: " << totalNodesEvaluated << std::endl;
  std::cout << "Captures: " << capturedPairs << " | Total - B:" << blackCaptured
            << " W:" << whiteCaptured << " | Moves - B:" << blackMoveCount
            << " W:" << whiteMoveCount << std::endl;

  isAiThinking = false;
  redraw();
  }
  catch (const std::bad_alloc& e) {
    std::cerr << "AI Error: Out of memory - " << e.what() << std::endl;
    isAiThinking = false;
    redraw();
  }
  catch (const std::exception& e) {
    std::cerr << "AI Error: Exception occurred - " << e.what() << std::endl;
    isAiThinking = false;
    redraw();
  }
  catch (...) {
    std::cerr << "AI Error: Unknown exception occurred" << std::endl;
    isAiThinking = false;
    redraw();
  }
}

bool GomokuBoard::clickedModeButton(int x, int y) {
  int buttonX = OFFSET + BOARD_WIDTH + 10;
  int buttonY = 50;
  int buttonW = 70;
  int buttonH = 35;

  if (x >= buttonX && x <= buttonX + buttonW && y >= buttonY &&
      y <= buttonY + buttonH) {
    if (gameMode != GameMode::TWO_PLAYER) {
      gameMode = GameMode::TWO_PLAYER;
      return true;
    }
    return false;
  }

  if (x >= buttonX && x <= buttonX + buttonW && y >= buttonY + buttonH + 5 &&
      y <= buttonY + 2 * buttonH + 5) {
    if (gameMode != GameMode::AI_VS_HUMAN) {
      gameMode = GameMode::AI_VS_HUMAN;
      aiColor = Stone::WHITE;
      return true;
    }
    return false;
  }

  // Reset button
  if (x >= buttonX && x <= buttonX + buttonW &&
      y >= buttonY + 2 * (buttonH + 5) && y <= buttonY + 3 * buttonH + 10) {
    reset();
    redraw();
    return false;
  }

  // Hint button (Two Player mode) or Heatmap button (AI vs Human mode)
  if (x >= buttonX && x <= buttonX + buttonW &&
      y >= buttonY + 3 * (buttonH + 5) && y <= buttonY + 4 * buttonH + 15) {
    if (gameMode == GameMode::TWO_PLAYER && winner == Stone::EMPTY) {
      // Toggle suggestion
      showSuggestion = !showSuggestion;
      if (showSuggestion) {
        updateSuggestion();
      }
      redraw();
      return false;
    }
    if (gameMode == GameMode::AI_VS_HUMAN) {
      // Toggle heatmap
      showHeatmap = !showHeatmap;
      if (showHeatmap) {
        heatmapNeedsRedraw = true;
      }
      redraw();
      return false;
    }
  }

  return false;
}

void GomokuBoard::updateSuggestion() {
  try {
    if (gameMode != GameMode::TWO_PLAYER || winner != Stone::EMPTY) {
      return;
    }

    Stone opponent =
        (currentPlayer == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
    Minimax m(currentPlayer, opponent);
    Coord lastMove = {-1, -1};
    suggestedMove = m.minimax(grid, lastMove, 10, currentPlayer, opponent);
    suggestionForPlayer = currentPlayer;
  }
  catch (const std::bad_alloc& e) {
    std::cerr << "Suggestion Error: Out of memory - " << e.what() << std::endl;
    showSuggestion = false;
  }
  catch (const std::exception& e) {
    std::cerr << "Suggestion Error: Exception occurred - " << e.what() << std::endl;
    showSuggestion = false;
  }
  catch (...) {
    std::cerr << "Suggestion Error: Unknown exception occurred" << std::endl;
    showSuggestion = false;
  }
}
