#include <sys/socket.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "utils/ErrorReporter.hpp"
#include "tokenizer/Tokenizer.hpp"

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
}