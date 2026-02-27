#include "utils/NetworkResolver.hpp"
#include "utils/Number.hpp"
#include <algorithm>

std::pair<std::string, int> NetworkResolver::resolveListen(const std::string &listenStr)
{
  std::string defaultInterface = "0.0.0.0";
  int defaultPort = 80;

  size_t colonPos = listenStr.find(':');
  if (colonPos == std::string::npos)
  {
    // Check if the whole string is a port (numeric)
    bool isNumeric = true;
    for (size_t i = 0; i < listenStr.length(); ++i)
    {
      if (!isdigit(listenStr[i]))
      {
        isNumeric = false;
        break;
      }
    }

    if (isNumeric)
    {
      return std::make_pair(defaultInterface, Number::toInt(listenStr));
    }
    else
    {
      return std::make_pair(listenStr, defaultPort);
    }
  }

  std::string interface = listenStr.substr(0, colonPos);
  if (interface.empty())
    interface = defaultInterface;
  
  std::string portStr = listenStr.substr(colonPos + 1);
  int port = portStr.empty() ? defaultPort : Number::toInt(portStr);
  
  return std::make_pair(interface, port);
}