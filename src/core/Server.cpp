#include "core/Server.hpp"

Server::Server()
{
}

Server::~Server()
{
  for (size_t i = 0; i < locations.size(); i++)
  {
    delete locations[i];
  }
}

void Server::setInterface(const std::string &interface, int port)
{
  listenInterfaces.push_back(std::make_pair(interface, port));
}

const std::vector<std::pair<std::string, int> > &Server::getListenInterfaces() const
{
  return listenInterfaces;
}

void Server::addHostname(const std::string &hostname)
{
  hostnames.insert(hostname);
}

const std::set<std::string> &Server::getHostnames() const
{
  return hostnames;
}

void Server::setRoot(const std::string &root)
{
  this->root = root;
}

const std::string &Server::getRoot() const
{
  return root;
}

void Server::addLocation(Location *location)
{
  locations.push_back(location);
}

const std::vector<Location *> &Server::getLocations() const
{
  return locations;
}