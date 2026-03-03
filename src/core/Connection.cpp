#include "core/Connection.hpp"
#include "core/Location.hpp"
#include "core/RequestContext.hpp"
#include "core/ServerManager.hpp"
#include "core/Socket.hpp"
#include "utils/Number.hpp"
#include "utils/String.hpp"
#include <errno.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(int fd, int port, ConnectionType type,
                       ServerManager &serverManager)
    : fd(fd), port(port), type(type), timer(60), shouldCleanup(false),
      serverManager(serverManager), keepAlive(false), context(NULL) {
  buffer = new char[4096];
}

Connection::~Connection() {
  delete[] buffer;
  delete context;
}

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
    if (request.getState() == PARSE_PROCESS_HEADERS) {
      processHeaders();
      if (request.getState() != PARSE_ERROR) {
        request.setState(PARSE_BODY);
        request.parse();
      }
    }
  
    if (request.getState() == PARSE_ERROR) {
      shouldCleanup = true;
    }
  }
  
  void Connection::writeData() {}
  
  void Connection::processHeaders() {
    resolveConnectionHeaders();
    Server *server = serverManager.resolveServerForRequest(request, port);
  
    if (server == NULL) {
      request.setState(PARSE_ERROR);
      return;
    }
  
    this->server = server;
    // match the path
    Location *location = server->matchPath(request.getPath());
    if (location != NULL) {
      std::cout << "Matched location: " << location->getPath() << std::endl;
    } else {
      std::cout << "No matching location found for path: " << request.getPath()
                << std::endl;
    }
    delete context;
    context = new RequestContext(server, location, &request);
  }

void Connection::resolveConnectionHeaders() {
  std::string connectionHeader = request.getHeader("connection");
  if (String::toLower(connectionHeader) == "keep-alive") {
    keepAlive = true;
  }
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

bool Connection::getShouldCleanup() const { return shouldCleanup; }

ServerManager &Connection::getServerManager() { return serverManager; }

bool Connection::getKeepAlive() const { return keepAlive; }