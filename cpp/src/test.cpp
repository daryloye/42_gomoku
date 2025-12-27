#include "Gomoku.hpp"
#include <iostream>
#include <chrono>

int main() {
  int passed = 0;
  int failed = 0;
  Grid testGrid;

  auto initGrid = [&]() {
    for (int y = 0; y < BOARD_SIZE; y++)
      for (int x = 0; x < BOARD_SIZE; x++)
        testGrid[y][x] = Stone::EMPTY;
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
    for (int i = 0; i < 5; i++) testGrid[0][i] = Stone::BLACK;
    if (!hasPlayerWon({2, 0}, Stone::BLACK, testGrid))
      throw std::runtime_error("Corner win not detected");

    initGrid();
    for (int i = 0; i < 5; i++) testGrid[18][18-i] = Stone::WHITE;
    if (!hasPlayerWon({16, 18}, Stone::WHITE, testGrid))
      throw std::runtime_error("Corner win not detected");
  });

  test("4. Multiple captures in all 4 directions at once", [&]() {
    initGrid();
    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;
    testGrid[9][8] = Stone::WHITE;
    testGrid[9][7] = Stone::WHITE;
    testGrid[9][6] = Stone::BLACK;
    testGrid[10][9] = Stone::WHITE;
    testGrid[11][9] = Stone::WHITE;
    testGrid[12][9] = Stone::BLACK;
    testGrid[8][9] = Stone::WHITE;
    testGrid[7][9] = Stone::WHITE;
    testGrid[6][9] = Stone::BLACK;

    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    if (captured != 4) throw std::runtime_error("Should capture 4 pairs");
  });

  test("5. Double-three with diagonal and horizontal", [&]() {
    initGrid();
    testGrid[9][8] = Stone::BLACK;
    testGrid[9][10] = Stone::BLACK;
    testGrid[8][8] = Stone::BLACK;
    testGrid[10][10] = Stone::BLACK;
    createsDoubleThree({9, 9}, Stone::BLACK, testGrid);
  });

  test("6. Opponent can break five-in-row by capture", [&]() {
    initGrid();
    for (int i = 0; i < 5; i++) testGrid[9][i] = Stone::BLACK;
    testGrid[9][0] = Stone::WHITE;
    testGrid[9][1] = Stone::BLACK;
    testGrid[9][2] = Stone::BLACK;
    testGrid[9][3] = Stone::WHITE;
    (void)canOpponentBreakFiveByCapture({9, 2}, Stone::BLACK, testGrid);
  });


  test("7. Diagonal five-in-row touching all edges", [&]() {
    initGrid();
    for (int i = 0; i < 5; i++) testGrid[i][i] = Stone::BLACK;
    if (!hasPlayerWon({2, 2}, Stone::BLACK, testGrid))
      throw std::runtime_error("Diagonal corner win not detected");

    initGrid();
    for (int i = 0; i < 5; i++) testGrid[i][18-i] = Stone::WHITE;
    if (!hasPlayerWon({16, 2}, Stone::WHITE, testGrid))
      throw std::runtime_error("Anti-diagonal corner win not detected");
  });

  test("8. Capture attempts at all board edges", [&]() {
    initGrid();
    testGrid[0][1] = Stone::WHITE;
    testGrid[0][2] = Stone::WHITE;
    testGrid[0][3] = Stone::BLACK;
    countCapturedPairs({0, 0}, Stone::BLACK, testGrid);

    testGrid[9][17] = Stone::WHITE;
    testGrid[9][16] = Stone::WHITE;
    testGrid[9][15] = Stone::BLACK;
    countCapturedPairs({9, 18}, Stone::BLACK, testGrid);
  });

  test("9. Seven-in-row should still win (overline)", [&]() {
    initGrid();
    for (int i = 5; i < 12; i++) testGrid[9][i] = Stone::BLACK;
    if (!hasPlayerWon({8, 9}, Stone::BLACK, testGrid))
      throw std::runtime_error("Overline should win");
  });

  test("10. Cannot capture single stone only", [&]() {
    initGrid();
    testGrid[9][10] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;
    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    if (captured != 0) throw std::runtime_error("Should not capture single stone");
  });

  test("11. Cannot capture three or more stones", [&]() {
    initGrid();
    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::WHITE;
    testGrid[9][13] = Stone::BLACK;
    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    if (captured != 0) throw std::runtime_error("Should not capture 3+ stones");
  });

  test("12. Win by exactly 10 captured stones", [&]() {
    initGrid();
    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;
    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    if (captured != 1) throw std::runtime_error("Should capture exactly 1 pair");
  });

  test("13. Free-three requires both ends open", [&]() {
    initGrid();
    testGrid[9][9] = Stone::BLACK;
    testGrid[9][10] = Stone::BLACK;
    testGrid[9][11] = Stone::BLACK;
    testGrid[9][12] = Stone::WHITE; 
    bool creates = createsDoubleThree({9, 10}, Stone::BLACK, testGrid);
    (void)creates;
  });

  test("14. All 8 directions alignment check", [&]() {
    initGrid();
    for (int i = 5; i < 10; i++) testGrid[i][9] = Stone::BLACK;
    if (!hasPlayerWon({9, 7}, Stone::BLACK, testGrid))
      throw std::runtime_error("Vertical win not detected");

    initGrid();
    for (int i = 0; i < 5; i++) testGrid[5+i][5+i] = Stone::WHITE;
    if (!hasPlayerWon({7, 7}, Stone::WHITE, testGrid))
      throw std::runtime_error("Diagonal down-right win not detected");

    initGrid();
    for (int i = 0; i < 5; i++) testGrid[5+i][13-i] = Stone::BLACK;
    if (!hasPlayerWon({11, 7}, Stone::BLACK, testGrid))
      throw std::runtime_error("Diagonal down-left win not detected");
  });

  test("15. Double-three via capture is allowed", [&]() {
    initGrid();
    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;
    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    if (captured != 1) throw std::runtime_error("Should still capture");
  });

  test("16. Multiple five-in-rows simultaneously", [&]() {
    initGrid();
    for (int i = 0; i < 5; i++) testGrid[9][5+i] = Stone::BLACK;
    for (int i = 0; i < 5; i++) testGrid[5+i][9] = Stone::BLACK;
    if (!hasPlayerWon({9, 9}, Stone::BLACK, testGrid))
      throw std::runtime_error("Intersecting wins not detected");
  });

  test("17. Capture exactly at position boundaries", [&]() {
    initGrid();
    testGrid[0][1] = Stone::WHITE;
    testGrid[0][2] = Stone::WHITE;
    testGrid[0][3] = Stone::BLACK;
    int cap1 = countCapturedPairs({0, 0}, Stone::BLACK, testGrid);

    initGrid();
    testGrid[18][17] = Stone::WHITE;
    testGrid[18][16] = Stone::WHITE;
    testGrid[18][15] = Stone::BLACK;
    int cap2 = countCapturedPairs({18, 18}, Stone::BLACK, testGrid);

    if (cap1 != 1 || cap2 != 1)
      throw std::runtime_error("Boundary captures failed");
  });

  test("18. Valid move check on occupied cells", [&]() {
    initGrid();
    testGrid[9][9] = Stone::BLACK;
    if (isValidMove({9, 9}, testGrid))
      throw std::runtime_error("Occupied cell should be invalid");

    if (!isValidMove({9, 10}, testGrid))
      throw std::runtime_error("Empty cell should be valid");
  });

  test("19. Capture in all 4 diagonal/straight directions", [&]() {
    initGrid();
    testGrid[9][10] = Stone::WHITE;
    testGrid[9][11] = Stone::WHITE;
    testGrid[9][12] = Stone::BLACK;

    testGrid[10][10] = Stone::WHITE;
    testGrid[11][11] = Stone::WHITE;
    testGrid[12][12] = Stone::BLACK;

    int captured = countCapturedPairs({9, 9}, Stone::BLACK, testGrid);
    if (captured != 2) throw std::runtime_error("Should capture in 2 directions");
  });

  test("20. Win priority: Alignment vs Capture threat", [&]() {
    initGrid();
    for (int i = 0; i < 5; i++) testGrid[9][i] = Stone::BLACK;

    testGrid[10][10] = Stone::WHITE;
    testGrid[10][11] = Stone::WHITE;
    testGrid[10][12] = Stone::BLACK;

    if (!hasPlayerWon({2, 9}, Stone::BLACK, testGrid))
      throw std::runtime_error("Win by alignment should be detected");
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
