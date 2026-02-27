#ifndef NUMBER_HPP
#define NUMBER_HPP

#include <string>

class Number
{
public:
  static bool isDigits(const std::string &str);
  static int toInt(const std::string &str);
};

#endif
