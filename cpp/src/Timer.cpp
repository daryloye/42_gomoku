#include "Gomoku.hpp"

Timer::Timer()
{
  resetAll();
}

Timer::~Timer()
{}

void Timer::resetAll()
{
  _totalBlackTime = 0.0f;
  _totalWhiteTime = 0.0f;
  _lastMoveTime = 0.0f;
  _lastBlackMoveTime = 0.0f;
  _lastWhiteMoveTime = 0.0f;
  _moveStartTime = std::chrono::steady_clock::now();
}

void Timer::resetTimer()
{
  _moveStartTime = std::chrono::steady_clock::now();
}

void Timer::calculateTimeSpentOnMove(Stone currentPlayer)
{
  auto now = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - _moveStartTime);
  _lastMoveTime = duration.count();

  if (currentPlayer == Stone::BLACK) {
    _lastBlackMoveTime = _lastMoveTime;
    _totalBlackTime += _lastMoveTime;
  } else {
    _lastWhiteMoveTime = _lastMoveTime;
    _totalWhiteTime += _lastMoveTime;
  }
}
