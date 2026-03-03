#include "config/Transformer.hpp"
#include "utils/NetworkResolver.hpp"
#include "utils/Number.hpp"

Transformer::Transformer(Config &config) : config(config) {}

void Transformer::transform() {
  const std::vector<ServerConfig> &serverConfigs = config.getServers();
  for (size_t i = 0; i < serverConfigs.size(); i++) {
    const ServerConfig &serverConfig = serverConfigs[i];
    Server *server = transformServer(serverConfig);
    servers.push_back(server);
  }
}

static std::string getFirstValue(
    std::map<std::string, std::vector<Directive> > &directivesMap,
    const std::string &key) {
  if (directivesMap.count(key) && !directivesMap[key].empty() &&
      !directivesMap[key].at(0).getValues().empty()) {
    return directivesMap[key].at(0).getValues()[0];
  }
  return "";
}

Server *Transformer::transformServer(const ServerConfig &serverConfig) {
  std::map<std::string, std::vector<Directive> > directivesMap =
      serverConfig.getDirectivesMap();

  Server *server = new Server();

  // listen
  std::vector<Directive> listenDirectives = directivesMap["listen"];
  for (size_t i = 0; i < listenDirectives.size(); i++) {
    std::string listen = listenDirectives[i].getValues()[0];
    std::pair<std::string, int> interfacePort =
        NetworkResolver::resolveListen(listen);
    server->setInterface(interfacePort.first, interfacePort.second);
  }

  // server_name
  std::vector<Directive> serverNameDirectives = directivesMap["server_name"];
  for (size_t i = 0; i < serverNameDirectives.size(); i++) {
    std::vector<std::string> names = serverNameDirectives[i].getValues();
    for (size_t j = 0; j < names.size(); j++) {
      server->addHostname(names[j]);
    }
  }

  // root
  std::string root = getFirstValue(directivesMap, "root");
  if (!root.empty())
    server->setRoot(root);

  // index
  std::string index = getFirstValue(directivesMap, "index");
  if (!index.empty())
    server->setIndex(index);

  // autoindex
  std::string autoindex = getFirstValue(directivesMap, "autoindex");
  if (autoindex == "on")
    server->setAutoindex(true);
  else if (autoindex == "off")
    server->setAutoindex(false);

  // client_max_body_size
  std::string maxBody = getFirstValue(directivesMap, "client_max_body_size");
  if (!maxBody.empty())
    server->setMaxClientBodySize(Number::toInt(maxBody));

  // methods
  if (directivesMap.count("methods") && !directivesMap["methods"].empty()) {
    server->setMethods(directivesMap["methods"].at(0).getValues());
  }

  // error_page
  if (directivesMap.count("error_page")) {
    std::vector<Directive> &errorPageDirectives = directivesMap["error_page"];
    for (size_t i = 0; i < errorPageDirectives.size(); i++) {
      const std::vector<std::string> &vals = errorPageDirectives[i].getValues();
      if (vals.size() >= 2) {
        std::string uri = vals[vals.size() - 1];
        for (size_t j = 0; j < vals.size() - 1; j++) {
          server->addErrorPage(Number::toInt(vals[j]), uri);
        }
      }
    }
  }

  // return
  if (directivesMap.count("return") && !directivesMap["return"].empty()) {
    const std::vector<std::string> &vals =
        directivesMap["return"].at(0).getValues();
    if (vals.size() == 1)
      server->setReturn(Number::toInt(vals[0]), "");
    else if (vals.size() == 2)
      server->setReturn(Number::toInt(vals[0]), vals[1]);
  }

  // upload_store
  std::string uploadStore = getFirstValue(directivesMap, "upload_store");
  if (!uploadStore.empty())
    server->setUploadStore(uploadStore);

  // cgi_extension
  if (directivesMap.count("cgi_extension")) {
    std::vector<Directive> &cgiDirectives = directivesMap["cgi_extension"];
    for (size_t i = 0; i < cgiDirectives.size(); i++) {
      const std::vector<std::string> &vals = cgiDirectives[i].getValues();
      if (vals.size() == 2)
        server->addCgiExtension(vals[0], vals[1]);
    }
  }

  // Locations
  const std::vector<LocationConfig> &locationConfigs =
      serverConfig.getLocations();
  for (size_t i = 0; i < locationConfigs.size(); i++) {
    server->addLocation(transformLocation(locationConfigs[i], server));
  }

  return server;
}

Location *Transformer::transformLocation(const LocationConfig &locationConfig,
                                         Server *server) {
  Location *location = new Location(locationConfig.getPath());
  location->setServer(server);

  const std::vector<Directive> &directives = locationConfig.getDirectives();
  for (size_t i = 0; i < directives.size(); i++) {
    const Directive &directive = directives[i];
    const std::string &key = directive.getKey();
    const std::vector<std::string> &vals = directive.getValues();

    if (key == "root") {
      location->setRoot(vals[0]);
    } else if (key == "index") {
      location->setIndex(vals[0]);
    } else if (key == "autoindex") {
      location->setAutoindex(vals[0] == "on");
    } else if (key == "client_max_body_size") {
      location->setMaxClientBodySize(Number::toInt(vals[0]));
    } else if (key == "methods") {
      location->setMethods(vals);
    } else if (key == "error_page" && vals.size() >= 2) {
      std::string uri = vals[vals.size() - 1];
      for (size_t j = 0; j < vals.size() - 1; j++) {
        location->addErrorPage(Number::toInt(vals[j]), uri);
      }
    } else if (key == "return") {
      if (vals.size() == 1)
        location->setReturn(Number::toInt(vals[0]), "");
      else if (vals.size() == 2)
        location->setReturn(Number::toInt(vals[0]), vals[1]);
    } else if (key == "upload_store") {
      location->setUploadStore(vals[0]);
    } else if (key == "cgi_extension" && vals.size() == 2) {
      location->addCgiExtension(vals[0], vals[1]);
    }
  }
  return location;
}

const std::vector<Server *> &Transformer::getServers() const { return servers; }

std::vector<Server *> Transformer::releaseServers() {
  std::vector<Server *> released = servers;
  servers.clear();
  return released;
}