#include "config/ConfigValidator.hpp"
#include <sstream>
#include "utils/File.hpp"
#include <set>

void ConfigValidator::reportInvalidDirective(const Directive &directive, const std::string &message)
{
  errorReporter.report(Report(directive.getSpan(), message, false));
}

void ConfigValidator::reportError(const Span &span, const std::string &message)
{
  errorReporter.report(Report(span, message, false));
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
  if (key == "listen" && !checkListenDirective(directive))
    return;
  if (key == "root" && !checkRootDirective(directive))
    return;
  if (key == "server_name" && !checkServerNameDirective(directive))
    return;
}

void ConfigValidator::validateServerConfig(const ServerConfig &serverConfig)
{
  validateRequiredDirectives(serverConfig);
  validateNoDuplicateDirectives(serverConfig);
  validateNoDuplicateLocations(serverConfig);
  for (size_t i = 0; i < serverConfig.getDirectives().size(); i++)
  {
    validateDirective(serverConfig.getDirectives()[i]);
  }

  for (size_t i = 0; i < serverConfig.getLocations().size(); i++)
  {
    validateLocationConfig(serverConfig.getLocations()[i]);
  }

  validateServerNameUniqueness(serverConfig);
}

void ConfigValidator::validateLocationConfig(const LocationConfig &locationConfig)
{
  for (size_t i = 0; i < locationConfig.getDirectives().size(); i++)
  {
    validateDirective(locationConfig.getDirectives()[i]);
  }
}
