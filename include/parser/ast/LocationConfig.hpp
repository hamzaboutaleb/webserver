#ifndef LOCATION_CONFIG_HPP
#define LOCATION_CONFIG_HPP

#include <vector>
#include <string>
#include "parser/ast/Directive.hpp"
#include "parser/ast/Node.hpp"

class LocationConfig : public Node
{
  std::string path;
  std::vector<Directive> directives;

public:
  LocationConfig(const std::string &path, const std::vector<Directive> &directives, const Span &span);
  const std::string &getPath() const;
  const std::vector<Directive> &getDirectives() const;
};

#endif