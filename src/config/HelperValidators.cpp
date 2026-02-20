#include "config/ConfigValidator.hpp"
#include "utils/File.hpp"
#include <sstream>

bool ConfigValidator::isValidPort(const std::string &portStr)
{
  if (portStr.empty() || portStr.size() > 5)
    return false;
  if (portStr.size() > 5)
    return false;
  for (size_t i = 0; i < portStr.size(); i++)
  {
    if (!isdigit(portStr[i]))
      return false;
  }
  int port;
  std::istringstream iss(portStr);
  iss >> port;
  return port > 0 && port <= 65535;
}

bool ConfigValidator::isValidRootPath(const std::string &path)
{
  if (path.empty())
    return false;
  if (path[0] != '/')
    return false;
  return true;
}

bool ConfigValidator::isValidServerName(const std::string &name)
{
  if (name.empty())
    return false;
  if (name[0] == '-' || name[0] == '.' || name[name.size() - 1] == '-' || name[name.size() - 1] == '.')
    return false;
  for (size_t i = 0; i < name.size(); i++)
  {
    char c = name[i];
    if (isspace(c) || (!isalnum(c) && c != '-' && c != '.'))
      return false;
  }
  return true;
}