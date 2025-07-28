#ifndef __LR_PARSER_GENERATOR_H__
#define __LR_PARSER_GENERATOR_H__

#include "grammar.h"
#include "lrItem.h"
#include <set>

class LRParserGenerator
{
public:
    LRParserGenerator(const Grammar& grammar);
    ~LRParserGenerator() = default;

    std::set<LRItem> caculateClosure(const std::set<LRItem>& item);

private:
    const Grammar& grammar_;
};



#endif // __LR_PARSER_GENERATOR_H__