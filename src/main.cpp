#include <sys/socket.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "utils/ErrorReporter.hpp"
#include "tokenizer/Tokenizer.hpp"
#include "parser/parser.hpp"
#include "config/ConfigValidator.hpp"
#include "core/EventLoop.hpp"
#include "config/Transformer.hpp"

std::string readFile(const std::string &filename)
{
  std::ifstream file(filename.c_str());
  if (!file.is_open())
  {
    return "";
  }
  std::ostringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  std::string filename = argv[1];
  std::string buffer = readFile(filename);
  ErrorReporter errorReporter(buffer, filename);
  Tokenizer tokenizer(buffer, errorReporter);
  std::vector<Token> tokens = tokenizer.tokenize();
  if (errorReporter.hasErrors())
  {
    errorReporter.printAll();
    return 1;
  }
  // for (size_t i = 0; i < tokens.size(); i++)
  // {
  //   const Token &token = tokens[i];
  //   std::cout << "Token: '" << token.getValue() << "' at line " << token.getSpan().start.line << ", column " << token.getSpan().start.col << std::endl;
  // }
  Parser parser(tokens, errorReporter);
  Config config = parser.parse();
  if (errorReporter.hasErrors())
  {
    errorReporter.printAll();
    return 1;
  }
  ConfigValidator validator(errorReporter);
  validator.validate(config);
  if (errorReporter.hasErrors())
  {
    errorReporter.printAll();
    return 1;
  }

  Transformer transformer(config);
  transformer.transform();

  const std::vector<Server *> &servers = transformer.getServers();
  for (size_t i = 0; i < servers.size(); i++)
  {
    Server *server = servers[i];
    std::cout << "Server " << i << ":" << std::endl;
    
    std::cout << "  Listen interfaces:" << std::endl;
    const std::vector<std::pair<std::string, int> > &interfaces = server->getListenInterfaces();
    for (size_t j = 0; j < interfaces.size(); j++)
    {
      std::cout << "    - " << (interfaces[j].first.empty() ? "*" : interfaces[j].first) << ":" << interfaces[j].second << std::endl;
    }

    std::cout << "  Hostnames:" << std::endl;
    const std::set<std::string> &hostnames = server->getHostnames();
    for (std::set<std::string>::const_iterator it = hostnames.begin(); it != hostnames.end(); ++it)
    {
      std::cout << "    - " << (it->empty() ? "(default)" : *it) << std::endl;
    }

    std::cout << "  Root: " << server->getRoot() << std::endl;

    const std::vector<Location *> &locations = server->getLocations();
    if (!locations.empty())
    {
      std::cout << "  Locations:" << std::endl;
      for (size_t j = 0; j < locations.size(); j++)
      {
        Location *loc = locations[j];
        std::cout << "    - Path: " << loc->getPath() << std::endl;
        if (!loc->getRoot().empty())
          std::cout << "      Root: " << loc->getRoot() << std::endl;
        
        const std::map<std::string, std::vector<std::string> > &directives = loc->getDirectives();
        for (std::map<std::string, std::vector<std::string> >::const_iterator it = directives.begin(); it != directives.end(); ++it)
        {
          std::cout << "      " << it->first << ":";
          for (size_t k = 0; k < it->second.size(); k++)
            std::cout << " " << it->second[k];
          std::cout << std::endl;
        }
      }
    }
    std::cout << std::endl;
  }
  
  return 0;
}