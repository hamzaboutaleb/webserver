#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include <map>
#include <string>

#include "core/EventLoop.hpp"
#include "core/Server.hpp"

class ServerManager
{
private:
  EventLoop eventloop;
  std::map<int, std::map<std::string, Server *>> servers;

public:
};

#endif