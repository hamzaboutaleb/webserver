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
  directiveValidators["listen"] = &ConfigValidator::checkListenDirective;
  directiveValidators["root"] = &ConfigValidator::checkRootDirective;
  directiveValidators["server_name"] = &ConfigValidator::checkServerNameDirective;
  directiveValidators["client_max_body_size"] = &ConfigValidator::checkClientMaxBodySizeDirective;
  directiveValidators["index"] = &ConfigValidator::checkIndexDirective;
  directiveValidators["autoindex"] = &ConfigValidator::checkAutoindexDirective;
  directiveValidators["error_page"] = &ConfigValidator::checkErrorPageDirective;
  directiveValidators["return"] = &ConfigValidator::checkReturnDirective;
  directiveValidators["upload_store"] = &ConfigValidator::checkUploadStoreDirective;
  directiveValidators["cgi_extension"] = &ConfigValidator::checkCgiExtensionDirective;
  directiveValidators["methods"] = &ConfigValidator::checkMethodsDirective;
}

const Directive *ConfigValidator::getDirective(const std::vector<Directive> &directives, const std::string &key)
{
  for (size_t i = 0; i < directives.size(); i++)
  {
    if (directives[i].getKey() == key)
      return &directives[i];
  }
  return NULL;
}

bool ConfigValidator::isDirectivePresent(const std::vector<Directive> &directives, const std::string &key)
{
  for (size_t i = 0; i < directives.size(); i++)
  {
    if (directives[i].getKey() == key)
      return true;
  }
  return false;
}

void ConfigValidator::validate(Config &config)
{
  usedServerNameLocationPairs.clear();
  for (size_t i = 0; i < config.getServers().size(); i++)
  {
    validateServerConfig(config.getServers()[i]);
  }
}

void ConfigValidator::validateDirective(const Directive &directive, Context context)
{
  const std::string &key = directive.getKey();
  
  if (context == LOCATION_CONTEXT && (key == "listen" || key == "server_name"))
  {
    reportInvalidDirective(directive, "The '" + key + "' directive is only allowed in server context");
    return;
  }

  std::map<std::string, ValidatorFunc>::iterator it = directiveValidators.find(key);
  if (it != directiveValidators.end())
  {
    ValidatorFunc func = it->second;
    (this->*func)(directive);
  }
  else
  {
    reportInvalidDirective(directive, "Unknown directive '" + key + "'");
  }
}

void ConfigValidator::validateServerConfig(const ServerConfig &serverConfig)
{
  validateRequiredDirectives(serverConfig);
  validateNoDuplicateDirectives(serverConfig);
  validateNoDuplicateLocations(serverConfig);

  for (size_t i = 0; i < serverConfig.getDirectives().size(); i++)
    validateDirective(serverConfig.getDirectives()[i], SERVER_CONTEXT);

  for (size_t i = 0; i < serverConfig.getLocations().size(); i++)
    validateLocationConfig(serverConfig.getLocations()[i]);

  if (errorReporter.hasErrors())
    return;
  validateServerNameUniqueness(serverConfig);
}

void ConfigValidator::validateLocationConfig(const LocationConfig &locationConfig)
{
  for (size_t i = 0; i < locationConfig.getDirectives().size(); i++)
    validateDirective(locationConfig.getDirectives()[i], LOCATION_CONTEXT);
}
