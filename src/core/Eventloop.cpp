#include <sys/epoll.h>
#include <errno.h>

#include "core/EventLoop.hpp"
#include "core/ConnectionType.hpp"
#include "core/Connection.hpp"

EventLoop::EventLoop()
{
  epollFd = epoll_create1(0);
  if (epollFd == -1)
  {
    throw EpollCreationException();
  }
}
void EventLoop::addConnection(Connection *connection)
{
  struct epoll_event event;
  event.events = EPOLLIN;
  event.data.ptr = (void *)connection;
  if (epoll_ctl(epollFd, EPOLL_CTL_ADD, connection->getFd(), &event) == -1)
  {
    delete connection;
    throw EventLoop::EpollAddConnectionException();
  }
  if (connections.find(connection->getFd()) != connections.end())
  {
    delete connection;
    throw EventLoop::ConnectionAlreadyExist();
  }
  connections[connection->getFd()] = connection;
}

void EventLoop::run()
{
  epoll_event *events = new epoll_event[1024];
  while (true)
  {
    int nfds = epoll_wait(epollFd, events, 1024, 100);
    if (nfds == -1)
    {
      if (errno == EINTR)
      {
        continue; // Interrupted by signal, retry
      }
      delete[] events;
      throw EventLoop::EpollWaitException();
    }
    for (int i = 0; i < nfds; i++)
    {
      Connection *connection = (Connection *)events[i].data.ptr;
      if (connection->getType() == LISTENER)
      {
        // Handle new incoming connection
      }
      else if (connection->getType() == CLIENT)
      {
        // Handle client request
      }
    }
  }
}

// EventLoop::~EventLoop()
// {
//   close(epollFd);
// }