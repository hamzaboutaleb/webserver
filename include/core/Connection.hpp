#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "core/ConnectionType.hpp"
#include "core/HttpRequest.hpp"
#include "core/Server.hpp"
#include "utils/Timer.hpp"

class ServerManager;

class Connection {
  int fd;
  int port;
  ConnectionType type;
  Timer timer;
  HttpRequest request;
  Server *server;
  bool shouldCleanup;
  ServerManager &serverManager;

  char *buffer;

public:
  Connection(int fd, int port, ConnectionType type,
             ServerManager &serverManager);
  ~Connection();

  int getFd() const;
  int getPort() const;
  ConnectionType getType() const;

  void updateActivity();
  bool isTimedOut() const;
  void setTimeout(int seconds);
  void readData();
  void writeData();
  ServerManager &getServerManager();
  HttpRequest &getRequest();

  static Connection *createListener(int fd, ServerManager &serverManager,
                                    int port);
  static Connection *createClient(int fd, ServerManager &serverManager,
                                  int port);

  class ConnectionClosedException : public std::exception {
    const char *what() const throw() { return "Connection closed by peer"; }
  };

  class ReadDataException : public std::exception {
    const char *what() const throw() {
      return "Error reading data from connection";
    }
  };
};

#endif