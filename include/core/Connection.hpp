#ifndef CONNECTION_CPP
#define CONNECTION_CPP

#include "core/ConnectionType.hpp"

class Connection
{
  int fd;
  ConnectionType type;

public:
  Connection(int fd, ConnectionType type);
  ~Connection();

  int getFd() const;
  ConnectionType getType() const;
};

#endif