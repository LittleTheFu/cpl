#ifndef _PARSER_H_
#define _PARSER_H_

#include "grammar.h"

class Parser
{
public:
    explicit Parser(const Grammar &grammar);
    ~Parser() = default;

    bool parse(const std::vector<GrammarSymbol> &symbols);

};

#endif // _PARSER_H_