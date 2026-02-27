#ifndef NETWORK_RESOLVER_HPP
#define NETWORK_RESOLVER_HPP

#include <string>

class NetworkResolver
{
public:
  static std::pair<std::string, int> resolveListen(const std::string &listenStr);
};

#endif