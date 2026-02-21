#include "config/ConfigValidator.hpp"
#include "utils/File.hpp"
#include <sstream>
#include <climits>

bool ConfigValidator::checkListenDirective(const Directive &directive)
{
  const std::vector<std::string> &values = directive.getValues();
  if (values.size() != 1)
  {
    reportInvalidDirective(directive, "listen directive requires exactly one value");
    return false;
  }
  const std::string &value = values[0];
  size_t colonPos = value.find(':');

  if (colonPos == std::string::npos)
  {
    if (!isValidPort(value))
    {
      reportInvalidDirective(directive, "Invalid port number: '" + value + "'");
      return false;
    }
  }
  else
  {
    std::string ip = value.substr(0, colonPos);
    std::string port = value.substr(colonPos + 1);

    if (!isValidIP(ip))
    {
      reportInvalidDirective(directive, "Invalid IP address: '" + ip + "'");
      return false;
    }
    if (!isValidPort(port))
    {
      reportInvalidDirective(directive, "Invalid port number: '" + port + "'");
      return false;
    }
  }
  return true;
}

bool ConfigValidator::checkRootDirective(const Directive &directive)
{
  const std::vector<std::string> &values = directive.getValues();
  if (values.size() != 1)
  {
    reportInvalidDirective(directive, "root directive requires exactly one value");
    return false;
  }

  if (!isValidRootPath(values[0]))
  {
    reportInvalidDirective(directive, "Invalid root path: '" + values[0] + "'. It should start with '/'");
    return false;
  }

  if (!File::exists(values[0]))
  {
    reportInvalidDirective(directive, "Root path does not exist: '" + values[0] + "'");
    return false;
  }
  if (!File::isDirectory(values[0]))
  {
    reportInvalidDirective(directive, "Root path cannot be a file: '" + values[0] + "'");
    return false;
  }

  return true;
}

bool ConfigValidator::checkServerNameDirective(const Directive &directive)
{
  const std::vector<std::string> &values = directive.getValues();
  if (values.empty())
  {
    reportInvalidDirective(directive, "server_name directive requires at least one value");
    return false;
  }

  for (size_t i = 0; i < values.size(); i++)
  {
    const std::string &serverName = values[i];
    if (serverName.size() > 255)
    {
      reportInvalidDirective(directive, "Server name is too long: '" + serverName + "'. Maximum length is 255 characters");
      return false;
    }
    if (!isValidServerName(serverName))
    {
      reportInvalidDirective(directive, "Invalid server name: '" + serverName + "'");
      return false;
    }
  }
  return true;
}

bool ConfigValidator::checkClientMaxBodySizeDirective(const Directive &directive)
{
  const std::vector<std::string> &values = directive.getValues();
  if (values.size() != 1)
  {
    reportInvalidDirective(directive, "client_max_body_size directive requires exactly one value");
    return false;
  }
  const std::string &value = values[0];
  if (!isValidSizeValue(value))
  {
    reportInvalidDirective(directive, "client_max_body_size value is invalid: '" + value + "'");
    return false;
  }

  size_t lastIndex = value.size() - 1;
  char unit = std::tolower(value[lastIndex]);
  bool hasUnit = (unit == 'k' || unit == 'm' || unit == 'g');
  std::string numericPart = hasUnit ? value.substr(0, lastIndex) : value;

  long long size;
  std::stringstream ss(numericPart);
  ss >> size;

  if (ss.fail() || size < 0)
  {
    reportInvalidDirective(directive, "client_max_body_size numeric value is invalid: '" + numericPart + "'");
    return false;
  }

  long long multiplier = 1;
  if (hasUnit)
  {
    if (unit == 'k')
      multiplier = 1024LL;
    else if (unit == 'm')
      multiplier = 1024LL * 1024LL;
    else if (unit == 'g')
      multiplier = 1024LL * 1024LL * 1024LL;
  }

  if (size > 0 && multiplier > 0 && size > (LLONG_MAX / multiplier))
  {
    reportInvalidDirective(directive, "client_max_body_size value is too large (numeric overflow)");
    return false;
  }

  long long finalSize = size * multiplier;
  if (finalSize > 2147483647LL)
  {
    reportInvalidDirective(directive, "client_max_body_size is too large (maximum 2GB)");
    return false;
  }

  return true;
}

bool ConfigValidator::checkIndexDirective(const Directive &directive)
{
  const std::vector<std::string> &values = directive.getValues();
  if (values.empty())
  {
    reportInvalidDirective(directive, "index directive requires at least one value");
    return false;
  }
  return true;
}

bool ConfigValidator::checkAutoindexDirective(const Directive &directive)
{
  const std::vector<std::string> &values = directive.getValues();
  if (values.size() != 1)
  {
    reportInvalidDirective(directive, "autoindex directive requires exactly one value");
    return false;
  }
  if (values[0] != "on" && values[0] != "off")
  {
    reportInvalidDirective(directive, "autoindex value must be 'on' or 'off': '" + values[0] + "'");
    return false;
  }
  return true;
}

