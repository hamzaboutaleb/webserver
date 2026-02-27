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

std::map<std::string, std::vector<Directive> > ServerConfig::getDirectivesMap() const
{
    std::map<std::string, std::vector<Directive> > directivesMap;
    for (size_t i = 0; i < directives.size(); i++)
    {
        const Directive &dir = directives[i];
        std::string key = dir.getKey();
        directivesMap[key].push_back(dir);
    }
    return directivesMap;
}