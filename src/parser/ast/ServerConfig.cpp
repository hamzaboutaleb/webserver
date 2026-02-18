#include "parser/ast/ServerConfig.hpp"

ServerConfig::ServerConfig(const std::vector<Directive> &directives, const std::vector<LocationConfig> &locations, const Span &span)
    : Node(span), directives(directives), locations(locations)
{
}

const std::vector<Directive> &ServerConfig::getDirectives() const
{
    return directives;
}

const std::vector<LocationConfig> &ServerConfig::getLocations() const
{
    return locations;
}