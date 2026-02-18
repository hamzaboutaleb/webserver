#include "parser/parser.hpp"
#include "parser/ParseError.hpp"

Parser::Parser(const std::vector<Token> &tokens, ErrorReporter &errorReporter)
    : tokenStream(tokens, errorReporter) {}

Config Parser::parse()
{
  return parseConfig();
}

Config Parser::parseConfig()
{
  std::vector<ServerConfig> servers;
  while (tokenStream.hasNext())
  {
    try
    {
      servers.push_back(parseServerConfig());
    }
    catch (const ParseError &e)
    {
      std::cout << "Error parsing server block: " << e.what() << std::endl;
      tokenStream.synchronizeServer();
    }
  }
  Span span = Span(servers.empty() ? Position() : servers.front().getSpan().start,
                   servers.empty() ? Position() : servers.back().getSpan().end);
  return Config(servers, span);
}

ServerConfig Parser::parseServerConfig()
{
  std::vector<Directive> directives;
  std::vector<LocationConfig> locations;

  Token start = tokenStream.consume(SERVER, "Expected 'server' keyword");
  tokenStream.consume(LEFT_BRACE, "Expected '{' after 'server'");

  while (!tokenStream.isAtEnd() && !tokenStream.check(RIGHT_BRACE))
  {
    try
    {
      if (tokenStream.check(LOCATION))
        locations.push_back(parseLocationConfig());
      else
        directives.push_back(parseDirective());
    }
    catch (const ParseError &e)
    {
      if (tokenStream.check(SERVER))
      {
        tokenStream.reportError("Unexpected start of server block inside a server block.");
        throw ParseError();
      }
      tokenStream.synchronize();
    }
  }
  Token end = tokenStream.consume(RIGHT_BRACE, "Expected '}' after server block");
  return ServerConfig(directives, locations, Span(start.getSpan().start, end.getSpan().end));
}

LocationConfig Parser::parseLocationConfig()
{
  std::vector<Directive> directives;
  std::string path;

  Token start = tokenStream.consume(LOCATION, "Expected 'location' keyword");
  path = tokenStream.consume(IDENTIFIER, "Expected location path").getValue();
  tokenStream.consume(LEFT_BRACE, "Expected '{' after 'location'");
  while (!tokenStream.isAtEnd() && !tokenStream.check(RIGHT_BRACE))
  {
    try
    {
      directives.push_back(parseDirective());
    }
    catch (const ParseError &e)
    {
      if (tokenStream.check(SERVER))
      {
        tokenStream.reportError("Unexpected start of server block inside a location block.");
        tokenStream.synchronize();
      }
      else if (tokenStream.check(LOCATION))
      {
        tokenStream.reportError("Nested locations are not supported.");
        tokenStream.synchronize();
      }
      else
        tokenStream.synchronize();
    }
  }
  Token end = tokenStream.consume(RIGHT_BRACE, "Expected '}' after location block");

  return LocationConfig(path, directives, Span(start.getSpan().start, end.getSpan().end));
}

Directive Parser::parseDirective()
{
  std::vector<std::string> values;
  if (tokenStream.check(LOCATION) || tokenStream.check(SERVER))
  {
    throw ParseError();
    // tokenStream.throwError("Unexpected start of block '" + tokenStream.peek().getValue() + "' inside a directive list.");
  }
  Token keyToken = tokenStream.consumeDirective();
  std::string key = keyToken.getValue();
  while (!tokenStream.isAtEnd() && !tokenStream.check(SEMICOLON))
  {
    values.push_back(tokenStream.consumeValue().getValue());
  }
  Token end = tokenStream.consume(SEMICOLON, "Expected ';' after directive");

  return Directive(key, values, Span(keyToken.getSpan().start, end.getSpan().end));
}