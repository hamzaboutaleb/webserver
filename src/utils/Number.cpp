
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

int Number::toInt(const std::string &str, bool *ok)
{
  int num = 0;
  std::stringstream ss(str);
  ss >> num;
  bool success = !ss.fail() && ss.eof();
  if (ok)
    *ok = success;
  return success ? num : 0;
}

std::string Number::toString(long long n)
{
  std::stringstream ss;
  ss << n;
  return ss.str();
}
