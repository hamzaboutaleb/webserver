#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <map>
#include <string>
#include <vector>

class Server;

class Location
{
private:
  std::string path;
  Server *server; // parent for fallback

  // Directives (empty/sentinel = not set, fallback to server)
  std::string root;
  std::string index;
  bool autoindex;
  bool autoindexSet;
  size_t maxClientBodySize;
  bool maxClientBodySizeSet;
  std::vector<std::string> methods;
  bool methodsSet;
  std::map<int, std::string> errorPages;
  int returnCode;
  std::string returnUrl;
  std::string uploadStore;
  std::map<std::string, std::string> cgiExtensions;

public:
  Location(const std::string &path);
  ~Location();

  void setServer(Server *server);

  // Setters
  void setRoot(const std::string &root);
  void setIndex(const std::string &index);
  void setAutoindex(bool autoindex);
  void setMaxClientBodySize(size_t size);
  void setMethods(const std::vector<std::string> &methods);
  void addErrorPage(int code, const std::string &uri);
  void setReturn(int code, const std::string &url);
  void setUploadStore(const std::string &path);
  void addCgiExtension(const std::string &ext, const std::string &binary);

  // Resolving getters (fall back to server when not set locally)
  const std::string &getPath() const;
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
  bool hasReturn() const;

  void print() const;
};

#endif