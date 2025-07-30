#ifndef _PARSER_H_
#define _PARSER_H_

#include "grammar.h"
#include "token.h"
#include "lrParserGenerator.h"
#include <variant>
#include <memory>
#include "astNode.h"

struct StackItem
{
    int stateId;
    GrammarSymbol symbol;

    std::variant<std::unique_ptr<AstNode>, Token> value;

    StackItem(int stateId, GrammarSymbol symbol, Token token)
        : stateId(stateId), symbol(symbol), value(token)
    {
    }

    StackItem(int stateId, GrammarSymbol symbol, std::unique_ptr<AstNode>&& node)
        : stateId(stateId), symbol(symbol), value(std::move(node))
    {
    }
};

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