
#include "core/Connection.hpp"
Connection::~Connection() {}
Connection::Connection(int fd, ConnectionType type) : fd(fd), type(type) {}

int Connection::getFd() const
{
  return fd;
}

ConnectionType Connection::getType() const
{
  return type;
}

Connection *Connection::createListener(int fd)
{
  return new Connection(fd, LISTENER);
}

Connection *Connection::createClient(int fd)
{
  return new Connection(fd, CLIENT);
}
