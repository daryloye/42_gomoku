#ifndef TIMER_HPP
#define TIMER_HPP

class Timer {
public:
  Timer();
  ~Timer();

  void resetAll();
  void resetTimer();
  void calculateTimeSpentOnMove(Stone currentPlayer);

  std::chrono::steady_clock::time_point moveStartTime;
  float lastMoveTime;
  float lastBlackMoveTime;
  float lastWhiteMoveTime;
  float totalBlackTime;
  float totalWhiteTime;
};

#endif