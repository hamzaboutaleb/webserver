#ifndef POSITION_HPP
#define POSITION_HPP

#include <iostream>

struct Position
{
  unsigned int line;
  unsigned int col;
  unsigned int index;

  Position(unsigned int line, unsigned int col, unsigned int index);
  Position();
};

std::ostream &operator<<(std::ostream &os, const Position &pos);

#endif