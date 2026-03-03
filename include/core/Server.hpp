#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

class Location;

class Server {
private:
  // Network
  std::vector<std::pair<std::string, int> > listenInterfaces;
  std::set<std::string> hostnames;

  // Directives
  std::string root;
  std::string index;
  bool autoindex;
  size_t maxClientBodySize;
  std::vector<std::string> methods;
  std::map<int, std::string> errorPages;
  int returnCode;
  std::string returnUrl;
  std::string uploadStore;
  std::map<std::string, std::string> cgiExtensions;

  // Locations
  std::vector<Location *> locations;

public:
  Server();
  ~Server();

  // Network setters
  void setInterface(const std::string &interface, int port);
  void addHostname(const std::string &hostname);

  // Directive setters
  void setRoot(const std::string &root);
  void setIndex(const std::string &index);
  void setAutoindex(bool autoindex);
  void setMaxClientBodySize(size_t maxClientBodySize);
  void setMethods(const std::vector<std::string> &methods);
  void addErrorPage(int code, const std::string &uri);
  void setReturn(int code, const std::string &url);
  void setUploadStore(const std::string &path);
  void addCgiExtension(const std::string &ext, const std::string &binary);

  // Location management
  void addLocation(Location *location);
  Location *matchPath(const std::string &path) const;

  // Getters
  const std::vector<std::pair<std::string, int> > &getListenInterfaces() const;
  const std::set<std::string> &getHostnames() const;
  const std::string &getRoot() const;
  const std::string &getIndex() const;
  bool getAutoindex() const;
  size_t getMaxClientBodySize() const;
  const std::vector<std::string> &getMethods() const;
  const std::map<int, std::string> &getErrorPages() const;
  int getReturnCode() const;
  const std::string &getReturnUrl() const;
  const std::string &getUploadStore() const;
  const std::map<std::string, std::string> &getCgiExtensions() const;
  const std::vector<Location *> &getLocations() const;
  bool hasReturn() const;

  void print() const;
};

#endif