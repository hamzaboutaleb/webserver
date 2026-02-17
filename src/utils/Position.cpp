#include "utils/Position.hpp"

Position::Position() : line(1), col(1), index(0)
{
}

Position::Position(unsigned int line, unsigned int col, unsigned int index) : line(line), col(col), index(index)
{
}

std::ostream &operator<<(std::ostream &os, const Position &pos)
{
  os << "[" << pos.line << ":" << pos.col << "]";
  return os;
}