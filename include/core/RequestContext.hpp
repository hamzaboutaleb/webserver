#ifndef REQUEST_CONTEXT_HPP
#define REQUEST_CONTEXT_HPP

#include <string>
#include <vector>
#include <map>

class Server;
class Location;
class HttpRequest;

class RequestContext
{
private:
  const Server *server;
  const Location *location;
  const HttpRequest *request;

public:
  RequestContext(const Server *server, const Location *location, const HttpRequest *request);
  ~RequestContext();

  // Resolving getters — delegate to location (with fallback) or server
  std::string getRoot() const;
  std::string getIndex() const;
  bool getAutoindex() const;
  size_t getMaxClientBodySize() const;
  std::vector<std::string> getMethods() const;
  std::map<int, std::string> getErrorPages() const;
  int getReturnCode() const;
  std::string getReturnUrl() const;
  bool hasReturn() const;
  std::string getUploadStore() const;
  std::map<std::string, std::string> getCgiExtensions() const;

  const Server *getServer() const;
  const Location *getLocation() const;
  const HttpRequest *getRequest() const;
};

#endif