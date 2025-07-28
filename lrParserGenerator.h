#ifndef __LR_PARSER_GENERATOR_H__
#define __LR_PARSER_GENERATOR_H__

#include "grammar.h"
#include "grammarSymbol.h"
#include "lrItem.h"
#include "lrState.h"
#include <set>
#include <map>
#include <optional>

enum class ActionType
{
    Shift,
    Reduce,
    Accept,
    Error,
};

struct Action
{
    ActionType type;
    int stateId;
    size_t productionRuleId;
};

class LRParserGenerator
{
public:
    LRParserGenerator(const Grammar& grammar);
    ~LRParserGenerator() = default;

private:
    std::set<LRItem> caculateClosure(const std::set<LRItem>& item);
    LRState calculateNextState(const LRState &state, const GrammarSymbol &inputSymbol);

    void buildDFA();
    static int getNextId();

    // std::optional<LRState> getState(int id) const;

private:
    const Grammar& grammar_;
    std::map<LRState, int> dfa_;

    std::map<int, std::map<GrammarSymbol, int>> gotoTable_;
    std::map<int, std::map<GrammarSymbol, Action>> actionTable_;
};



#endif // __LR_PARSER_GENERATOR_H__