bool ConfigValidator::checkErrorPageDirective(const Directive &directive)
{
  const std::vector<std::string> &values = directive.getValues();
  if (values.size() < 2)
  {
    reportInvalidDirective(directive, "error_page directive requires at least one status code and a URI");
    return false;
  }

  for (size_t i = 0; i < values.size() - 1; i++)
  {
    const std::string &codeStr = values[i];

    if (codeStr.size() != 3)
    {
      reportInvalidDirective(directive, "HTTP status code must be exactly 3 digits: '" + codeStr + "'");
      return false;
    }

    for (size_t j = 0; j < codeStr.size(); j++)
    {
      if (!isdigit(codeStr[j]))
      {
        reportInvalidDirective(directive, "Invalid HTTP status code: '" + codeStr + "'");
        return false;
      }
    }

    int code;
    std::stringstream ss(codeStr);
    ss >> code;

    if (code < 300 || code > 599)
    {
      reportInvalidDirective(directive, "HTTP status code out of range (300-599): '" + codeStr + "'");
      return false;
    }
  }
  return true;
}

bool ConfigValidator::checkReturnDirective(const Directive &directive)
{
  const std::vector<std::string> &values = directive.getValues();
  if (values.size() < 1 || values.size() > 2)
  {
    reportInvalidDirective(directive, "return directive requires 1 or 2 values (status code and/or URL)");
    return false;
  }

  if (values.size() == 2)
  {
    const std::string &codeStr = values[0];
    if (codeStr.size() != 3)
    {
      reportInvalidDirective(directive, "HTTP status code must be exactly 3 digits: '" + codeStr + "'");
      return false;
    }
    for (size_t i = 0; i < codeStr.size(); i++)
    {
      if (!isdigit(codeStr[i]))
      {
        reportInvalidDirective(directive, "Invalid HTTP status code: '" + codeStr + "'");
        return false;
      }
    }
    int code;
    std::stringstream ss(codeStr);
    ss >> code;
    if (code < 300 || code > 599)
    {
      reportInvalidDirective(directive, "HTTP status code out of range (300-599): '" + codeStr + "'");
      return false;
    }
  }
  return true;
}

bool ConfigValidator::checkUploadStoreDirective(const Directive &directive)
{
  const std::vector<std::string> &values = directive.getValues();
  if (values.size() != 1)
  {
    reportInvalidDirective(directive, "upload_store directive requires exactly one value");
    return false;
  }

  const std::string &path = values[0];
  if (!isValidRootPath(path))
  {
    reportInvalidDirective(directive, "Invalid upload_store path: '" + path + "'. It should start with '/'");
    return false;
  }

  if (!File::exists(path))
  {
    reportInvalidDirective(directive, "upload_store path does not exist: '" + path + "'");
    return false;
  }
  if (!File::isDirectory(path))
  {
    reportInvalidDirective(directive, "upload_store path must be a directory: '" + path + "'");
    return false;
  }

  return true;
}

bool ConfigValidator::checkCgiExtensionDirective(const Directive &directive)
{
  const std::vector<std::string> &values = directive.getValues();
  if (values.size() != 2)
  {
    reportInvalidDirective(directive, "cgi_extension directive requires exactly two values: <extension> <binary_path>");
    return false;
  }

  const std::string &extension = values[0];
  const std::string &binaryPath = values[1];

  if (extension.empty() || extension[0] != '.')
  {
    reportInvalidDirective(directive, "CGI extension must start with a dot: '" + extension + "'");
    return false;
  }

  if (!isValidRootPath(binaryPath))
  {
    reportInvalidDirective(directive, "CGI binary path must be an absolute path: '" + binaryPath + "'");
    return false;
  }

  if (!File::exists(binaryPath))
  {
    reportInvalidDirective(directive, "CGI binary path does not exist: '" + binaryPath + "'");
    return false;
  }

  if (File::isDirectory(binaryPath))
  {
    reportInvalidDirective(directive, "CGI binary path cannot be a directory: '" + binaryPath + "'");
    return false;
  }

  if (!File::isExecutable(binaryPath))
  {
    reportInvalidDirective(directive, "CGI binary path is not executable: '" + binaryPath + "'");
    return false;
  }

  return true;
}

bool ConfigValidator::checkMethodsDirective(const Directive &directive)
{
  const std::vector<std::string> &values = directive.getValues();
  if (values.empty())
  {
    reportInvalidDirective(directive, "methods directive requires at least one value");
    return false;
  }

  for (size_t i = 0; i < values.size(); i++)
  {
    const std::string &method = values[i];
    if (method != "GET" && method != "POST" && method != "DELETE")
    {
      reportInvalidDirective(directive, "Invalid HTTP method: '" + method + "'. Allowed methods are GET, POST, DELETE");
      return false;
    }
  }
  return true;
}