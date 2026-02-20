#include "config/ConfigValidator.hpp"
#include <set>

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