#ifndef DIRECTIVE_HPP
#define DIRECTIVE_HPP

#include <string>
#include <vector>

#include "parser/ast/Node.hpp"

class Directive : public Node
{
private:
  std::string key;
  std::vector<std::string> values;

public:
  Directive(const std::string &key, const std::vector<std::string> &values, Span span);
  const std::string &getKey() const;
  const std::vector<std::string> &getValues() const;
};

#endif