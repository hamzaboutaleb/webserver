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

bool ConfigValidator::isValidSizeValue(const std::string &value)
{
  if (value.empty())
    return false;
  size_t lastCharIndex = value.size() - 1;
  char lastChar = std::tolower(value[lastCharIndex]);
  bool hasUnit = (lastChar == 'k' || lastChar == 'm' || lastChar == 'g');
  std::string numberPart = hasUnit ? value.substr(0, lastCharIndex) : value;
  if (numberPart.empty())
    return false;
  for (size_t i = 0; i < numberPart.size(); i++)
  {
    if (!isdigit(numberPart[i]))
      return false;
  }
  return true;
}

bool ConfigValidator::isValidIP(const std::string &ip)
{
  if (ip == "*" || ip == "0.0.0.0" || ip == "localhost")
    return true;

  std::vector<std::string> segments;
  std::stringstream ss(ip);
  std::string segment;
  while (std::getline(ss, segment, '.'))
  {
    segments.push_back(segment);
  }

  if (segments.size() != 4)
    return false;

  for (size_t i = 0; i < 4; i++)
  {
    if (segments[i].empty() || segments[i].size() > 3)
      return false;
    for (size_t j = 0; j < segments[i].size(); j++)
    {
      if (!isdigit(segments[i][j]))
        return false;
    }
    int val;
    std::stringstream valSS(segments[i]);
    valSS >> val;
    if (val < 0 || val > 255)
      return false;
  }
  return true;
}