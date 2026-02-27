#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include "core/Socket.hpp"

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
  events = new epoll_event[1024];
}
void EventLoop::addConnection(Connection *connection)
{
  struct epoll_event event;
  event.events = EPOLLIN;
  event.data.ptr = (void *)connection;
  if (epoll_ctl(epollFd, EPOLL_CTL_ADD, connection->getFd(), &event) == -1)
  {
    // Do not delete connection here, let the caller handle it or manage it consistently
    throw EventLoop::EpollAddConnectionException();
  }
  if (connections.find(connection->getFd()) != connections.end())
  {
    throw EventLoop::ConnectionAlreadyExist();
  }
  connections[connection->getFd()] = connection;
}

void EventLoop::run()
{
  while (true)
  {
    int nfds = epoll_wait(epollFd, events, 1024, 1000);
    if (nfds == -1)
    {
      if (errno == EINTR)
      {
        continue; // Interrupted by signal, retry
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
            if (connection->getRequest().getState() == PARSE_SUCCESS)
            {
              connection->getRequest().print();
              // For now, send a response and close
              write(connection->getFd(), "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello world!\n", 52);
            }
          }
          else if (events[i].events & EPOLLOUT)
          {
            connection->writeData();
          }
        }
        catch (const Connection::ConnectionClosedException &e)
        {
          std::cout << "Connection closed by peer on fd " << connection->getFd() << std::endl;
          epoll_ctl(epollFd, EPOLL_CTL_DEL, connection->getFd(), NULL);
          connections.erase(connection->getFd());
          close(connection->getFd());
          delete connection;
        }
        catch (const std::exception &e)
        {
          std::cerr << "Error handling connection on fd " << connection->getFd() << ": " << e.what() << std::endl;
          epoll_ctl(epollFd, EPOLL_CTL_DEL, connection->getFd(), NULL);
          connections.erase(connection->getFd());
          close(connection->getFd());
          delete connection;
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
        epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->getFd(), NULL);
        close(conn->getFd());
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