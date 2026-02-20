#include "utils/File.hpp"
#include <sys/stat.h>
#include <unistd.h>

bool File::exists(const std::string &path)
{
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}

bool File::isDirectory(const std::string &path)
{
  struct stat buffer;
  if (stat(path.c_str(), &buffer) != 0)
    return false;
  return S_ISDIR(buffer.st_mode);
}

bool File::isFile(const std::string &path)
{
  struct stat buffer;
  if (stat(path.c_str(), &buffer) != 0)
    return false;
  return S_ISREG(buffer.st_mode);
}