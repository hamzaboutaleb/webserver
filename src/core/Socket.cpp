#include "core/Socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <string>

void Socket::setNonBlocking(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1)
  {
    throw SocketNonBlockingException();
  }
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
  {
    throw SocketNonBlockingException();
  }
}

int Socket::createListener(const std::string &interface, int port)
{
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    throw SocketCreationException();
  }

  try {
    setNonBlocking(sockfd);
  } catch (...) {
    close(sockfd);
    throw;
  }

  int opt = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
  {
    close(sockfd);
    throw SocketOptionException();
  }

  struct sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr(interface.c_str());
  serverAddr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
  {
    close(sockfd);
    throw SocketBindException();
  }

  if (listen(sockfd, SOMAXCONN) == -1)
  {
    close(sockfd);
    throw SocketListenException();
  }

  return sockfd;
}

int Socket::acceptConnection(int fd)
{
  struct sockaddr_in clientAddr;
  socklen_t clientLen = sizeof(clientAddr);
  int newsockfd = accept(fd, (struct sockaddr *)&clientAddr, &clientLen);
  
  if (newsockfd != -1)
  {
    try {
      setNonBlocking(newsockfd);
    } catch (...) {
      close(newsockfd);
      return -1;
    }
  }
  return newsockfd;
}