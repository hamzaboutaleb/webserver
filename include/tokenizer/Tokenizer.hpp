#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP
#include <string>
#include <vector>
#include <map>
#include "utils/Position.hpp"
#include "utils/ErrorReporter.hpp"
#include "tokenizer/Token.hpp"

class Tokenizer
{
private:
  std::string buffer;
  Position start;
  ErrorReporter &reporter;
  unsigned int cursor;
  unsigned int line;
  unsigned int col;

  std::map<std::string, TokenType> keywords;

public:
  Tokenizer(const std::string &buffer, ErrorReporter &reproter);
  std::vector<Token> tokenize();

private:
  Token getNextToken();
  char peek();
  char peekNext();
  char advance();
  bool match(char c);
  bool isEof();

  void skipComments();
  void skipWhitespace();
  bool isWhitespace(char c);
  bool isIdentifierChar(char c);

  Token readIdentifier();

  Token newToken(TokenType type);
  Position position();

  Token reportUnknown();
};

#endif