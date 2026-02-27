#include "core/Location.hpp"

Location::Location(const std::string &path) : path(path)
{
}

Location::~Location()
{
}

void Location::setRoot(const std::string &root)
{
  this->root = root;
}

void Location::addDirective(const std::string &key, const std::vector<std::string> &values)
{
  directives[key] = values;
}

const std::string &Location::getPath() const
{
  return path;
}

const std::string &Location::getRoot() const
{
  return root;
}

const std::map<std::string, std::vector<std::string> > &Location::getDirectives() const
{
  return directives;
}