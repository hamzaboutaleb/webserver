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
#include "core/ServerManager.hpp"
#include "core/Socket.hpp"

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

  const std::vector<Server *> servers = transformer.releaseServers();
  for (size_t i = 0; i < servers.size(); i++)
  {
    std::cout << "Server " << i << ":" << std::endl;
    servers[i]->print();
    std::cout << std::endl;
  }

  ServerManager manager;
  try
  {
    manager.setup(servers);
    manager.run();
  }
  catch (const std::exception &e)
  {
    std::cerr << "Fatal Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}