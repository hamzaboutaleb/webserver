#include "core/Server.hpp"
#include <iostream>

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

void Server::print() const
{
  std::cout << "Server:" << std::endl;
  std::cout << "  Listen interfaces:" << std::endl;
  for (size_t i = 0; i < listenInterfaces.size(); i++)
  {
    std::cout << "    - " << (listenInterfaces[i].first.empty() ? "*" : listenInterfaces[i].first) << ":" << listenInterfaces[i].second << std::endl;
  }

  std::cout << "  Hostnames:" << std::endl;
  for (std::set<std::string>::const_iterator it = hostnames.begin(); it != hostnames.end(); ++it)
  {
    std::cout << "    - " << (it->empty() ? "(default)" : *it) << std::endl;
  }

  std::cout << "  Root: " << root << std::endl;

  if (!locations.empty())
  {
    std::cout << "  Locations:" << std::endl;
    for (size_t i = 0; i < locations.size(); i++)
    {
      std::cout << "    - Path: " << locations[i]->getPath() << std::endl;
      if (!locations[i]->getRoot().empty())
        std::cout << "      Root: " << locations[i]->getRoot() << std::endl;
      
      const std::map<std::string, std::vector<std::string> > &directives = locations[i]->getDirectives();
      for (std::map<std::string, std::vector<std::string> >::const_iterator it = directives.begin(); it != directives.end(); ++it)
      {
        std::cout << "      " << it->first << ":";
        for (size_t k = 0; k < it->second.size(); k++)
          std::cout << " " << it->second[k];
        std::cout << std::endl;
      }
    }
  }
}