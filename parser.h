#ifndef _PARSER_H_
#define _PARSER_H_

#include "grammar.h"
#include "token.h"
#include "lrParserGenerator.h"

class Parser
{
public:
    explicit Parser(const Grammar &grammar);
    ~Parser() = default;

    bool parse(const std::vector<Token> &tokens);

private:
    GrammarSymbol map(const Token &token) const;

private:
    const Grammar &grammar_;
    LRParserGenerator parserGenerator_;
    std::vector<GrammarSymbol> symbols_;
};

#endif // _PARSER_H_