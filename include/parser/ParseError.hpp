#ifndef PARSE_ERROR_HPP
#define PARSE_ERROR_HPP

#include <stdexcept>

class ParseError : public std::exception
{
public:
  ParseError();
};

#endif