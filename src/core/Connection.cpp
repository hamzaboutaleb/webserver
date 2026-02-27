#include "core/Connection.hpp"
#include "core/ServerManager.hpp"
#include "core/Socket.hpp"
#include <errno.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(int fd, int port, ConnectionType type,
                       ServerManager &serverManager)
    : fd(fd), port(port), type(type), timer(60), shouldCleanup(false),
      serverManager(serverManager) {
  buffer = new char[4096];
}

Connection::~Connection() { delete[] buffer; }

void Connection::readData() {
  while (true) {
    ssize_t bytesRead = recv(fd, buffer, 4096, 0);
    std::cout << "Read " << bytesRead << " bytes from fd " << fd << std::endl;
    if (bytesRead > 0) {
      request.appendData(buffer, bytesRead);
    } else if (bytesRead == 0) {
      throw ConnectionClosedException();
    } else {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      throw ReadDataException();
    }
  }

  request.parse();
  if (request.getState() == PARSE_RESOLVE_SERVER) {
    Server *server = serverManager.resolveServerForRequest(request, port);
    if (server == NULL) {
      shouldCleanup = true;
      return;
    }
    this->server = server;
    server->print();
    request.setState(PARSE_BODY);
    request.parse();
  }

  if (request.getState() == PARSE_ERROR) {
    shouldCleanup = true;
  }
}

void Connection::writeData() {
  // Implement writing data from the buffer to the socket
}

int Connection::getFd() const { return fd; }

int Connection::getPort() const { return port; }

ConnectionType Connection::getType() const { return type; }

void Connection::updateActivity() { timer.update(); }

bool Connection::isTimedOut() const {
  if (type == LISTENER)
    return false;
  return timer.isExpired();
}

void Connection::setTimeout(int seconds) { timer.setLimit(seconds); }

HttpRequest &Connection::getRequest() { return request; }

Connection *Connection::createListener(int fd, ServerManager &serverManager,
                                       int port) {
  return new Connection(fd, port, LISTENER, serverManager);
}

Connection *Connection::createClient(int fd, ServerManager &serverManager,
                                     int port) {
  return new Connection(fd, port, CLIENT, serverManager);
}

ServerManager &Connection::getServerManager() { return serverManager; }