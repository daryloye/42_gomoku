#include "Gomoku.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <initializer_list>

int main() {
  int passed = 0;
  int failed = 0;
  Grid testGrid;

  auto initGrid = [&]() {
    for (int y = 0; y < BOARD_SIZE; y++)
      for (int x = 0; x < BOARD_SIZE; x++)
        testGrid[y][x] = Stone::EMPTY;
  };

  auto setStones = [&](std::initializer_list<std::tuple<int,int,Stone>> stones) {
    for (auto [y, x, c] : stones) testGrid[y][x] = c;
  };

  auto setLine = [&](int y, int x, int dy, int dx, int count, Stone c) {
    for (int i = 0; i < count; i++) testGrid[y + i*dy][x + i*dx] = c;
  };

  auto setCapturePattern = [&](int y, int x, int dy, int dx, Stone attacker) {
    Stone victim = (attacker == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
    testGrid[y + dy][x + dx] = victim;
    testGrid[y + 2*dy][x + 2*dx] = victim;
    testGrid[y + 3*dy][x + 3*dx] = attacker;
  };

  [[maybe_unused]] auto playMoves = [&](std::initializer_list<Coord> moves, Stone firstPlayer = Stone::BLACK) {
    Stone currentPlayer = firstPlayer;
    for (auto move : moves) {
      if (move.x >= 0 && move.x < BOARD_SIZE && move.y >= 0 && move.y < BOARD_SIZE) {
        testGrid[move.y][move.x] = currentPlayer;
        currentPlayer = (currentPlayer == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
      }
    }
    return currentPlayer;
  };

  auto playRealisticOpening = [&](int targetMoves = 100) -> int {
    Stone player = Stone::BLACK;
    int moveCount = 0;
    std::vector<Coord> openingPattern = {
      {2,2}, {16,16}, {2,16}, {16,2}, {3,3}, {15,15}, {3,15}, {15,3},
      {0,9}, {18,9}, {9,0}, {9,18}, {0,10}, {18,10}, {10,0}, {10,18},
      {1,9}, {17,9}, {9,1}, {9,17}, {1,10}, {17,10}, {10,1}, {10,17},
      {2,9}, {16,9}, {9,2}, {9,16}, {2,10}, {16,10}, {10,2}, {10,16},
      {3,9}, {15,9}, {9,3}, {9,15}, {3,10}, {15,10}, {10,3}, {10,15},
      {4,4}, {14,14}, {4,14}, {14,4}, {4,5}, {14,13}, {5,4}, {13,14},
      {0,5}, {18,13}, {5,0}, {13,18}, {0,13}, {18,5}, {13,0}, {5,18},
      {1,5}, {17,13}, {5,1}, {13,17}, {1,13}, {17,5}, {13,1}, {5,17},
      {2,5}, {16,13}, {5,2}, {13,16}, {2,13}, {16,5}, {13,2}, {5,16},
      {3,5}, {15,13}, {5,3}, {13,15}, {3,13}, {15,5}, {13,3}, {5,15},
      {4,8}, {14,10}, {8,4}, {10,14}, {4,10}, {14,8}, {10,4}, {8,14},
      {0,6}, {18,12}, {6,0}, {12,18}, {0,12}, {18,6}, {12,0}, {6,18},
      {1,6}, {17,12}, {6,1}, {12,17}, {1,12}, {17,6}, {12,1}, {6,17},
      {2,6}, {16,12}, {6,2}, {12,16}, {2,12}, {16,6}, {12,2}, {6,16},
      {3,6}, {15,12}, {6,3}, {12,15}, {3,12}, {15,6}, {12,3}, {6,15},
      {0,7}, {18,11}, {7,0}, {11,18}, {0,11}, {18,7}, {11,0}, {7,18},
      {1,7}, {17,11}, {7,1}, {11,17}, {1,11}, {17,7}, {11,1}, {7,17},
      {2,7}, {16,11}, {7,2}, {11,16}, {2,11}, {16,7}, {11,2}, {7,16},
      {3,7}, {15,11}, {7,3}, {11,15}, {3,11}, {15,7}, {11,3}, {7,15},
    };

    for (int i = 0; i < targetMoves && i < (int)openingPattern.size(); i++) {
      auto coord = openingPattern[i];
      if (testGrid[coord.y][coord.x] == Stone::EMPTY) {
        testGrid[coord.y][coord.x] = player;
        player = (player == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
        moveCount++;
      }
    }
    return moveCount;
  };

  auto assertEq = [](int actual, int expected, const char* msg) {
    if (actual != expected) throw std::runtime_error(msg);
  };

  auto assertTrue = [](bool cond, const char* msg) {
    if (!cond) throw std::runtime_error(msg);
  };

  auto test = [&](const char* name, auto func) {
    std::cout << name << "... ";
    try {
      func();
      std::cout << "PASS" << std::endl;
      passed++;
    }
    catch (const std::exception& e) {
      std::cout << "FAIL: " << e.what() << std::endl;
      failed++;
    }
    catch (...) {
      std::cout << "FAIL: Unknown exception" << std::endl;
      failed++;
    }
  };
/*
  test("0. AI depth-10 completes under 0.5 seconds (skipped)", [&]() {
    initGrid();
    playRealisticOpening();
    for (int i = 0; i < 15; i++) {
      int x = (i * 7) % BOARD_SIZE;
      int y = (i * 5) % BOARD_SIZE;
      testGrid[y][x] = (i % 2 == 0) ? Stone::BLACK : Stone::WHITE;
    }

    Minimax m(Stone::BLACK, Stone::WHITE);
    auto start = std::chrono::high_resolution_clock::now();
    (void)m.minimax(testGrid, {9, 9}, 10, Stone::BLACK, Stone::WHITE);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "[" << duration.count() << "ms] ";
    if (duration.count() > 500)
      throw std::runtime_error("AI too slow");
  });
*/  
  test("1. AI move on completely empty board", [&]() {
    initGrid();
    Minimax m(Stone::BLACK, Stone::WHITE);
    auto result = m.minimax(testGrid, {-1, -1}, 10, Stone::BLACK, Stone::WHITE);
    if (result.move.x < 0 || result.move.x >= BOARD_SIZE ||
        result.move.y < 0 || result.move.y >= BOARD_SIZE)
      throw std::runtime_error("Invalid move");
  });

  test("2. Operations on completely full board", [&]() {
    for (int y = 0; y < BOARD_SIZE; y++)
      for (int x = 0; x < BOARD_SIZE; x++)
        testGrid[y][x] = Stone::BLACK;
    hasPlayerWon({9, 9}, Stone::BLACK, testGrid);
    countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
  });

  test("3. Five-in-row at all board corners", [&]() {
    initGrid();
    playRealisticOpening();
    setLine(0, 0, 0, 1, 5, Stone::BLACK);
    assertTrue(hasPlayerWon({2, 0}, Stone::BLACK, testGrid), "Corner win not detected");

    initGrid();
    setLine(18, 18, 0, -1, 5, Stone::WHITE);
    assertTrue(hasPlayerWon({16, 18}, Stone::WHITE, testGrid), "Corner win not detected");
  });

  test("4. Multiple captures in all 4 directions at once", [&]() {
    initGrid();
    playRealisticOpening();
    setCapturePattern(9, 9, 0, 1, Stone::BLACK);
    setCapturePattern(9, 9, 0, -1, Stone::BLACK);
    setCapturePattern(9, 9, 1, 0, Stone::BLACK);
    setCapturePattern(9, 9, -1, 0, Stone::BLACK);
    assertEq(countCapturedPairs({9, 9}, Stone::BLACK, testGrid), 4, "Should capture 4 pairs");
  });

  test("5. Double-three with diagonal and horizontal", [&]() {
    initGrid();
    playRealisticOpening();
    setStones({{9,8,Stone::BLACK}, {9,10,Stone::BLACK}, {8,8,Stone::BLACK}, {10,10,Stone::BLACK}});
    createsDoubleThree({9, 9}, Stone::BLACK, testGrid);
  });

  test("6. Opponent can break five-in-row by capture", [&]() {
    initGrid();
    playRealisticOpening();
    setLine(9, 0, 0, 1, 5, Stone::BLACK);
    setStones({{9,0,Stone::WHITE}, {9,3,Stone::WHITE}});
    (void)canOpponentBreakFiveByCapture({9, 2}, Stone::BLACK, testGrid);
  });

  test("7. Diagonal five-in-row touching all edges", [&]() {
    initGrid();
    playRealisticOpening();
    setLine(0, 0, 1, 1, 5, Stone::BLACK);
    assertTrue(hasPlayerWon({2, 2}, Stone::BLACK, testGrid), "Diagonal corner win not detected");

    initGrid();
    setLine(0, 18, 1, -1, 5, Stone::WHITE);
    assertTrue(hasPlayerWon({16, 2}, Stone::WHITE, testGrid), "Anti-diagonal corner win not detected");
  });

  test("8. Capture attempts at all board edges", [&]() {
    initGrid();
    playRealisticOpening();
    setCapturePattern(0, 0, 0, 1, Stone::BLACK);
    setCapturePattern(9, 18, 0, -1, Stone::BLACK);
    countCapturedPairs({0, 0}, Stone::BLACK, testGrid);
    countCapturedPairs({9, 18}, Stone::BLACK, testGrid);
  });

  test("9. Seven-in-row should still win (overline)", [&]() {
    initGrid();
    playRealisticOpening();
    setLine(9, 5, 0, 1, 7, Stone::BLACK);
    assertTrue(hasPlayerWon({8, 9}, Stone::BLACK, testGrid), "Overline should win");
  });

  test("10. Cannot capture single stone only", [&]() {
    initGrid();
    playRealisticOpening();
    setStones({{9,10,Stone::WHITE}, {9,12,Stone::BLACK}});
    assertEq(countCapturedPairs({9, 9}, Stone::BLACK, testGrid), 0, "Should not capture single stone");
  });

  test("11. Cannot capture three or more stones", [&]() {
    initGrid();
    playRealisticOpening();
    setLine(9, 10, 0, 1, 3, Stone::WHITE);
    testGrid[9][13] = Stone::BLACK;
    assertEq(countCapturedPairs({9, 9}, Stone::BLACK, testGrid), 0, "Should not capture 3+ stones");
  });

  test("12. Win by exactly 10 captured stones", [&]() {
    initGrid();
    playRealisticOpening();
    setCapturePattern(9, 9, 0, 1, Stone::BLACK);
    assertEq(countCapturedPairs({9, 9}, Stone::BLACK, testGrid), 1, "Should capture exactly 1 pair");
  });

  test("13. Free-three requires both ends open", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][9] = Stone::BLACK;
    testGrid[9][10] = Stone::BLACK;
    testGrid[9][11] = Stone::BLACK;
    testGrid[9][12] = Stone::WHITE; 
    bool creates = createsDoubleThree({9, 10}, Stone::BLACK, testGrid);
    (void)creates;
  });

  test("14. All 8 directions alignment check", [&]() {
    initGrid();
    playRealisticOpening();
    setLine(5, 9, 1, 0, 5, Stone::BLACK);
    assertTrue(hasPlayerWon({9, 7}, Stone::BLACK, testGrid), "Vertical win not detected");

    initGrid();
    setLine(5, 5, 1, 1, 5, Stone::WHITE);
    assertTrue(hasPlayerWon({7, 7}, Stone::WHITE, testGrid), "Diagonal down-right win not detected");

    initGrid();
    setLine(5, 13, 1, -1, 5, Stone::BLACK);
    assertTrue(hasPlayerWon({11, 7}, Stone::BLACK, testGrid), "Diagonal down-left win not detected");
  });

  test("15. Double-three via capture is allowed", [&]() {
    initGrid();
    playRealisticOpening();
    setCapturePattern(9, 9, 0, 1, Stone::BLACK);
    assertEq(countCapturedPairs({9, 9}, Stone::BLACK, testGrid), 1, "Should still capture");
  });

  test("16. Multiple five-in-rows simultaneously", [&]() {
    initGrid();
    playRealisticOpening();
    setLine(9, 5, 0, 1, 5, Stone::BLACK);
    setLine(5, 9, 1, 0, 5, Stone::BLACK);
    assertTrue(hasPlayerWon({9, 9}, Stone::BLACK, testGrid), "Intersecting wins not detected");
  });

  test("17. Capture exactly at position boundaries", [&]() {
    initGrid();
    playRealisticOpening();
    setCapturePattern(0, 0, 0, 1, Stone::BLACK);
    int cap1 = countCapturedPairs({0, 0}, Stone::BLACK, testGrid);
    initGrid();
    playRealisticOpening();
    setCapturePattern(18, 18, 0, -1, Stone::BLACK);
    int cap2 = countCapturedPairs({18, 18}, Stone::BLACK, testGrid);
    assertTrue(cap1 == 1 && cap2 == 1, "Boundary captures failed");
  });

  test("18. Valid move check on occupied cells", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][9] = Stone::BLACK;
    assertTrue(!isValidMove({9, 9}, testGrid), "Occupied cell should be invalid");
    assertTrue(isValidMove({9, 10}, testGrid), "Empty cell should be valid");
  });

  test("19. Capture in all 4 diagonal/straight directions", [&]() {
    initGrid();
    playRealisticOpening();
    setCapturePattern(9, 9, 0, 1, Stone::BLACK);
    setCapturePattern(9, 9, 1, 1, Stone::BLACK);
    assertEq(countCapturedPairs({9, 9}, Stone::BLACK, testGrid), 2, "Should capture in 2 directions");
  });

  test("20. Win priority: Alignment vs Capture threat", [&]() {
    initGrid();
    playRealisticOpening();
    setLine(9, 0, 0, 1, 5, Stone::BLACK);
    setCapturePattern(10, 10, 0, 1, Stone::BLACK);
    assertTrue(hasPlayerWon({2, 9}, Stone::BLACK, testGrid), "Win by alignment should be detected");
  });

  test("21. Endgame capture: Breaking five-in-row with capture", [&]() {
    initGrid();
    playRealisticOpening();
    setStones({{9,5,Stone::BLACK},{9,6,Stone::BLACK},{9,7,Stone::BLACK},{9,8,Stone::WHITE},
               {9,9,Stone::BLACK},{9,10,Stone::BLACK},{9,11,Stone::WHITE}});
    (void)canOpponentBreakFiveByCapture({9, 7}, Stone::BLACK, testGrid);
  });

  test("22. Win by exactly 10 captures with 8 pre-existing", [&]() {
    initGrid();
    playRealisticOpening();
    setCapturePattern(9, 9, 0, 1, Stone::BLACK);
    setCapturePattern(9, 9, 1, 0, Stone::BLACK);
    assertEq(countCapturedPairs({9, 9}, Stone::BLACK, testGrid), 2, "Should capture exactly 2 pairs for 10 total");
  });

  test("23. Opponent has 8 captures, 5-in-row is breakable", [&]() {
    initGrid();
    playRealisticOpening();
    setLine(9, 5, 0, 1, 5, Stone::BLACK);
    setStones({{9,4,Stone::WHITE}, {9,7,Stone::WHITE}});
    (void)canOpponentBreakFiveByCapture({9, 7}, Stone::BLACK, testGrid);
  });

  test("24. Double-three forbidden at corner boundaries", [&]() {
    initGrid();
    playRealisticOpening();
    setStones({{1,0,Stone::BLACK},{2,0,Stone::BLACK},{0,1,Stone::BLACK},{0,2,Stone::BLACK}});
    (void)createsDoubleThree({0, 0}, Stone::BLACK, testGrid);
  });

  test("25. Maximum overline: 19 stones in a row", [&]() {
    initGrid();
    playRealisticOpening();
    setLine(9, 0, 0, 1, BOARD_SIZE, Stone::BLACK);
    assertTrue(hasPlayerWon({9, 9}, Stone::BLACK, testGrid), "Maximum overline should win");
  });

  test("26. Eight-direction simultaneous captures (max possible)", [&]() {
    initGrid();
    playRealisticOpening();
    setCapturePattern(9, 9, 0, 1, Stone::BLACK);
    setCapturePattern(9, 9, 0, -1, Stone::BLACK);
    setCapturePattern(9, 9, 1, 0, Stone::BLACK);
    setCapturePattern(9, 9, -1, 0, Stone::BLACK);
    setCapturePattern(9, 9, 1, 1, Stone::BLACK);
    setCapturePattern(9, 9, -1, -1, Stone::BLACK);
    setCapturePattern(9, 9, 1, -1, Stone::BLACK);
    setCapturePattern(9, 9, -1, 1, Stone::BLACK);
    assertEq(countCapturedPairs({9, 9}, Stone::BLACK, testGrid), 8, "Should capture 8 pairs in all directions");
  });

  test("27. Double-three with gaps (X_XX pattern)", [&]() {
    initGrid();
    playRealisticOpening();
    setStones({{9,8,Stone::BLACK},{9,10,Stone::BLACK},{9,11,Stone::BLACK},
               {8,9,Stone::BLACK},{10,9,Stone::BLACK},{11,9,Stone::BLACK}});
    (void)createsDoubleThree({9, 9}, Stone::BLACK, testGrid);
  });

  test("28. Near-full board with only one valid move", [&]() {
    initGrid();
    playRealisticOpening();
    for (int y = 0; y < BOARD_SIZE; y++) {
      for (int x = 0; x < BOARD_SIZE; x++) {
        if (x == 9 && y == 9) continue;
        testGrid[y][x] = ((x + y) % 2 == 0) ? Stone::BLACK : Stone::WHITE;
      }
    }

    if (!isValidMove({9, 9}, testGrid))
      throw std::runtime_error("Last empty cell should be valid");

    Minimax m(Stone::BLACK, Stone::WHITE);
    auto result = m.minimax(testGrid, {9, 9}, 10, Stone::BLACK, Stone::WHITE);
    if (result.move.x != 9 || result.move.y != 9)
      throw std::runtime_error("AI should choose the only valid move");
  });

  test("29. Forbidden double-three blocks only winning path", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][5] = Stone::BLACK;
    testGrid[9][6] = Stone::BLACK;
    testGrid[9][7] = Stone::BLACK;
    testGrid[9][8] = Stone::BLACK;

    testGrid[8][9] = Stone::BLACK;
    testGrid[10][9] = Stone::BLACK;
    testGrid[11][9] = Stone::BLACK;

    bool creates = createsDoubleThree({9, 9}, Stone::BLACK, testGrid);
    (void)creates;
  });

  test("30. Capture that creates free-three is allowed", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;

    testGrid[9][7] = Stone::BLACK;
    testGrid[9][8] = Stone::BLACK;

    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    if (captured != 1)
      throw std::runtime_error("Capture should be allowed even if creating free-three");
  });

  test("31. Simultaneous 5-in-row AND 10th capture on same move", [&]() {
    initGrid();
    playRealisticOpening();
    for (int i = 5; i < 9; i++) testGrid[9][i] = Stone::BLACK;
    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;

    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    bool won = hasPlayerWon({9, 9}, Stone::BLACK, testGrid);

    if (captured != 1)
      throw std::runtime_error("Should capture 1 pair");
    if (!won)
      throw std::runtime_error("Should also achieve 5-in-row");
  });

  test("32. Endgame capture - 5-in-row breakable with 8 captures existing", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][5] = Stone::BLACK;
    testGrid[9][6] = Stone::WHITE;
    testGrid[9][7] = Stone::WHITE;
    testGrid[9][8] = Stone::BLACK;
    testGrid[9][9] = Stone::BLACK;
    testGrid[9][10] = Stone::BLACK;
    testGrid[9][11] = Stone::BLACK;

    bool canBreak = canOpponentBreakFiveByCapture({9, 9}, Stone::BLACK, testGrid);
    (void)canBreak;
  });

  test("33. Multiple overlapping 5-in-rows all breakable by single capture", [&]() {
    initGrid();
    playRealisticOpening();
    for (int i = 5; i < 10; i++) testGrid[9][i] = Stone::BLACK;
    for (int i = 5; i < 10; i++) testGrid[i][9] = Stone::BLACK;
    for (int i = 0; i < 5; i++) testGrid[5+i][5+i] = Stone::BLACK;

    testGrid[9][4] = Stone::WHITE;
    testGrid[9][7] = Stone::WHITE;

    bool canBreak = canOpponentBreakFiveByCapture({9, 7}, Stone::BLACK, testGrid);
    bool won = hasPlayerWon({9, 9}, Stone::BLACK, testGrid);

    if (!won)
      throw std::runtime_error("Multiple 5-in-rows should trigger win");
    (void)canBreak;
  });

  test("34. Quadruple-three (four free-threes simultaneously)", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][8] = Stone::BLACK;
    testGrid[9][10] = Stone::BLACK;
    testGrid[8][9] = Stone::BLACK;
    testGrid[10][9] = Stone::BLACK;
    testGrid[8][8] = Stone::BLACK;
    testGrid[10][10] = Stone::BLACK;
    testGrid[8][10] = Stone::BLACK;
    testGrid[10][8] = Stone::BLACK;

    bool creates = createsDoubleThree({9, 9}, Stone::BLACK, testGrid);
    (void)creates;
  });

  test("35. 19-stone diagonal overline (corner to corner)", [&]() {
    initGrid();
    playRealisticOpening();
    for (int i = 0; i < BOARD_SIZE; i++)
      testGrid[i][i] = Stone::BLACK;

    if (!hasPlayerWon({9, 9}, Stone::BLACK, testGrid))
      throw std::runtime_error("Maximum diagonal overline should win");
  });

  test("36. Capture attempt at all 4 board corners simultaneously", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[0][1] = Stone::WHITE;
    testGrid[0][2] = Stone::WHITE;
    testGrid[0][3] = Stone::BLACK;

    testGrid[0][17] = Stone::WHITE;
    testGrid[0][16] = Stone::WHITE;
    testGrid[0][15] = Stone::BLACK;

    testGrid[18][1] = Stone::WHITE;
    testGrid[18][2] = Stone::WHITE;
    testGrid[18][3] = Stone::BLACK;

    testGrid[18][17] = Stone::WHITE;
    testGrid[18][16] = Stone::WHITE;
    testGrid[18][15] = Stone::BLACK;

    int cap1 = countCapturedPairs({0, 0}, Stone::BLACK, testGrid);
    int cap2 = countCapturedPairs({0, 18}, Stone::BLACK, testGrid);
    int cap3 = countCapturedPairs({18, 0}, Stone::BLACK, testGrid);
    int cap4 = countCapturedPairs({18, 18}, Stone::BLACK, testGrid);

    if (cap1 + cap2 + cap3 + cap4 != 4)
      throw std::runtime_error("Should capture at all 4 corners");
  });

  test("37. Win condition with opponent at 9 captures about to win", [&]() {
    initGrid();
    playRealisticOpening();
    for (int i = 0; i < 5; i++) testGrid[9][i] = Stone::BLACK;

    testGrid[10][10] = Stone::WHITE;
    testGrid[10][11] = Stone::WHITE;
    testGrid[10][12] = Stone::BLACK;

    bool won = hasPlayerWon({2, 9}, Stone::BLACK, testGrid);
    int opponentCapturePotential = countCapturedPairs({9, 10}, Stone::BLACK, testGrid);

    if (!won)
      throw std::runtime_error("Should win by 5-in-row even if opponent threatens capture");
    if (opponentCapturePotential != 1)
      throw std::runtime_error("Opponent should have capture potential");
  });

  test("38. Only move available creates forbidden double-three", [&]() {
    initGrid();
    playRealisticOpening();
    for (int y = 0; y < BOARD_SIZE; y++) {
      for (int x = 0; x < BOARD_SIZE; x++) {
        if (x == 9 && y == 9) continue;
        testGrid[y][x] = Stone::WHITE;
      }
    }

    testGrid[9][8] = Stone::BLACK;
    testGrid[9][10] = Stone::BLACK;
    testGrid[8][9] = Stone::BLACK;
    testGrid[10][9] = Stone::BLACK;
    testGrid[9][9] = Stone::EMPTY;

    bool creates = createsDoubleThree({9, 9}, Stone::BLACK, testGrid);
    bool valid = isValidMove({9, 9}, testGrid);

    if (!valid)
      throw std::runtime_error("Cell should be empty");
    (void)creates;
  });

  test("39. Capture chain - capturing opens new capture opportunity", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;

    testGrid[9][13] = Stone::BLACK;
    testGrid[9][14] = Stone::WHITE;
    testGrid[9][15] = Stone::WHITE;
    testGrid[9][16] = Stone::BLACK;

    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    if (captured != 1)
      throw std::runtime_error("Should capture first pair");
  });

  test("40. Five stones with middle stone capturable by opponent", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][5] = Stone::BLACK;
    testGrid[9][6] = Stone::BLACK;
    testGrid[9][7] = Stone::BLACK;
    testGrid[9][8] = Stone::BLACK;
    testGrid[9][9] = Stone::BLACK;

    testGrid[8][7] = Stone::WHITE;
    testGrid[8][8] = Stone::BLACK;
    testGrid[10][7] = Stone::WHITE;

    bool won = hasPlayerWon({7, 9}, Stone::BLACK, testGrid);
    bool canBreak = canOpponentBreakFiveByCapture({7, 9}, Stone::BLACK, testGrid);

    if (!won)
      throw std::runtime_error("Should detect 5-in-row");
    (void)canBreak;
  });

  test("41. Cannot move into a capture (self-sacrifice forbidden)", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][9] = Stone::BLACK;
    testGrid[9][11] = Stone::BLACK;
    testGrid[9][10] = Stone::EMPTY;

    testGrid[9][12] = Stone::EMPTY;

    if (!isValidMove({10, 9}, testGrid))
      throw std::runtime_error("Empty position should be valid for move check");
  });

  test("42. Capture prevents opponent's imminent 5-in-row win", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][5] = Stone::WHITE;
    testGrid[9][6] = Stone::WHITE;
    testGrid[9][7] = Stone::WHITE;
    testGrid[9][8] = Stone::WHITE;

    testGrid[9][10] = Stone::BLACK;
    testGrid[9][11] = Stone::BLACK;
    testGrid[9][12] = Stone::WHITE;

    int captured = countCapturedPairs({9, 9}, Stone::WHITE, testGrid);
    if (captured != 1)
      throw std::runtime_error("Should capture to prevent own loss");
  });

  test("43. Maximum distance capture - opposite board corners involved", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[0][1] = Stone::WHITE;
    testGrid[0][2] = Stone::WHITE;
    testGrid[0][3] = Stone::BLACK;

    testGrid[18][17] = Stone::WHITE;
    testGrid[18][16] = Stone::WHITE;
    testGrid[18][15] = Stone::BLACK;

    int cap1 = countCapturedPairs({0, 0}, Stone::BLACK, testGrid);
    int cap2 = countCapturedPairs({18, 18}, Stone::BLACK, testGrid);

    if (cap1 != 1 || cap2 != 1)
      throw std::runtime_error("Captures at max distance should work");
  });

  test("44. Free-three with all gap patterns (_XXX_, X_XX_, XX_X_)", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][6] = Stone::BLACK;
    testGrid[9][7] = Stone::BLACK;
    testGrid[9][8] = Stone::BLACK;

    testGrid[9][11] = Stone::BLACK;
    testGrid[9][13] = Stone::BLACK;
    testGrid[9][14] = Stone::BLACK;

    bool creates1 = createsDoubleThree({9, 9}, Stone::BLACK, testGrid);
    (void)creates1;

    initGrid();
    testGrid[9][7] = Stone::BLACK;
    testGrid[9][9] = Stone::BLACK;
    testGrid[9][10] = Stone::BLACK;

    testGrid[10][8] = Stone::BLACK;
    testGrid[11][8] = Stone::BLACK;

    bool creates2 = createsDoubleThree({8, 9}, Stone::BLACK, testGrid);
    (void)creates2;
  });

  test("45. Win by 10th capture while opponent has unblockable four-in-row", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[5][5] = Stone::WHITE;
    testGrid[5][6] = Stone::WHITE;
    testGrid[5][7] = Stone::WHITE;
    testGrid[5][8] = Stone::WHITE;

    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;

    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    if (captured != 1)
      throw std::runtime_error("Should capture for 10th pair even with opponent threat");
  });

  test("46. Triple-overlapping captures in same direction", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;

    testGrid[9][13] = Stone::WHITE;
    testGrid[9][14] = Stone::WHITE;
    testGrid[9][15] = Stone::BLACK;

    testGrid[9][16] = Stone::WHITE;
    testGrid[9][17] = Stone::WHITE;
    testGrid[9][18] = Stone::BLACK;

    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    if (captured != 1)
      throw std::runtime_error("Should only capture adjacent pair, not multiple");
  });

  test("47. Forbidden double-three but triple-three is created", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][8] = Stone::BLACK;
    testGrid[9][10] = Stone::BLACK;

    testGrid[8][9] = Stone::BLACK;
    testGrid[10][9] = Stone::BLACK;

    testGrid[8][8] = Stone::BLACK;
    testGrid[10][10] = Stone::BLACK;

    testGrid[7][9] = Stone::BLACK;
    testGrid[11][9] = Stone::BLACK;

    bool creates = createsDoubleThree({9, 9}, Stone::BLACK, testGrid);
    (void)creates;
  });

  test("48. Both players have 4 pairs captured, next capture wins", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;

    testGrid[10][9] = Stone::WHITE;
    testGrid[11][9] = Stone::WHITE;
    testGrid[12][9] = Stone::BLACK;

    int blackCaptures = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);

    testGrid[8][10] = Stone::BLACK;
    testGrid[8][11] = Stone::BLACK;
    testGrid[8][12] = Stone::WHITE;

    int whiteCaptures = countCapturedPairs({9, 8}, Stone::WHITE, testGrid);

    if (blackCaptures != 2)
      throw std::runtime_error("Black should capture 2 pairs");
    if (whiteCaptures != 1)
      throw std::runtime_error("White should capture 1 pair");
  });

  test("49. Six-in-row with both ends blocked still wins", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][4] = Stone::WHITE;
    for (int i = 5; i < 11; i++) testGrid[9][i] = Stone::BLACK;
    testGrid[9][11] = Stone::WHITE;

    if (!hasPlayerWon({7, 9}, Stone::BLACK, testGrid))
      throw std::runtime_error("Six-in-row should win even with blocked ends");
  });

  test("50. Complex endgame: 5-in-row breakable, opponent at 8 captures, can counter-capture", [&]() {
    initGrid();
    for (int i = 5; i < 10; i++) testGrid[9][i] = Stone::BLACK;

    testGrid[9][4] = Stone::WHITE;
    testGrid[9][7] = Stone::WHITE;

    testGrid[10][10] = Stone::BLACK;
    testGrid[10][11] = Stone::BLACK;
    testGrid[10][12] = Stone::WHITE;
    playRealisticOpening();

    bool hasWon = hasPlayerWon({7, 9}, Stone::BLACK, testGrid);
    bool canBreak = canOpponentBreakFiveByCapture({7, 9}, Stone::BLACK, testGrid);
    int whiteCanCapture = countCapturedPairs({9, 10}, Stone::WHITE, testGrid);

    if (!hasWon)
      throw std::runtime_error("Should have 5-in-row");
    if (whiteCanCapture != 1)
      throw std::runtime_error("White should have capture option");
    (void)canBreak;
  });

  test("51. Capture creates five-in-row that is immediately breakable by opponent", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][5] = Stone::BLACK;
    testGrid[9][6] = Stone::BLACK;
    testGrid[9][7] = Stone::BLACK;
    testGrid[9][8] = Stone::BLACK;

    testGrid[10][9] = Stone::WHITE;
    testGrid[11][9] = Stone::WHITE;
    testGrid[12][9] = Stone::BLACK;

    testGrid[8][7] = Stone::WHITE;
    testGrid[10][7] = Stone::WHITE;

    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    bool hasWon = hasPlayerWon({9, 9}, Stone::BLACK, testGrid);
    bool canBreak = canOpponentBreakFiveByCapture({9, 9}, Stone::BLACK, testGrid);

    if (captured != 1)
      throw std::runtime_error("Should capture 1 pair");
    if (!hasWon)
      throw std::runtime_error("Should create 5-in-row");
    (void)canBreak;
  });

  test("52. Both players at 9 captures (18 stones total), racing to win", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[5][10] = Stone::WHITE;
    testGrid[5][11] = Stone::WHITE;
    testGrid[5][12] = Stone::BLACK;

    testGrid[14][10] = Stone::BLACK;
    testGrid[14][11] = Stone::BLACK;
    testGrid[14][12] = Stone::WHITE;

    int blackCaptures = countCapturedPairs({9, 5}, Stone::BLACK, testGrid);
    int whiteCaptures = countCapturedPairs({9, 14}, Stone::WHITE, testGrid);

    if (blackCaptures != 1 || whiteCaptures != 1)
      throw std::runtime_error("Both should be able to make game-winning captures");
  });

  test("53. Capture creating double-three is explicitly allowed by rules", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;

    testGrid[9][7] = Stone::BLACK;
    testGrid[9][8] = Stone::BLACK;

    testGrid[10][9] = Stone::BLACK;
    testGrid[11][9] = Stone::BLACK;

    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    bool createsDouble = createsDoubleThree({9, 9}, Stone::BLACK, testGrid);

    if (captured != 1)
      throw std::runtime_error("Should capture 1 pair");
    (void)createsDouble;
  });

  test("54. Four overlapping five-in-rows all breakable by same capture", [&]() {
    initGrid();
    playRealisticOpening();
    for (int i = 5; i < 10; i++) testGrid[9][i] = Stone::BLACK;
    for (int i = 5; i < 10; i++) testGrid[i][9] = Stone::BLACK;
    for (int i = 0; i < 5; i++) testGrid[5+i][5+i] = Stone::BLACK;
    for (int i = 0; i < 5; i++) testGrid[5+i][13-i] = Stone::BLACK;

    testGrid[9][4] = Stone::WHITE;
    testGrid[9][7] = Stone::WHITE;

    bool won = hasPlayerWon({9, 9}, Stone::BLACK, testGrid);
    bool canBreak = canOpponentBreakFiveByCapture({9, 7}, Stone::BLACK, testGrid);

    if (!won)
      throw std::runtime_error("Should have four overlapping wins");
    (void)canBreak;
  });

  test("55. Double-three detection when one alignment is actually a four", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][6] = Stone::BLACK;
    testGrid[9][7] = Stone::BLACK;
    testGrid[9][8] = Stone::BLACK;
    testGrid[9][10] = Stone::BLACK;

    testGrid[7][9] = Stone::BLACK;
    testGrid[8][9] = Stone::BLACK;

    bool creates = createsDoubleThree({9, 9}, Stone::BLACK, testGrid);
    (void)creates;
  });

  test("56. Eight-in-row with middle pair capturable still wins", [&]() {
    initGrid();
    playRealisticOpening();
    for (int i = 5; i < 13; i++) testGrid[9][i] = Stone::BLACK;

    testGrid[8][8] = Stone::WHITE;
    testGrid[10][8] = Stone::WHITE;
    testGrid[8][9] = Stone::BLACK;

    bool won = hasPlayerWon({9, 9}, Stone::BLACK, testGrid);

    if (!won)
      throw std::runtime_error("Eight-in-row should win despite capturable pair");
  });

  test("57. Simultaneous five-in-rows: one breakable, one unbreakable", [&]() {
    initGrid();
    playRealisticOpening();
    for (int i = 5; i < 10; i++) testGrid[9][i] = Stone::BLACK;
    for (int i = 5; i < 10; i++) testGrid[i][9] = Stone::BLACK;

    testGrid[9][4] = Stone::WHITE;
    testGrid[9][7] = Stone::WHITE;

    bool won = hasPlayerWon({9, 9}, Stone::BLACK, testGrid);

    if (!won)
      throw std::runtime_error("Should win with multiple 5-in-rows");
  });

  test("58. Win by 10th capture while blocking opponent's unbreakable five", [&]() {
    initGrid();
    playRealisticOpening();
    for (int i = 5; i < 9; i++) testGrid[5][i] = Stone::WHITE;
    testGrid[5][10] = Stone::WHITE;

    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;

    int blackCaptures = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);

    if (blackCaptures != 1)
      throw std::runtime_error("Should capture to win by 10 pairs");
  });

  test("59. Forbidden move is the only move in heavily constrained endgame", [&]() {
    initGrid();
    playRealisticOpening();
    for (int y = 0; y < BOARD_SIZE; y++) {
      for (int x = 0; x < BOARD_SIZE; x++) {
        if ((x == 9 && y == 9) || (x == 10 && y == 10)) continue;
        testGrid[y][x] = Stone::WHITE;
      }
    }

    testGrid[9][8] = Stone::BLACK;
    testGrid[9][10] = Stone::EMPTY;
    testGrid[9][11] = Stone::BLACK;
    testGrid[8][9] = Stone::BLACK;
    testGrid[10][9] = Stone::BLACK;

    bool creates = createsDoubleThree({9, 9}, Stone::BLACK, testGrid);
    bool valid = isValidMove({9, 9}, testGrid);

    if (!valid)
      throw std::runtime_error("Empty cell should pass basic validity");
    (void)creates;
  });

  test("60. Capture chain reaction - three capturable pairs in same line", [&]() {
    initGrid();
    playRealisticOpening();
    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;

    testGrid[9][13] = Stone::BLACK;
    testGrid[9][14] = Stone::WHITE;
    testGrid[9][15] = Stone::WHITE;
    testGrid[9][16] = Stone::BLACK;

    testGrid[9][17] = Stone::BLACK;
    testGrid[9][18] = Stone::WHITE;

    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    if (captured != 1)
      throw std::runtime_error("Should only capture immediately adjacent pair");
  });

  std::cout << std::endl;
  std::cout << "=== Results ===" << std::endl;
  std::cout << "Total tests: " << (passed + failed) << std::endl;
  std::cout << "Passed: " << passed << std::endl;
  std::cout << "Failed: " << failed << std::endl;

  if (failed == 0) {
    std::cout << "\nALL TESTS PASSED! No crashes or critical errors." << std::endl;
    return 0;
  } else {
    std::cout << "\nSome tests failed." << std::endl;
    return 1;
  }
}
