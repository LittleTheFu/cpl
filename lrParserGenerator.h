#ifndef __LR_PARSER_GENERATOR_H__
#define __LR_PARSER_GENERATOR_H__

#include "grammar.h"
#include "grammarSymbol.h"
#include "lrItem.h"
#include "lrState.h"
#include <set>
#include <map>

class LRParserGenerator
{
public:
    LRParserGenerator(const Grammar& grammar);
    ~LRParserGenerator() = default;

private:
    std::set<LRItem> caculateClosure(const std::set<LRItem>& item);
    LRState calculateGoto(const LRState &state, const GrammarSymbol &inputSymbol);

    void buildDFA();
    static int getNextId();

private:
    const Grammar& grammar_;
    std::map<LRState, int> dfa_;
};



#endif // __LR_PARSER_GENERATOR_H__