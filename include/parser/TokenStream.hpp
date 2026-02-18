#ifndef TOKEN_STREAM_HPP
#define TOKEN_STREAM_HPP

#include <vector>
#include <set>
#include "tokenizer/Token.hpp"
#include "utils/ErrorReporter.hpp"

class TokenStream
{
public:
    TokenStream(const std::vector<Token> &tokens, ErrorReporter &errorReporter);
    const Token &peek() const;
    const Token advance();
    bool match(TokenType type);
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string &errorMessage);
    Token consume(TokenType type);
    Token consumeDirective();
    Token consumeValue();
    bool isAtEnd() const;
    bool hasNext() const;
    void throwError(const std::string &message);
    void reportError(const std::string &message);

    const Token prev();
    // syn methods
    void synchronize();
    void synchronizeServer();

private:
    std::vector<Token> tokens;
    std::set<TokenType> directives;
    ErrorReporter &errorReporter;
    size_t position;
    static std::string tokenTypeString(TokenType type);
    bool isValue(TokenType type) const;
    bool isDirective(TokenType type) const;
};

#endif