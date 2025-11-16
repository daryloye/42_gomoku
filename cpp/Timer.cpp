#include "Gomoku.hpp"

Timer::Timer()
{
  resetAll();
}

Timer::~Timer()
{}

void Timer::resetAll()
{
  totalBlackTime = 0.0f;
  totalWhiteTime = 0.0f;
  lastMoveTime = 0.0f;
  moveStartTime = std::chrono::steady_clock::now();
}

void Timer::resetTimer()
{
  moveStartTime = std::chrono::steady_clock::now();
}

void Timer::calculateTimeSpentOnMove(Stone currentPlayer)
{
  auto now = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - moveStartTime);
  lastMoveTime = duration.count();

  if (currentPlayer == Stone::BLACK) {
    totalBlackTime += lastMoveTime;
  } else {
    totalWhiteTime += lastMoveTime;
  }
}