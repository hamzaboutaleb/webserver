
#include "utils/Number.hpp"
#include <sstream>

bool Number::isDigits(const std::string &str)
{
  for (size_t i = 0; i < str.size(); i++)
  {
    if (!isdigit(str[i]))
    {
      return false;
    }
  }
  return true;
}

int Number::toInt(const std::string &str)
{
  int num;
  std::stringstream ss(str);
  ss >> num;
  return num;
}
