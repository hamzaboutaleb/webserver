#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <vector>
#include "parser/ast/Node.hpp"
#include "parser/ast/Directive.hpp"
#include "parser/ast/LocationConfig.hpp"

class ServerConfig : public Node
{
private:
  std::vector<Directive> directives;
  std::vector<LocationConfig> locations;

public:
  ServerConfig(const std::vector<Directive> &directives, const std::vector<LocationConfig> &locations, const Span &span);
};

#endif