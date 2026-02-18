#ifndef DIRECTIVE_HPP
#define DIRECTIVE_HPP

#include <string>
#include <vector>

#include "parser/ast/Node.hpp"

struct Directive : public Node
{
  std::string key;
  std::vector<std::string> values;

  Directive(const std::string &key, const std::vector<std::string> &values, Span span);
};

#endif