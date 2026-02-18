#include "parser/ast/Node.hpp"

Node::Node(const Span &span) : span(span) {}
Node::~Node()
{
}
Span Node::getSpan() const
{
  return span;
}
