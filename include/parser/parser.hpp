#ifndef PARSER_HPP
#define PARSER_HPP
#include <string>
#include <vector>

#include "tokenizer/Token.hpp"
#include "parser/ast/Config.hpp"
#include "parser/ast/ServerConfig.hpp"
#include "parser/ast/LocationConfig.hpp"
#include "parser/ast/Directive.hpp"
#include "parser/TokenStream.hpp"


class Parser {
public:
    Parser(const std::vector<Token>& tokens, ErrorReporter &errorReporter);
    Config parse();
private:
    TokenStream tokenStream;
    Config parseConfig();
    ServerConfig parseServerConfig();
    LocationConfig parseLocationConfig();
    Directive parseDirective();
};

#endif
