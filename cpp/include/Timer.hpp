#ifndef TIMER_HPP
#define TIMER_HPP

class Timer {
public:
  Timer();
  ~Timer();

  void resetAll();
  void resetTimer();
  void calculateTimeSpentOnMove(Stone currentPlayer);

  float totalBlackTime() const { return _totalBlackTime; }
  float totalWhiteTime() const { return _totalWhiteTime; }
  float lastBlackMoveTime() const { return _lastBlackMoveTime; }
  float lastWhiteMoveTime() const { return _lastWhiteMoveTime; }

private:
  std::chrono::steady_clock::time_point _moveStartTime;
  float _lastMoveTime;
  float _lastBlackMoveTime;
  float _lastWhiteMoveTime;
  float _totalBlackTime;
  float _totalWhiteTime;
};

#endif
