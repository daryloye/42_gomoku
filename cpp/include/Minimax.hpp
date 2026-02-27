#ifndef MINIMAX_HPP
#define MINIMAX_HPP

constexpr int MAX_DEPTH = 3;
constexpr float WIN_SCORE = 1.0e+10f;

class Minimax {
public:
  Minimax(const Stone aiColour, const Stone opponentColour);
  ~Minimax();

  MinimaxResult minimax(const Grid &grid, Coord lastMove, int depth,
                        Stone currentColour, Stone prevColour,
                        float alpha = -1.0e+10f, float beta = 1.0e+10f);

  const CountGrid &getEvaluationHeatmap() const { return _evaluationCount; }
  void resetHeatmap() {
    for (int y = 0; y < BOARD_SIZE; y++)
      for (int x = 0; x < BOARD_SIZE; x++)
        _evaluationCount[y][x] = 0;
  }

private:
  const Stone _aiColour;
  const Stone _opponentColour;
  CountGrid _evaluationCount;

  std::vector<Coord> getPossibleMoves(const Grid& grid);
  bool isWinningMove(const Grid& grid, const Coord& move, Stone stone);
  float evaluateMove(const Grid& grid, const Coord& move);
  void updateAlphaBeta(const MinimaxResult& result, const Coord& move, Stone currentColour,
                        MinimaxResult& best, float& alpha, float& beta);
};

#endif