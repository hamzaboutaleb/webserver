#ifndef TRANSFORMER_HPP
#define TRANSFORMER_HPP

#include <string>
#include <vector>
#include "core/Server.hpp"
#include "core/Location.hpp"
#include "parser/ast/Config.hpp"
#include "parser/ast/LocationConfig.hpp"

class Transformer
{
private:
  std::vector<Server *> servers;
  Config &config;

public:
  Transformer(Config &config);
  void transform();
  Server *transformServer(const ServerConfig &serverConfig);
  Location *transformLocation(const LocationConfig &locationConfig);
  const std::vector<Server *> &getServers() const;
};

#endif