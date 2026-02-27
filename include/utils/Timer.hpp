#ifndef TIMER_HPP
#define TIMER_HPP

#include <ctime>

class Timer
{
private:
  time_t lastActivity;
  int limit; // in seconds

public:
  Timer(int limitSeconds);
  ~Timer();

  void update();
  bool isExpired() const;
  void setLimit(int seconds);
  int getLimit() const;
};

#endif