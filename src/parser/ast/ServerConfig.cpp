#include "parser/ast/ServerConfig.hpp"

ServerConfig::ServerConfig(const std::vector<Directive> &directives, const std::vector<LocationConfig> &locations, const Span &span)
    : Node(span), directives(directives), locations(locations)
{
}