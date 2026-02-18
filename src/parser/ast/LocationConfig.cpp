#include "parser/ast/LocationConfig.hpp"

LocationConfig::LocationConfig(const std::string &path, const std::vector<Directive> &directives, const Span &span)
    : Node(span), path(path), directives(directives)
{
}

const std::string &LocationConfig::getPath() const
{
    return path;
}

const std::vector<Directive> &LocationConfig::getDirectives() const
{
    return directives;
}