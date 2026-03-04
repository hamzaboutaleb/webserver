#ifndef MIME_TYPES_HPP
#define MIME_TYPES_HPP

#include <string>
#include <map>

class MimeTypes
{
  static std::map<std::string, std::string> mimeMap;
  static bool initialized;

  static void initialize();

public:
  static std::string getMimeType(const std::string &path);
};

#endif