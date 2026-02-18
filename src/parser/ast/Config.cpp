#include "parser/ast/Config.hpp"

Config::Config(const std::vector<ServerConfig> &servers, const Span &span)
    : Node(span), servers(servers)
{
}
