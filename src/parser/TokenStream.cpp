#include "parser/TokenStream.hpp"
#include "parser/ParseError.hpp"

TokenStream::TokenStream(const std::vector<Token> &tokens, ErrorReporter &errorReporter)
    : tokens(tokens), errorReporter(errorReporter), position(0)
{
    directives.insert(LISTEN);
    directives.insert(ROOT);
    directives.insert(INDEX);
    directives.insert(SERVER_NAME);
    directives.insert(ERROR_PAGE);
    directives.insert(ALLOW_METHODS);
    directives.insert(AUTO_INDEX);
    directives.insert(CGI_PASS);
    directives.insert(CGI_EXTENSIONS);
    directives.insert(UPLOAD_DIR);
    directives.insert(CLIENT_MAX_BODY_SIZE);
}

const Token &TokenStream::peek() const
{
    if (isAtEnd())
        return tokens.back();
    return tokens[position];
}

void TokenStream::reportError(const std::string &message)
{
    if (!isAtEnd())
    {
        errorReporter.report(Report(peek().getSpan(), message, false));
    }
    else
    {
        Span end = tokens.empty() ? Span() : tokens.back().getSpan();
        errorReporter.report(Report(end, message, false));
    }
}

bool TokenStream::isAtEnd() const
{
    return position >= tokens.size() || tokens.at(position).getType() == END_OF_FILE;
}

bool TokenStream::match(TokenType type)
{
    if (isAtEnd())
        return false;

    if (peek().getType() == type)
    {
        advance();
        return true;
    }
    return false;
}

bool TokenStream::check(TokenType type) const
{
    if (isAtEnd())
        return false;
    return peek().getType() == type;
}

const Token TokenStream::advance()
{
    Token token = peek();
    position++;
    return token;
}

const Token TokenStream::prev()
{
    if (position == 0)
    {
        return tokens.front();
    }
    return tokens.at(position - 1);
}

void TokenStream::throwError(const std::string &message)
{
    reportError(message);
    throw ParseError();
}

Token TokenStream::consume(TokenType type, const std::string &errorMessage)
{
    if (check(type))
        return advance();

    reportError(errorMessage);
    throw ParseError();
}

Token TokenStream::consume(TokenType type)
{
    if (check(type))
        return advance();

    reportError("Expected token of type " + tokenTypeString(type) + " but found " + tokenTypeString(peek().getType()));
    throw ParseError();
}

Token TokenStream::consumeDirective()
{
    TokenType type = peek().getType();
    if (directives.find(type) != directives.end())
        return advance();

    std::cout << "directive value: " << peek().getValue() << std::endl;
    reportError("Expected directive name but found " + tokenTypeString(type));
    throw ParseError();
}

Token TokenStream::consumeValue()
{
    TokenType type = peek().getType();
    if (isValue(type))
        return advance();
    reportError("Expected directive value but found " + tokenTypeString(type));
    throw ParseError();
}

bool TokenStream::isValue(TokenType type) const
{
    return directives.find(type) != directives.end() || type == IDENTIFIER || type == LOCATION || type == SERVER;
}

bool TokenStream::isDirective(TokenType type) const
{
    return directives.find(type) != directives.end();
}

bool TokenStream::hasNext() const
{
    return !isAtEnd();
}

std::string TokenStream::tokenTypeString(TokenType type)
{
    switch (type)
    {
    case UNKNOWN:
        return "UNKNOWN";
    case IDENTIFIER:
        return "IDENTIFIER";
    case LEFT_BRACE:
        return "LEFT_BRACE";
    case RIGHT_BRACE:
        return "RIGHT_BRACE";
    case SEMICOLON:
        return "SEMICOLON";
    case LOCATION:
        return "LOCATION";
    case SERVER:
        return "SERVER";
    default:
        return "UNKNOWN";
    }
}

void TokenStream::synchronize()
{
    advance();
    while (!isAtEnd())
    {
        if (prev().getType() == SEMICOLON)
            return;
        TokenType type = peek().getType();
        // if (directives.find(type) != directives.end())
        //     return;
        switch (type)
        {
        case SERVER:
        case LOCATION:
        case RIGHT_BRACE:
            return;
            break;

        default:
            advance();
        }
    }
}

void TokenStream::synchronizeServer()
{
    advance();
    while (!isAtEnd())
    {
        if (peek().getType() == SERVER)
            return;
        advance();
    }
}