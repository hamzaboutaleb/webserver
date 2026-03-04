#include "core/ServerManager.hpp"
#include <iostream>
#include <map>
#include <set>
#include <sstream>

ServerManager::ServerManager() {}

ServerManager::~ServerManager() {
  for (size_t i = 0; i < servers.size(); i++) {
    delete servers[i];
  }
}

void ServerManager::initializeListener(const std::string &interface, int port) {
  try {
    int fd = Socket::createListener(interface, port);
    Connection *connection = Connection::createListener(fd, *this, port);
    eventloop.addConnection(connection);
    std::cout << "Listening on " << interface << ":" << port << std::endl;
  } catch (const std::exception &e) {
    std::ostringstream ss;
    ss << "Failed to listen on " << interface << ":" << port << ": "
       << e.what();
    throw ServerSetupException(ss.str());
  }
}

void ServerManager::setup(const std::vector<Server *> &servers) {
  this->servers = servers;
  std::map<int, std::set<std::string> > portToInterfaces;

  // 1. Collect all unique interfaces for each port across all servers
  for (size_t i = 0; i < servers.size(); i++) {
    const std::vector<std::pair<std::string, int> > &interfaces =
        servers[i]->getListenInterfaces();
    for (size_t j = 0; j < interfaces.size(); j++) {
      portToInterfaces[interfaces[j].second].insert(interfaces[j].first);
    }
  }

  // 2. Create sockets based on collected interfaces
  for (std::map<int, std::set<std::string> >::iterator it =
           portToInterfaces.begin();
       it != portToInterfaces.end(); ++it) {
    int port = it->first;
    std::set<std::string> &interfaces = it->second;

    if (interfaces.count("0.0.0.0")) {
      initializeListener("0.0.0.0", port);
    } else {
      for (std::set<std::string>::iterator sit = interfaces.begin();
           sit != interfaces.end(); ++sit) {
        initializeListener(*sit, port);
      }
    }
  }
}

void ServerManager::run() { eventloop.run(); }

void ServerManager::stop() { eventloop.stop(); }

Server *ServerManager::resolveServerForRequest(const HttpRequest &request,
                                               int port) {
  std::string host = request.getHeader("host");
  if (host.find(':') != std::string::npos) {
    host = host.substr(0, host.find(':'));
  }
  std::vector<Server *> candidates;
  for (size_t i = 0; i < servers.size(); i++) {
    const std::vector<std::pair<std::string, int> > &interfaces =
        servers[i]->getListenInterfaces();
    for (size_t j = 0; j < interfaces.size(); j++) {
      if (interfaces[j].second == port) {
        candidates.push_back(servers[i]);
        break;
      }
    }
  }
  // choise based on hostname
  for (size_t i = 0; i < candidates.size(); i++) {
    const std::set<std::string> &hostnames = candidates[i]->getHostnames();
    if (hostnames.count(host) > 0) {
      return candidates[i];
    }
  }
  if (candidates.size() == 0)
    return NULL;
  return candidates[0];
}