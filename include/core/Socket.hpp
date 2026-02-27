#ifndef SOCKET_HPP
#define SOCKET_HPP
#include <exception>
#include <string>

class ServerManager;

class Socket
{
public:
  static int createListener(const std::string &interface, int port);
  static int acceptConnection(int fd);
  static void setNonBlocking(int fd);

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

  class SocketNonBlockingException : public std::exception
  {
    const char *what() const throw()
    {
      return "Failed to set socket to non-blocking";
    }
  };
};

#endif