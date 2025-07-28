#ifndef __LR_PARSER_GENERATOR_H__
#define __LR_PARSER_GENERATOR_H__

#include "grammar.h"
#include "lrItem.h"
#include "grammarSymbol.h"
#include <set>
#include "lrState.h"

class LRParserGenerator
{
public:
    LRParserGenerator(const Grammar& grammar);
    ~LRParserGenerator() = default;

    std::set<LRItem> caculateClosure(const std::set<LRItem>& item);
    std::set<LRItem> calculateGoto(const LRState &state, const GrammarSymbol &inputSymbol);

private:
    const Grammar& grammar_;
};



#endif // __LR_PARSER_GENERATOR_H__