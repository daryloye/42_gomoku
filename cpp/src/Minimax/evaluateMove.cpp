#include "Gomoku.hpp"

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
    
    if (!isInBounds({nx, ny})) {
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

  for (const auto &dir : DIRECTIONS) {
    int dx = dir[0];
    int dy = dir[1];
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
  const float aiScore = evaluateForPlayer(grid, lastMove, _aiColour);
  const float opponentScore = evaluateForPlayer(grid, lastMove, _opponentColour);

  // Positive scores favour AI, negative favour the opponent.
  return (aiScore - opponentScore);
}
