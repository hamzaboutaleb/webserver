#include "utils/Timer.hpp"

Timer::Timer(int limitSeconds) : limit(limitSeconds)
{
  update();
}

Timer::~Timer() {}

void Timer::update()
{
  lastActivity = time(NULL);
}

bool Timer::isExpired() const
{
  return (time(NULL) - lastActivity) >= limit;
}

void Timer::setLimit(int seconds)
{
  limit = seconds;
}

int Timer::getLimit() const
{
  return limit;
}