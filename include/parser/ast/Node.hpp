#ifndef NODE_HPP
#define NODE_HPP
#include "utils/Span.hpp"

class Node
{
protected:
  Span span;

public:
  Node(const Span &span);
  virtual ~Node();
  Span getSpan() const;
};

#endif