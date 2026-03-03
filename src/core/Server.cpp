#include "core/Server.hpp"
#include "core/Location.hpp"
#include <iostream>

Server::Server()
    : autoindex(false), maxClientBodySize(1048576), returnCode(-1)
{
  index = "index.html";
  methods.push_back("GET");
}

Server::~Server()
{
  for (size_t i = 0; i < locations.size(); i++)
  {
    delete locations[i];
  }
}

// Network setters
void Server::setInterface(const std::string &interface, int port)
{
  listenInterfaces.push_back(std::make_pair(interface, port));
}

void Server::addHostname(const std::string &hostname)
{
  hostnames.insert(hostname);
}

// Directive setters
void Server::setRoot(const std::string &root) { this->root = root; }
void Server::setIndex(const std::string &index) { this->index = index; }
void Server::setAutoindex(bool autoindex) { this->autoindex = autoindex; }
void Server::setMaxClientBodySize(size_t size) { this->maxClientBodySize = size; }
void Server::setMethods(const std::vector<std::string> &methods) { this->methods = methods; }

void Server::addErrorPage(int code, const std::string &uri)
{
  errorPages[code] = uri;
}

void Server::setReturn(int code, const std::string &url)
{
  returnCode = code;
  returnUrl = url;
}

void Server::setUploadStore(const std::string &path) { uploadStore = path; }

void Server::addCgiExtension(const std::string &ext, const std::string &binary)
{
  cgiExtensions[ext] = binary;
}

// Location management
void Server::addLocation(Location *location)
{
  locations.push_back(location);
}

Location *Server::matchPath(const std::string &path) const
{
  Location *bestMatch = NULL;
  size_t bestMatchLength = 0;

  for (size_t i = 0; i < locations.size(); i++)
  {
    const std::string &locPath = locations[i]->getPath();
    if (path.compare(0, locPath.length(), locPath) == 0)
    {
      if (locPath.length() > bestMatchLength)
      {
        bestMatch = locations[i];
        bestMatchLength = locPath.length();
      }
    }
  }

  return bestMatch;
}

// Getters
const std::vector<std::pair<std::string, int> > &Server::getListenInterfaces() const { return listenInterfaces; }
const std::set<std::string> &Server::getHostnames() const { return hostnames; }
const std::string &Server::getRoot() const { return root; }
const std::string &Server::getIndex() const { return index; }
bool Server::getAutoindex() const { return autoindex; }
size_t Server::getMaxClientBodySize() const { return maxClientBodySize; }
const std::vector<std::string> &Server::getMethods() const { return methods; }
const std::map<int, std::string> &Server::getErrorPages() const { return errorPages; }
int Server::getReturnCode() const { return returnCode; }
const std::string &Server::getReturnUrl() const { return returnUrl; }
const std::string &Server::getUploadStore() const { return uploadStore; }
const std::map<std::string, std::string> &Server::getCgiExtensions() const { return cgiExtensions; }
const std::vector<Location *> &Server::getLocations() const { return locations; }
bool Server::hasReturn() const { return returnCode != -1; }

// Print
void Server::print() const
{
  std::cout << "Server:" << std::endl;
  std::cout << "  Listen interfaces:" << std::endl;
  for (size_t i = 0; i < listenInterfaces.size(); i++)
  {
    std::cout << "    - " << (listenInterfaces[i].first.empty() ? "*" : listenInterfaces[i].first)
              << ":" << listenInterfaces[i].second << std::endl;
  }

  std::cout << "  Hostnames:" << std::endl;
  for (std::set<std::string>::const_iterator it = hostnames.begin(); it != hostnames.end(); ++it)
  {
    std::cout << "    - " << (it->empty() ? "(default)" : *it) << std::endl;
  }

  std::cout << "  Root: " << root << std::endl;
  std::cout << "  Index: " << index << std::endl;
  std::cout << "  Autoindex: " << (autoindex ? "on" : "off") << std::endl;
  std::cout << "  Max client body size: " << maxClientBodySize << std::endl;

  std::cout << "  Methods:";
  for (size_t i = 0; i < methods.size(); i++)
    std::cout << " " << methods[i];
  std::cout << std::endl;

  if (!errorPages.empty())
  {
    std::cout << "  Error pages:" << std::endl;
    for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it)
      std::cout << "    " << it->first << " -> " << it->second << std::endl;
  }

  if (hasReturn())
    std::cout << "  Return: " << returnCode << " " << returnUrl << std::endl;

  if (!uploadStore.empty())
    std::cout << "  Upload store: " << uploadStore << std::endl;

  if (!cgiExtensions.empty())
  {
    std::cout << "  CGI extensions:" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = cgiExtensions.begin(); it != cgiExtensions.end(); ++it)
      std::cout << "    " << it->first << " -> " << it->second << std::endl;
  }

  if (!locations.empty())
  {
    std::cout << "  Locations:" << std::endl;
    for (size_t i = 0; i < locations.size(); i++)
    {
      locations[i]->print();
    }
  }
}