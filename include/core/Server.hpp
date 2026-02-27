#ifndef SERVER_HPP
#define SERVER_HPP

#include <set>
#include <vector>
#include <string>
#include "core/Location.hpp"

class Server
{
private:
  std::vector<std::pair<std::string, int> > listenInterfaces; // pair of (interface, port)
  std::set<std::string> hostnames;
  std::string root;
  std::vector<Location *> locations;

public:
  Server();
  ~Server();
  // setters
  void setInterface(const std::string &interface, int port);
  void addHostname(const std::string &hostname);
  void setRoot(const std::string &root);
  void addLocation(Location *location);
  // getters
  const std::vector<std::pair<std::string, int> > &getListenInterfaces() const;
  const std::set<std::string> &getHostnames() const;
  const std::string &getRoot() const;
  const std::vector<Location *> &getLocations() const;
  void print() const;
};

#endif