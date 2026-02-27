#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <map>
#include <vector>

class Location
{
private:
  std::string path;
  std::string root;
  std::map<std::string, std::vector<std::string> > directives;

public:
  Location(const std::string &path);
  ~Location();

  void setRoot(const std::string &root);
  void addDirective(const std::string &key, const std::vector<std::string> &values);

  const std::string &getPath() const;
  const std::string &getRoot() const;
  const std::map<std::string, std::vector<std::string> > &getDirectives() const;
};

#endif