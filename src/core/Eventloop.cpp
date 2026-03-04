#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include "core/Socket.hpp"

#include "core/EventLoop.hpp"
#include "core/ConnectionType.hpp"
#include "core/Connection.hpp"
#include "utils/Constants.hpp"

EventLoop::EventLoop() : running(true)
{
  epollFd = epoll_create1(0);
  if (epollFd == -1)
  {
    throw EpollCreationException();
  }
  events = new epoll_event[Constants::Network::EpollMaxEvents];
}

void EventLoop::stop()
{
  running = false;
}

void EventLoop::addConnection(Connection *connection)
{
  if (connections.find(connection->getFd()) != connections.end())
  {
    throw EventLoop::ConnectionAlreadyExist();
  }
  struct epoll_event event;
  event.events = EPOLLIN;
  event.data.ptr = (void *)connection;
  if (epoll_ctl(epollFd, EPOLL_CTL_ADD, connection->getFd(), &event) == -1)
  {
    throw EventLoop::EpollAddConnectionException();
  }
  connections[connection->getFd()] = connection;
}

void EventLoop::removeConnection(Connection *connection)
{
  int fd = connection->getFd();
  epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
  connections.erase(fd);
  close(fd);
  delete connection;
}

void EventLoop::run()
{
  while (running)
  {
    int nfds = epoll_wait(epollFd, events, Constants::Network::EpollMaxEvents, Constants::Network::EpollWaitTimeout);
    if (nfds == -1)
    {
      if (errno == EINTR)
      {
        continue;
      }
      throw EventLoop::EpollWaitException();
    }
    for (int i = 0; nfds > 0 && i < nfds; i++)
    {
      Connection *connection = (Connection *)events[i].data.ptr;
      if (connection->getType() == LISTENER)
      {
        int clientFd = Socket::acceptConnection(connection->getFd());
        if (clientFd != -1)
        {
          Connection *clientConn = Connection::createClient(clientFd, connection->getServerManager(), connection->getPort());
          try
          {
            addConnection(clientConn);
            std::cout << "Accepted new connection on fd " << clientFd << std::endl;
          }
          catch (...)
          {
            delete clientConn;
            close(clientFd);
          }
        }
      }
      else if (connection->getType() == CLIENT)
      {
        connection->updateActivity();
        try
        {
          if (events[i].events & EPOLLIN)
          {
            connection->readData();
            if (connection->getShouldCleanup())
            {
              removeConnection(connection);
              continue;
            }
            if (connection->getRequest().getState() == PARSE_SUCCESS)
            {
              struct epoll_event ev;
              ev.events = EPOLLOUT;
              ev.data.ptr = (void *)connection;
              epoll_ctl(epollFd, EPOLL_CTL_MOD, connection->getFd(), &ev);
            }
          }
          else if (events[i].events & EPOLLOUT)
          {
            connection->writeData();
            if (connection->getShouldCleanup())
            {
              removeConnection(connection);
              continue;
            }
            if (connection->getResponse().getState() == RESPONSE_IDLE)
            {
              struct epoll_event ev;
              ev.events = EPOLLIN;
              ev.data.ptr = (void *)connection;
              epoll_ctl(epollFd, EPOLL_CTL_MOD, connection->getFd(), &ev);
            }
          }
        }
        catch (const Connection::ConnectionClosedException &e)
        {
          std::cout << "Connection closed by peer on fd " << connection->getFd() << std::endl;
          removeConnection(connection);
        }
        catch (const std::exception &e)
        {
          std::cerr << "Error handling connection on fd " << connection->getFd() << ": " << e.what() << std::endl;
          removeConnection(connection);
        }
      }
    }

    // Check for timeouts
    for (std::map<int, Connection *>::iterator it = connections.begin(); it != connections.end();)
    {
      Connection *conn = it->second;
      if (conn->isTimedOut())
      {
        std::cout << "Closing timed out connection on fd " << conn->getFd() << std::endl;
        int fd = conn->getFd();
        epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
        delete conn;
        connections.erase(it++);
      }
      else
      {
        ++it;
      }
    }
  }
}

EventLoop::~EventLoop()
{
  close(epollFd);
  delete[] events;
  for (std::map<int, Connection *>::iterator it = connections.begin(); it != connections.end(); ++it)
  {
    close(it->first);
    delete it->second;
  }
}