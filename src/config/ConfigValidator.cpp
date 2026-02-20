#include "config/ConfigValidator.hpp"
#include <sstream>
#include "utils/File.hpp"
#include <set>

// ----------- reporting functions -----------

void ConfigValidator::reportInvalidDirective(const Directive &directive, const std::string &message)
{
  errorReporter.report(Report(directive.getSpan(), message, false));
}

void ConfigValidator::reportError(const Span &span, const std::string &message)
{
  errorReporter.report(Report(span, message, false));
}
// -------------------- validation functions -------------------

bool ConfigValidator::isValidPort(const std::string &portStr)
{
  if (portStr.empty() || portStr.size() > 5)
    return false;
  if (portStr.size() > 5)
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

bool ConfigValidator::isValidRootPath(const std::string &path)
{
  if (path.empty())
    return false;
  if (path[0] != '/')
    return false;
  return true;
}

bool ConfigValidator::isValidServerName(const std::string &name)
{
  if (name.empty())
    return false;
  if (name[0] == '-' || name[0] == '.' || name[name.size() - 1] == '-' || name[name.size() - 1] == '.')
    return false;
  for (size_t i = 0; i < name.size(); i++)
  {
    char c = name[i];
    if (isspace(c) || (!isalnum(c) && c != '-' && c != '.'))
      return false;
  }
  return true;
}

// ----------- directive-specific validation functions -----------

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

// ----------- helper functions for validating directive values -----------

void ConfigValidator::validateRequiredDirectives(const ServerConfig &serverConfig)
{
  std::set<std::string> requiredDirectives;
  requiredDirectives.insert("listen");
  requiredDirectives.insert("root");

  const std::vector<Directive> &directives = serverConfig.getDirectives();
  for (size_t i = 0; i < directives.size(); i++)
  {
    requiredDirectives.erase(directives[i].getKey());
  }

  for (std::set<std::string>::const_iterator it = requiredDirectives.begin(); it != requiredDirectives.end(); ++it)
  {
    reportError(serverConfig.getSpan(), "Server block is missing required directive: '" + *it + "'");
  }
}

void ConfigValidator::validateServerNameUniqueness(const ServerConfig &serverConfig)
{
  std::string port;
  std::vector<std::string> serverNames;
  Span serverNameSpan = Span();

  const std::vector<Directive> &directives = serverConfig.getDirectives();
  for (size_t i = 0; i < directives.size(); i++)
  {
    const Directive &directive = directives[i];
    if (directive.getKey() == "listen" && directive.getValues().size() == 1)
      port = directive.getValues()[0];
    else if (directive.getKey() == "server_name")
    {
      serverNames = directive.getValues();
      serverNameSpan = directive.getSpan();
    }
  }

  if (serverNames.empty())
  {
    serverNames.push_back("");
    serverNameSpan = serverConfig.getSpan();
  }

  for (size_t i = 0; i < serverNames.size(); i++)
  {
    const std::string &serverName = serverNames[i];
    std::pair<std::string, std::string> key = std::make_pair(port, serverName);

    if (usedServerNameLocationPairs.find(key) != usedServerNameLocationPairs.end())
    {
      Span previousSpan = usedServerNameLocationPairs[key];
      if (serverName.empty())
      {
        reportError(serverNameSpan, "Duplicate listen port '" + port + "' with default server_name");
        reportError(previousSpan, "Previous declaration of this listen port with default server_name");
      }
      else
      {
        reportError(serverNameSpan, "Duplicate server_name '" + serverName + "' with listen port '" + port + "'");
        reportError(previousSpan, "Previous declaration of this server_name and port combination");
      }
    }
    else
    {
      usedServerNameLocationPairs[key] = serverNameSpan;
    }
  }
}

void ConfigValidator::validateNoDuplicateDirectives(const ServerConfig &serverConfig)
{
  std::set<std::string> seen;
  const std::vector<Directive> &directives = serverConfig.getDirectives();

  for (size_t i = 0; i < directives.size(); i++)
  {
    const Directive &directive = directives[i];
    const std::string &key = directive.getKey();

    if (seen.find(key) != seen.end())
    {
      reportError(directive.getSpan(), "Duplicate directive '" + key + "'");
    }
    else
    {
      seen.insert(key);
    }
  }
}

void ConfigValidator::validateNoDuplicateLocations(const ServerConfig &serverConfig)
{
  std::map<std::string, Span> seen;
  const std::vector<LocationConfig> &locations = serverConfig.getLocations();

  for (size_t i = 0; i < locations.size(); i++)
  {
    const LocationConfig &location = locations[i];
    const std::string &path = location.getPath();

    if (seen.find(path) != seen.end())
    {
      reportError(location.getSpan(), "Duplicate location '" + path + "'");
      reportError(seen[path], "Previous declaration of location '" + path + "'");
    }
    else
    {
      seen[path] = location.getSpan();
    }
  }
}