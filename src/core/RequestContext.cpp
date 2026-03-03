#include "core/RequestContext.hpp"
#include "core/Server.hpp"
#include "core/Location.hpp"
#include "core/HttpRequest.hpp"

RequestContext::RequestContext(const Server *server, const Location *location, const HttpRequest *request)
    : server(server), location(location), request(request)
{
}

RequestContext::~RequestContext() {}

std::string RequestContext::getRoot() const
{
  if (location)
    return location->getRoot();
  if (server)
    return server->getRoot();
  return "";
}

std::string RequestContext::getIndex() const
{
  if (location)
    return location->getIndex();
  if (server)
    return server->getIndex();
  return "index.html";
}

bool RequestContext::getAutoindex() const
{
  if (location)
    return location->getAutoindex();
  if (server)
    return server->getAutoindex();
  return false;
}

size_t RequestContext::getMaxClientBodySize() const
{
  if (location)
    return location->getMaxClientBodySize();
  if (server)
    return server->getMaxClientBodySize();
  return 1048576;
}

std::vector<std::string> RequestContext::getMethods() const
{
  if (location)
    return location->getMethods();
  if (server)
    return server->getMethods();
  std::vector<std::string> defaults;
  defaults.push_back("GET");
  return defaults;
}

std::map<int, std::string> RequestContext::getErrorPages() const
{
  if (location)
    return location->getErrorPages();
  if (server)
    return server->getErrorPages();
  return std::map<int, std::string>();
}

int RequestContext::getReturnCode() const
{
  if (location)
    return location->getReturnCode();
  if (server)
    return server->getReturnCode();
  return -1;
}

std::string RequestContext::getReturnUrl() const
{
  if (location)
    return location->getReturnUrl();
  if (server)
    return server->getReturnUrl();
  return "";
}

bool RequestContext::hasReturn() const
{
  return getReturnCode() != -1;
}

std::string RequestContext::getUploadStore() const
{
  if (location)
    return location->getUploadStore();
  if (server)
    return server->getUploadStore();
  return "";
}

std::map<std::string, std::string> RequestContext::getCgiExtensions() const
{
  if (location)
    return location->getCgiExtensions();
  if (server)
    return server->getCgiExtensions();
  return std::map<std::string, std::string>();
}

const Server *RequestContext::getServer() const { return server; }
const Location *RequestContext::getLocation() const { return location; }
const HttpRequest *RequestContext::getRequest() const { return request; }
