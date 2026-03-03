#include "core/Location.hpp"
#include "core/Server.hpp"
#include <iostream>

Location::Location(const std::string &path)
    : path(path), server(NULL), autoindex(false), autoindexSet(false),
      maxClientBodySize(0), maxClientBodySizeSet(false),
      methodsSet(false), returnCode(-1)
{
}

Location::~Location()
{
}

void Location::setServer(Server *server) { this->server = server; }

// Setters
void Location::setRoot(const std::string &root) { this->root = root; }
void Location::setIndex(const std::string &index) { this->index = index; }

void Location::setAutoindex(bool autoindex)
{
  this->autoindex = autoindex;
  this->autoindexSet = true;
}

void Location::setMaxClientBodySize(size_t size)
{
  this->maxClientBodySize = size;
  this->maxClientBodySizeSet = true;
}

void Location::setMethods(const std::vector<std::string> &methods)
{
  this->methods = methods;
  this->methodsSet = true;
}

void Location::addErrorPage(int code, const std::string &uri)
{
  errorPages[code] = uri;
}

void Location::setReturn(int code, const std::string &url)
{
  returnCode = code;
  returnUrl = url;
}

void Location::setUploadStore(const std::string &path)
{
  uploadStore = path;
}

void Location::addCgiExtension(const std::string &ext, const std::string &binary)
{
  cgiExtensions[ext] = binary;
}

// Resolving getters — fall back to server when not set locally

const std::string &Location::getPath() const { return path; }

const std::string &Location::getRoot() const
{
  if (!root.empty())
    return root;
  if (server)
    return server->getRoot();
  return root; // returns empty string
}

const std::string &Location::getIndex() const
{
  if (!index.empty())
    return index;
  if (server)
    return server->getIndex();
  return index;
}

bool Location::getAutoindex() const
{
  if (autoindexSet)
    return autoindex;
  if (server)
    return server->getAutoindex();
  return false;
}

size_t Location::getMaxClientBodySize() const
{
  if (maxClientBodySizeSet)
    return maxClientBodySize;
  if (server)
    return server->getMaxClientBodySize();
  return 1048576; // 1MB default
}

const std::vector<std::string> &Location::getMethods() const
{
  if (methodsSet)
    return methods;
  if (server)
    return server->getMethods();
  return methods; // returns empty vector
}

const std::map<int, std::string> &Location::getErrorPages() const
{
  if (!errorPages.empty())
    return errorPages;
  if (server)
    return server->getErrorPages();
  return errorPages;
}

int Location::getReturnCode() const
{
  if (returnCode != -1)
    return returnCode;
  if (server)
    return server->getReturnCode();
  return -1;
}

const std::string &Location::getReturnUrl() const
{
  if (returnCode != -1)
    return returnUrl;
  if (server)
    return server->getReturnUrl();
  return returnUrl;
}

const std::string &Location::getUploadStore() const
{
  if (!uploadStore.empty())
    return uploadStore;
  if (server)
    return server->getUploadStore();
  return uploadStore;
}

const std::map<std::string, std::string> &Location::getCgiExtensions() const
{
  if (!cgiExtensions.empty())
    return cgiExtensions;
  if (server)
    return server->getCgiExtensions();
  return cgiExtensions;
}

bool Location::hasReturn() const { return getReturnCode() != -1; }

void Location::print() const
{
  std::cout << "    - Path: " << path << std::endl;
  if (!root.empty())
    std::cout << "      Root: " << root << std::endl;
  if (!index.empty())
    std::cout << "      Index: " << index << std::endl;
  if (autoindexSet)
    std::cout << "      Autoindex: " << (autoindex ? "on" : "off") << std::endl;
  if (maxClientBodySizeSet)
    std::cout << "      Max client body size: " << maxClientBodySize << std::endl;
  if (methodsSet)
  {
    std::cout << "      Methods:";
    for (size_t i = 0; i < methods.size(); i++)
      std::cout << " " << methods[i];
    std::cout << std::endl;
  }
  if (!errorPages.empty())
  {
    std::cout << "      Error pages:" << std::endl;
    for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it)
      std::cout << "        " << it->first << " -> " << it->second << std::endl;
  }
  if (returnCode != -1)
    std::cout << "      Return: " << returnCode << " " << returnUrl << std::endl;
  if (!uploadStore.empty())
    std::cout << "      Upload store: " << uploadStore << std::endl;
  if (!cgiExtensions.empty())
  {
    std::cout << "      CGI extensions:" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = cgiExtensions.begin(); it != cgiExtensions.end(); ++it)
      std::cout << "        " << it->first << " -> " << it->second << std::endl;
  }
}