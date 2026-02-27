#include "config/ConfigValidator.hpp"
#include "utils/NetworkResolver.hpp"
#include <set>
#include <sstream>

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
  std::vector<std::string> rawListenValues;
  std::vector<std::string> serverNames;
  Span serverNameSpan = Span();

  const std::vector<Directive> &directives = serverConfig.getDirectives();
  for (size_t i = 0; i < directives.size(); i++)
  {
    const Directive &directive = directives[i];
    if (directive.getKey() == "listen" && directive.getValues().size() == 1)
      rawListenValues.push_back(directive.getValues()[0]);
    else if (directive.getKey() == "server_name")
    {
      std::vector<std::string> values = directive.getValues();
      serverNames.insert(serverNames.end(), values.begin(), values.end());
      serverNameSpan = directive.getSpan();
    }
  }

  if (serverNames.empty())
  {
    serverNames.push_back("");
    serverNameSpan = serverConfig.getSpan();
  }

  for (size_t p = 0; p < rawListenValues.size(); p++)
  {
    std::pair<std::string, int> interfacePort = NetworkResolver::resolveListen(rawListenValues[p]);
    std::string interface = interfacePort.first;
    int port = interfacePort.second;
    std::string portStr;
    {
       std::ostringstream ss;
       ss << port;
       portStr = ss.str();
    }

    for (size_t i = 0; i < serverNames.size(); i++)
    {
      const std::string &serverName = serverNames[i];
      
      // 1. Exact Match Check (Interface:Port + ServerName)
      std::pair<std::string, std::string> exactKey = std::make_pair(rawListenValues[p], serverName);
      if (usedServerNameLocationPairs.find(exactKey) != usedServerNameLocationPairs.end())
      {
        Span previousSpan = usedServerNameLocationPairs[exactKey];
        reportError(serverNameSpan, "Duplicate server_name '" + serverName + "' with listen '" + rawListenValues[p] + "'");
        reportError(previousSpan, "Previous declaration of this server_name and listen combination");
      }
      else
      {
        usedServerNameLocationPairs[exactKey] = serverNameSpan;
      }

      // 2. Wildcard Conflict Detection (0.0.0.0:Port + ServerName)
      if (interface == "0.0.0.0")
      {
         portToWildcardSpan[port] = serverConfig.getSpan();

         if (hostnamesOnPort[port].count(serverName))
         {
            Span prevSpan = hostnamesOnPort[port][serverName];
            reportError(serverNameSpan, "Conflict: server_name '" + serverName + "' on port " + portStr + " is already used, and 0.0.0.0 listener is present");
            reportError(prevSpan, "Previous declaration on the same port");
         }
      }
      else if (portToWildcardSpan.count(port))
      {
          if (hostnamesOnPort[port].count(serverName))
          {
             Span prevSpan = hostnamesOnPort[port][serverName];
             reportError(serverNameSpan, "Conflict: server_name '" + serverName + "' on port " + portStr + " conflicts with 0.0.0.0 listener");
             reportError(prevSpan, "Previous declaration on the same port");
          }
      }

      // Record this hostname for this port
      if (hostnamesOnPort[port].find(serverName) == hostnamesOnPort[port].end())
      {
          hostnamesOnPort[port][serverName] = serverNameSpan;
      }
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
    if (key == "listen" || key == "server_name")
    {
      continue;
    }
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