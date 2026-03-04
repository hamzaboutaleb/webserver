#include <sys/socket.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <signal.h>

#include "utils/ErrorReporter.hpp"
#include "tokenizer/Tokenizer.hpp"
#include "parser/parser.hpp"
#include "config/ConfigValidator.hpp"
#include "core/EventLoop.hpp"
#include "config/Transformer.hpp"
#include "core/ServerManager.hpp"
#include "core/Socket.hpp"

ServerManager *g_manager = NULL;

void handle_sigint(int sig)
{
  (void)sig;
  if (g_manager)
  {
    std::cout << "\nStopping server gracefully..." << std::endl;
    g_manager->stop();
  }
}

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

  signal(SIGINT, handle_sigint);

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

  const std::vector<Server *> servers = transformer.releaseServers();
  for (size_t i = 0; i < servers.size(); i++)
  {
    std::cout << "Server " << i << ":" << std::endl;
    servers[i]->print();
    std::cout << std::endl;
  }

  ServerManager manager;
  g_manager = &manager;
  try
  {
    manager.setup(servers);
    manager.run();
  }
  catch (const std::exception &e)
  {
    std::cerr << "Fatal Error: " << e.what() << std::endl;
    g_manager = NULL;
    return 1;
  }

  g_manager = NULL;
  return 0;
}