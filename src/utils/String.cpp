#include "utils/String.hpp"

std::string String::toLower(const std::string &str)
{
  std::string result = str;
  for (size_t i = 0; i < result.size(); i++)
  {
    if (result[i] >= 'A' && result[i] <= 'Z')
    {
      result[i] = result[i] - 'A' + 'a';
    }
  }
  return result;
}

std::string String::trim(const std::string &str)
{
  size_t first = str.find_first_not_of(" \t");
  if (first == std::string::npos)
    return "";
  size_t last = str.find_last_not_of(" \t");
  return str.substr(first, (last - first + 1));
}