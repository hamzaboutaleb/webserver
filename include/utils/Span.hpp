#ifndef SPAN_HPP
#define SPAN_HPP

#include "utils/Position.hpp"

struct Span
{
  Position start;
  Position end;

  Span(Position start, Position end);
  Span();
};

#endif