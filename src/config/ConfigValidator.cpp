#include "config/ConfigValidator.hpp"
#include <sstream>

// ----------- reporting functions -----------

void ConfigValidator::reportInvalidDirective(const Directive &directive, const std::string &message)
{
  errorReporter.report(Report(directive.getSpan(), message, false));
}
// -------------------- validation functions -------------------

bool ConfigValidator::isValidPort(const std::string &portStr)
{
  if (portStr.empty() || portStr.size() > 5)
    return false;
  for (size_t i = 0; i < portStr.size(); i++)
  {
    if (!isdigit(portStr[i]))
      return false;
  }
  int port;
  std::istringstream iss(portStr);
  iss >> port;
  return port > 0 && port <= 65535;
}

bool ConfigValidator::checkListenDirective(const Directive &directive)
{
  const std::vector<std::string> &values = directive.getValues();
  if (values.size() != 1)
  {
    reportInvalidDirective(directive, "listen directive requires exactly one value");
    return false;
  }
  if (!isValidPort(values[0]))
  {
    reportInvalidDirective(directive, "Invalid port number: '" + values[0] + "'");
    return false;
  }
  return true;
}

ConfigValidator::ConfigValidator(ErrorReporter &errorReporter) : errorReporter(errorReporter)
{
}

void ConfigValidator::validate(Config &config)
{
  for (size_t i = 0; i < config.getServers().size(); i++)
  {
    validateServerConfig(config.getServers()[i]);
  }
}

void ConfigValidator::validateDirective(const Directive &directive)
{
  const std::string &key = directive.getKey();
  const std::vector<std::string> &values = directive.getValues();
  if (values.empty())
  {
    errorReporter.report(Report(directive.getSpan(), "Directive '" + key + "' requires at least one value", false));
    return;
  }
  if (key == "listen")
  {
    if (values.size() != 1)
    {
      errorReporter.report(Report(directive.getSpan(), "Directive 'listen' requires exactly one value", false));
      return;
    }
    if (!isValidPort(values[0]))
    {
      errorReporter.report(Report(directive.getSpan(), "Invalid port number: '" + values[0] + "'", false));
    }
  }
}

void ConfigValidator::validateServerConfig(const ServerConfig &serverConfig)
{
  for (size_t i = 0; i < serverConfig.getDirectives().size(); i++)
  {
    validateDirective(serverConfig.getDirectives()[i]);
  }
  for (size_t i = 0; i < serverConfig.getLocations().size(); i++)
  {
    validateLocationConfig(serverConfig.getLocations()[i]);
  }
}

void ConfigValidator::validateLocationConfig(const LocationConfig &locationConfig)
{
  for (size_t i = 0; i < locationConfig.getDirectives().size(); i++)
  {
    validateDirective(locationConfig.getDirectives()[i]);
  }
}