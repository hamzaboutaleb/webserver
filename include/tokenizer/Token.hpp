#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include "tokenizer/TokenType.hpp"
#include "utils/Span.hpp"

class Token
{
private:
  TokenType type;
  std::string value;
  Span span;

public:
  Token(TokenType type, const std::string &value, Span span);

  TokenType getType() const;
  std::string getValue() const;
  Span getSpan() const;
  std::string getTypeString();
};

#endif