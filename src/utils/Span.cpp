#include "utils/Span.hpp"

Span::Span(Position start, Position end)
{
  this->start = start;
  this->end = end;
}

Span::Span()
{
}