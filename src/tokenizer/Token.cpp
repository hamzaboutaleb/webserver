#include "tokenizer/Token.hpp"

Token::Token(TokenType type, const std::string &value, Span span) : type(type), value(value), span(span)
{
}

TokenType Token::getType() const
{
  return type;
}

std::string Token::getValue() const
{
  return value;
}

Span Token::getSpan() const
{
  return span;
}

std::string Token::getTypeString()
{
  switch (type)
  {
  case LEFT_BRACE:
    return "{";
  case RIGHT_BRACE:
    return "}";
  case SEMICOLON:
    return ";";
  case SERVER:
    return "server";
  case IDENTIFIER:
    return "identifier";
  case END_OF_FILE:
    return "eof";
  default:
    return "unknown";
  };
}