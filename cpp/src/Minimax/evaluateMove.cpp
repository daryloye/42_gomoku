#include "Gomoku.hpp"

static const int DIRS[4][2] = {
  {1, 0},  // horizontal
  {0, 1},  // vertical
  {1, 1},  // diag '\'
  {1, -1}  // diag '/'
};

static const std::map<std::string, float> SCORE_MAP = {
  {"_PPPP_",  100000.0f},    // open four
  {"OPPPP_",  50000.0f},     // half-open four
  {"_PPP_",   10000.0f},     // open three
  {"_PP_",    1000.0f}       // open two
};

static std::string getLine(const Grid& grid, const Coord& coord, int dx, int dy, Stone player) {
  const int R = 4;
  std::string line; 
  
  for (int i = -R; i <= R; i++) {
    int nx = coord.x + dx * i;
    int ny = coord.y + dy * i;
    
    if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE) {
      line.push_back('#');
    } else if (grid[ny][nx] == Stone::EMPTY) {
      line.push_back('_');
    } else if (grid[ny][nx] == player) {
      line.push_back('P');
    } else {
      line.push_back('O');
    }
  }
  return line;
}

static float evaluateForPlayer(const Grid& grid, const Coord& lastMove, Stone player) {
  float score = 0.0f;

  for (int i = 0; i < 4; i++) {
    int dx = DIRS[i][0], dy = DIRS[i][1];
    std::string line = getLine(grid, lastMove, dx, dy, player);

    for (const auto& [pattern, value] : SCORE_MAP) {
      if (line.find(pattern) != std::string::npos) {
        score += value;
      }
    }
  }

  return score;
}

float Minimax::evaluateMove(const Grid& grid, const Coord& lastMove) {
  if (lastMove.x < 0 || lastMove.x >= BOARD_SIZE || lastMove.y < 0 ||
      lastMove.y >= BOARD_SIZE) {
    return 0.0f;
  }

  const float aiScore = evaluateForPlayer(grid, lastMove, _aiColour);
  const float opponentScore = evaluateForPlayer(grid, lastMove, _opponentColour);

  // Positive scores favour AI, negative favour the opponent.
  return (aiScore - opponentScore);
}
