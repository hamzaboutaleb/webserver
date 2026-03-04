#include "utils/MimeTypes.hpp"
#include <map>

std::map<std::string, std::string> MimeTypes::mimeMap;
bool MimeTypes::initialized = false;

void MimeTypes::initialize()
{
  if (initialized)
    return;
  mimeMap[".html"] = "text/html";
  mimeMap[".htm"] = "text/html";
  mimeMap[".css"] = "text/css";
  mimeMap[".js"] = "application/javascript";
  mimeMap[".json"] = "application/json";
  mimeMap[".png"] = "image/png";
  mimeMap[".jpg"] = "image/jpeg";
  mimeMap[".jpeg"] = "image/jpeg";
  mimeMap[".gif"] = "image/gif";
  mimeMap[".ico"] = "image/x-icon";
  mimeMap[".svg"] = "image/svg+xml";
  mimeMap[".pdf"] = "application/pdf";
  mimeMap[".txt"] = "text/plain";
  initialized = true;
}

std::string MimeTypes::getMimeType(const std::string &path)
{
  initialize();
  size_t dotPos = path.find_last_of('.');
  if (dotPos == std::string::npos)
    return "application/octet-stream";

  std::string ext = path.substr(dotPos);
  std::map<std::string, std::string>::iterator it = mimeMap.find(ext);
  if (it != mimeMap.end())
    return it->second;

  return "application/octet-stream";
}