#include "parser/ast/Directive.hpp"

Directive::Directive(const std::string &key, const std::vector<std::string> &values, Span span)
    : Node(span), key(key), values(values)
{
}

const std::string &Directive::getKey() const
{
    return key;
}

const std::vector<std::string> &Directive::getValues() const
{
    return values;
}