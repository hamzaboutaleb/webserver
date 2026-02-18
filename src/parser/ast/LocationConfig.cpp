#include "parser/ast/LocationConfig.hpp"

LocationConfig::LocationConfig(const std::string &path, const std::vector<Directive> &directives, const Span &span)
    : Node(span), path(path), directives(directives)
{
}