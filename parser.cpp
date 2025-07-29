#include "parser.h"
#include "predefineSymbol.h"
#include <stdexcept>
#include <stack>

Parser::Parser(const Grammar &grammar)
    : grammar_(grammar),
      parserGenerator_(grammar)
{
}

bool Parser::parse(const std::vector<Token> &tokens)
{
    symbols_.reserve(tokens.size());
    for (const auto &token : tokens)
    {
        symbols_.push_back(map(token));
    }

    std::map<LRState, int> dfa = parserGenerator_.getDFA();
    std::map<int, std::map<GrammarSymbol, int>> gotoTable_ = parserGenerator_.getGotoTable();
    std::map<int, std::map<GrammarSymbol, Action>> actionTable_ = parserGenerator_.getActionTable();
    LRState startState = parserGenerator_.getStartState();
    GrammarSymbol startSymbol = grammar_.getArgumentedStartSymbol();

    std::stack<std::pair<int, GrammarSymbol>> stack;
    stack.push({dfa[startState], PredefineSymbol::SYMBOL_STACK_BOTTOM});

    size_t inputIndex = 0;
    while (true)
    {
        if(inputIndex >= symbols_.size())
        {
            throw std::runtime_error("input index out of range");
        }

        const GrammarSymbol &symbol = symbols_.at(inputIndex);
        int stateId = stack.top().first;
        if (actionTable_[stateId].find(symbol) == actionTable_[stateId].end())
        {
            throw std::runtime_error("no action in table");
        }
        else
        {
            Action action = actionTable_[stateId][symbol];
            if (action.type == ActionType::Shift)
            {
                stack.push({action.shiftStateId, symbol});
                inputIndex++;
            }
            else if (action.type == ActionType::Reduce)
            {
                const ProductionRule& rule = grammar_.getRuleById(action.productionRuleId);
                for(size_t i = 0; i < rule.getRightSize(); i++)
                {
                    stack.pop();
                }
                int oldState = stack.top().first;
                int nextState = gotoTable_[oldState][rule.getLeft()];
                stack.push({nextState, rule.getLeft()});
            }
            else if (action.type == ActionType::Accept)
            {
                return true;
            }
        }
    }

    throw std::runtime_error("parse error");
}

GrammarSymbol Parser::map(const Token &token) const
{
    if (token.type == TokenType::IDENTIFIER)
    {
        return PredefineSymbol::SYMBOL_IDENTIFIER;
    }
    else if (token.type == TokenType::INTEGER)
    {
        return PredefineSymbol::SYMBOL_NUMBER;
    }
    else if (token.type == TokenType::PLUS)
    {
        return PredefineSymbol::SYMBOL_PLUS;
    }
    else if (token.type == TokenType::ASSIGN)
    {
        return PredefineSymbol::SYMBOL_ASSIGN;
    }
    else if (token.type == TokenType::EOF_TOKEN)
    {
        return PredefineSymbol::SYMBOL_END;
    }
    else
    {
        throw std::runtime_error("Unknown token type");
    }
}
