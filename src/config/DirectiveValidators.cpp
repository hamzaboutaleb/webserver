#include "config/ConfigValidator.hpp"
#include "utils/File.hpp"

bool ConfigValidator::checkListenDirective(const Directive &directive)
{
  const std::vector<std::string> &values = directive.getValues();
  if (values.size() != 1)
  {
    reportInvalidDirective(directive, "listen directive requires exactly one value");
    return false;
  }
  const std::string &port = values[0];
  if (!isValidPort(port))
  {
    reportInvalidDirective(directive, "Invalid port number: '" + port + "'");
    return false;
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