#include <cctype>
#include "tokenizer/Tokenizer.hpp"

Tokenizer::Tokenizer(const std::string &buffer, ErrorReporter &reporter) : buffer(buffer), reporter(reporter), cursor(0), line(1), col(1)
{
  keywords["server"] = SERVER;
  keywords["listen"] = LISTEN;
  keywords["root"] = ROOT;
  keywords["index"] = INDEX;
  keywords["location"] = LOCATION;
  keywords["error_page"] = ERROR_PAGE;
  keywords["allow_methods"] = ALLOW_METHODS;
  keywords["auto_index"] = AUTO_INDEX;
  keywords["cgi_pass"] = CGI_PASS;
  keywords["cgi_extensions"] = CGI_EXTENSIONS;
  keywords["upload_dir"] = UPLOAD_DIR;
  keywords["client_max_body_size"] = CLIENT_MAX_BODY_SIZE;
}

std::vector<Token> Tokenizer::tokenize()
{
  std::vector<Token> tokens;

  while (!isEof())
  {
    skipWhitespace();
    if (isEof())
      break;
    start = position();
    Token token = getNextToken();
    if (token.getType() == UNKNOWN)
      continue;
    tokens.push_back(token);
  }
  tokens.push_back(newToken(END_OF_FILE));
  return tokens;
}

Token Tokenizer::getNextToken()
{
  char c = advance();
  if (c == '{')
    return newToken(LEFT_BRACE);
  else if (c == '}')
    return newToken(RIGHT_BRACE);
  else if (c == ';')
    return newToken(SEMICOLON);
  else if (isIdentifierChar(c))
    return readIdentifier();
  return reportUnknown();
}

Token Tokenizer::readIdentifier()
{
  while (isIdentifierChar(peek()))
    advance();
  std::string value = buffer.substr(start.index, cursor - start.index);
  TokenType type = IDENTIFIER;
  if (keywords.find(value) != keywords.end())
    type = keywords[value];
  return newToken(type);
}

Token Tokenizer::newToken(TokenType type)
{
  std::string value = buffer.substr(start.index, cursor - start.index);
  Position end = position();
  Span span = Span(start, end);
  return Token(type, value, span);
}

Token Tokenizer::reportUnknown()
{
  Token token = newToken(UNKNOWN);
  std::string message = "unexpected character ";
  if (isprint(token.getValue().at(0)))
    message = "unexpected character '" + token.getValue() + "'";
  Report report = Report(token.getSpan(), message, false);
  reporter.report(report);
  return token;
}

Position Tokenizer::position()
{
  return Position(line, col, cursor);
}

void Tokenizer::skipWhitespace()
{
  while (!isEof())
  {
    if (isWhitespace(peek()))
      advance();
    else if (peek() == '#')
      skipComments();
    else
      break;
  }
}

void Tokenizer::skipComments()
{
  while (!isEof() && peek() != '\n')
    advance();
  if (!isEof())
    advance();
}

bool Tokenizer::isIdentifierChar(char c)
{
  return isalnum(c) || c == '/' || c == '_' || c == '.' || c == '-' || c == ':';
}

bool Tokenizer::isWhitespace(char c)
{
  return (c >= 9 && c <= 13) || c == ' ';
}

bool Tokenizer::isEof()
{
  return cursor >= buffer.size();
}

char Tokenizer::advance()
{
  char c = peek();
  cursor++;
  if (c == '\n')
  {
    line++;
    col = 1;
  }
  else
    col++;
  return c;
}

char Tokenizer::peek()
{
  if (isEof())
    return '\0';
  return buffer.at(cursor);
}
char Tokenizer::peekNext()
{
  if (cursor + 1 >= buffer.size())
    return '\0';
  return buffer.at(cursor + 1);
}

bool Tokenizer::match(char c)
{
  char curr = peek();
  if (curr == c)
  {
    advance();
    return true;
  }
  return false;
}
