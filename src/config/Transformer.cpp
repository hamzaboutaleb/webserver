#include "config/Transformer.hpp"
#include "utils/NetworkResolver.hpp"

Transformer::Transformer(Config &config) : config(config)
{
}

void Transformer::transform()
{
  const std::vector<ServerConfig> &serverConfigs = config.getServers();
  for (size_t i = 0; i < serverConfigs.size(); i++)
  {
    const ServerConfig &serverConfig = serverConfigs[i];
    Server *server = transformServer(serverConfig);
    servers.push_back(server);
  }
}

Server *Transformer::transformServer(const ServerConfig &serverConfig)
{
  std::map<std::string, std::vector<Directive> > directivesMap = serverConfig.getDirectivesMap();
  std::vector<Directive> listenDirectives = directivesMap["listen"];
  std::vector<Directive> serverNameDirectives = directivesMap["server_name"];
  std::string root = directivesMap["root"].at(0).getValues()[0];

  Server *server = new Server();

  for (size_t i = 0; i < listenDirectives.size(); i++)
  {
    std::string listen = listenDirectives[i].getValues()[0];
    std::pair<std::string, int> interfacePort = NetworkResolver::resolveListen(listen);
    server->setInterface(interfacePort.first, interfacePort.second);
  }

  for (size_t i = 0; i < serverNameDirectives.size(); i++)
  {
    std::vector<std::string> names = serverNameDirectives[i].getValues();
    for (size_t j = 0; j < names.size(); j++)
    {
      server->addHostname(names[j]);
    }
  }

  server->setRoot(root);

  const std::vector<LocationConfig> &locationConfigs = serverConfig.getLocations();
  for (size_t i = 0; i < locationConfigs.size(); i++)
  {
    server->addLocation(transformLocation(locationConfigs[i]));
  }

  return server;
}

Location *Transformer::transformLocation(const LocationConfig &locationConfig)
{
  Location *location = new Location(locationConfig.getPath());
  const std::vector<Directive> &directives = locationConfig.getDirectives();

  for (size_t i = 0; i < directives.size(); i++)
  {
    const Directive &directive = directives[i];
    if (directive.getKey() == "root")
    {
      location->setRoot(directive.getValues()[0]);
    }
    else
    {
      location->addDirective(directive.getKey(), directive.getValues());
    }
  }
  return location;
}

const std::vector<Server *> &Transformer::getServers() const
{
  return servers;
}