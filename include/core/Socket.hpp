#ifndef SOCKET_HPP
#define SOCKET_HPP
#include <exception>

class Socket
{
  static int createListener(int port);

  class SocketCreationException : public std::exception
  {
    const char *what() const throw()
    {
      return "Failed to create socket";
    }
  };

  class SocketOptionException : public std::exception
  {
    const char *what() const throw()
    {
      return "Failed to set socket options";
    }
  };

  class SocketBindException : public std::exception
  {
    const char *what() const throw()
    {
      return "Failed to bind socket";
    }
  };

  class SocketListenException : public std::exception
  {
    const char *what() const throw()
    {
      return "Failed to listen on socket";
    }
  };

};

#endif