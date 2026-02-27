#include "core/Socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int Socket::createListener(int port)
{
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    throw SocketCreationException();
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
  serverAddr.sin_addr.s_addr = INADDR_ANY;
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