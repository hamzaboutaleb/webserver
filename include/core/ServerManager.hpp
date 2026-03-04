#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include <map>
#include <string>
#include <vector>
#include <stdexcept>

#include "core/EventLoop.hpp"
#include "core/Server.hpp"
#include "core/Connection.hpp"
#include "core/Socket.hpp"
#include "core/HttpRequest.hpp"

class ServerManager
{
private:
  EventLoop eventloop;
  std::vector<Server *> servers;

  void initializeListener(const std::string &interface, int port);

public:
  ServerManager();
  ~ServerManager();

  Server *resolveServerForRequest(const HttpRequest &request, int port);
  void setup(const std::vector<Server *> &servers);
  void run();
  void stop();

  class ServerSetupException : public std::runtime_error
  {
  public:
    ServerSetupException(const std::string &msg) : std::runtime_error(msg) {}
  };
};

#endif