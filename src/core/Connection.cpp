#include "core/Connection.hpp"
#include "core/Location.hpp"
#include "core/RequestContext.hpp"
#include "core/ServerManager.hpp"
#include "core/Socket.hpp"
#include "utils/File.hpp"
#include "utils/Number.hpp"
#include "utils/String.hpp"
#include "utils/Constants.hpp"
#include <errno.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(int fd, int port, ConnectionType type,
                       ServerManager &serverManager)
    : fd(fd), port(port), type(type), timer(Constants::Timeout::ConnectionIdle), shouldCleanup(false),
      serverManager(serverManager), keepAlive(false), context(NULL) {
  buffer = new char[Constants::Buffer::ReadBufferSize];
}

Connection::~Connection() {
  delete[] buffer;
  delete context;
}

void Connection::readData() {
  while (true) {
    ssize_t bytesRead = recv(fd, buffer, Constants::Buffer::ReadBufferSize, 0);
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

    if (request.getState() == PARSE_SUCCESS) {
      prepareResponse();
    }
  }
  
  void Connection::writeData() {
    if (response.getState() == RESPONSE_SENDING_HEADERS) {
      const std::string &headers = response.getHeadersBuffer();
      size_t sent = response.getHeadersSent();
      ssize_t bytes = send(fd, headers.c_str() + sent, headers.size() - sent, 0);
      if (bytes > 0) {
        response.updateHeadersSent(bytes);
      } else if (bytes < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        shouldCleanup = true;
      }
    }

    if (response.getState() == RESPONSE_SENDING_BODY) {
      if (response.getFileFd() != -1) {
        // Stream from file
        char chunk[Constants::Buffer::WriteChunkSize];
        ssize_t bytesRead = read(response.getFileFd(), chunk, sizeof(chunk));
        
        if (bytesRead > 0) {
          ssize_t bytesSent = send(fd, chunk, bytesRead, 0);
          if (bytesSent > 0) {
            response.updateBodySent(bytesSent);
            // If we didn't send everything we read, we need to seek back
            if (bytesSent < bytesRead) {
              lseek(response.getFileFd(), bytesSent - bytesRead, SEEK_CUR);
            }
          } else if (bytesSent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            shouldCleanup = true;
          }
        } else {
          response.updateBodySent(0); // Mark as finished if nothing to read
        }
      } else {
        // Stream from string
        const std::string &body = response.getStringBody();
        size_t sent = response.getBodySent();
        ssize_t bytes = send(fd, body.c_str() + sent, body.size() - sent, 0);
        if (bytes > 0) {
          response.updateBodySent(bytes);
        } else if (bytes < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
          shouldCleanup = true;
        }
      }
    }

    if (response.getState() == RESPONSE_FINISHED) {
      if (!keepAlive) {
        shouldCleanup = true;
      } else {
        // Reset for next request if keep-alive
        request.clear();
        response.clear();
        // We need to tell the event loop to switch back to EPOLLIN
        // This is usually done by the event loop itself or by returning a status
        // In this architecture, we might need a way to trigger EPOLL_CTL_MOD
      }
    }
  }

  void Connection::prepareResponse() {
    if (!context) {
      response.prepareFromError(Constants::HttpStatus::InternalServerError, "Request Context Missing");
      return;
    }

    if (context->hasReturn()) {
      response.prepareRedirect(context->getReturnCode(), context->getReturnUrl());
      return;
    }

    std::string root = context->getRoot();
    std::string path = request.getPath();
    std::string fullPath = root + path;

    if (File::isDirectory(fullPath)) {
      if (!fullPath.empty() && fullPath[fullPath.size() - 1] != '/') {
        // Handle directory redirect for missing trailing slash
        response.prepareRedirect(Constants::HttpStatus::MovedPermanently, path + "/");
        return;
      }
      fullPath += context->getIndex();
    }

    if (!File::exists(fullPath)) {
      response.prepareFromError(Constants::HttpStatus::NotFound);
    } else if (!File::isFile(fullPath)) {
      response.prepareFromError(Constants::HttpStatus::Forbidden, "Not a regular file");
    } else {
      response.prepareFromFile(fullPath, Constants::HttpStatus::OK);
    }
  }
  
  void Connection::processHeaders() {
    resolveConnectionHeaders();
    Server *server = serverManager.resolveServerForRequest(request, port);
  
    if (server == NULL) {
      request.setErrorCode(Constants::HttpStatus::BadRequest);
      response.prepareFromError(Constants::HttpStatus::BadRequest);
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

    // Enforce max body size
    std::string clHeader = request.getHeader("content-length");
    if (!clHeader.empty()) {
      size_t contentLength = Number::toInt(clHeader);
      if (contentLength > context->getMaxClientBodySize()) {
        request.setErrorCode(Constants::HttpStatus::PayloadTooLarge);
        response.prepareFromError(Constants::HttpStatus::PayloadTooLarge);
        return;
      }
    }
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

HttpResponse &Connection::getResponse() { return response; }

bool Connection::getShouldCleanup() const { return shouldCleanup; }

ServerManager &Connection::getServerManager() { return serverManager; }

bool Connection::getKeepAlive() const { return keepAlive; }