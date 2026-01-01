#ifndef BOARD_HPP
#define BOARD_HPP

// https://www.fltk.org/doc-1.4/classFl__Window.html
class GomokuBoard : public Fl_Window
{
public:
  GomokuBoard();
  ~GomokuBoard();

  void draw() override;
  int handle(int event) override;

private:
  Timer timer;
  std::array<std::array<Stone, BOARD_SIZE>, BOARD_SIZE> grid;

  Stone currentPlayer;
  Stone winner;
  Coord previousOutlineCell;

  bool fontsInitialized = false;

  int blackMoveCount;
  int whiteMoveCount;

  GameMode gameMode = GameMode::TWO_PLAYER;
  Stone aiColor = Stone::WHITE;
  bool isAiThinking = false;
  std::chrono::steady_clock::time_point aiThinkStartTime;
  float aiThinkTime = 0.0f;

  GameRules gameRules;

  int blackCaptured = 0;
  int whiteCaptured = 0;

  Coord illegalMoveCell = {-1, -1};
  bool hasIllegalMove = false;
  std::array<bool, 4> illegalDirections = {false, false, false, false};

  MinimaxResult suggestedMove = {-1000000000, {-1, -1}};
  Stone suggestionForPlayer = Stone::EMPTY;
  bool showSuggestion = false;

  CountGrid lastEvaluationHeatmap;
  bool showHeatmap = false;
  bool heatmapNeedsRedraw = false;

  void reset();
  Stone getStone(Coord cell) const;
  void setStone(Coord cell, Stone p);
  bool checkWin(Coord cell, Stone stone) const;
  void drawBoard();
  void drawUI();
  void drawModeButtons();
  void makeAIMove();
  bool clickedModeButton(int x, int y);
  void handleSwapDecision(bool acceptSwap);
  void handleSwap2Decision(int choice);
  void analyzeDoubleThree(Coord move, Stone colour,
                          std::array<bool, 4> &directions);
  void updateSuggestion();
};

#endif
