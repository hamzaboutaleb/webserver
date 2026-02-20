#ifndef FILE_HPP
#define FILE_HPP

#include <string>

class File
{
public:
  static bool exists(const std::string &path);
  static bool isDirectory(const std::string &path);
  static bool isFile(const std::string &path);
};

#endif