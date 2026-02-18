#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include "parser/ast/ServerConfig.hpp"

class Config : public Node
{
private:
  std::vector<ServerConfig> servers;

public:
  Config(const std::vector<ServerConfig> &servers, const Span &span);
  const std::vector<ServerConfig> &getServers() const;
};

#endif