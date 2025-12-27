#ifndef MINIMAX_HPP
#define MINIMAX_HPP

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
  static const int directions[8][2];
  const Stone _aiColour;
  const Stone _opponentColour;
  CountGrid _evaluationCount;

  bool hasOccupiedNeighbour(Coord cell, const Grid &grid);
  std::vector<Coord> getPossibleMoves(const Grid &grid);
  float evaluateMove(const Grid &grid);
  int countConsecutive(Coord pos, int dy, int dx, Stone colour, const Grid &grid);
  void evaluateDirection(Coord pos, int dy, int dx, Stone colour, const Grid &grid, int &score);
};

#endif