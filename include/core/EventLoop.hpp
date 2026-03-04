#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include <exception>
#include <map>
#include <sys/epoll.h>
#include "core/Connection.hpp"

class EventLoop
{
  int epollFd;
  epoll_event *events;
  std::map<int, Connection *> connections;
  bool running;

public:
  EventLoop();
  ~EventLoop();

  void addConnection(Connection *connection);
  void removeConnection(Connection *connection);
  void run();
  void stop();

  class EpollCreationException : public std::exception
  {
  };

  class EpollAddConnectionException : public std::exception
  {
  };

  class ConnectionAlreadyExist : public std::exception
  {
  };

  class EpollWaitException : public std::exception
  {
  };
};

#endif