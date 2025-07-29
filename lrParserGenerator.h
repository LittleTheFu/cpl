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
    Action()
    {
        type = ActionType::Error;
    }

    Action(ActionType type, int shiftStateId, size_t productionRuleId)
    {
        this->type = type;
        this->shiftStateId = shiftStateId;
        this->productionRuleId = productionRuleId;
    }

    ActionType type;
    int shiftStateId;
    size_t productionRuleId;
};

class LRParserGenerator
{
public:
    LRParserGenerator(const Grammar& grammar);
    ~LRParserGenerator() = default;

    const std::map<int, std::map<GrammarSymbol, int>>& getGotoTable() const;
    const std::map<int, std::map<GrammarSymbol, Action>>& getActionTable() const;
    const std::map<LRState, int>& getDFA() const;

private:
    std::set<LRItem> calculateClosure(const std::set<LRItem>& item);
    LRState calculateNextState(const LRState &state, const GrammarSymbol &inputSymbol);

    void buildDFA();


    int getNextId();
    int idCnt_;


    // std::optional<LRState> getState(int id) const;
private:
    const Grammar& grammar_;
    std::map<LRState, int> dfa_;

    std::map<int, std::map<GrammarSymbol, int>> gotoTable_;
    std::map<int, std::map<GrammarSymbol, Action>> actionTable_;
};



#endif // __LR_PARSER_GENERATOR_H__