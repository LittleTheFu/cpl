#include "parser.h"
#include "predefineSymbol.h"
#include <stdexcept>
#include <stack>
#include <stackItem.h>
#include <iostream>

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

    const std::map<LRState, int>& dfa = parserGenerator_.getDFA();
    const std::map<int, std::map<GrammarSymbol, int>>& gotoTable_ = parserGenerator_.getGotoTable();
    const std::map<int, std::map<GrammarSymbol, Action>>& actionTable_ = parserGenerator_.getActionTable();
    LRState startState = parserGenerator_.getStartState();
    GrammarSymbol startSymbol = grammar_.getArgumentedStartSymbol();

    std::stack<StackItem> stack;
    if(dfa.find(startState) == dfa.end())
    {
        throw std::runtime_error("no state in table");
    }
    stack.push({dfa.at(startState), PredefineSymbol::SYMBOL_STACK_BOTTOM});

    size_t inputIndex = 0;

    const GrammarSymbol *current_lookahead_symbol = nullptr;
    if (symbols_.empty())
    {
        throw std::runtime_error("input index out of range");
    }
    current_lookahead_symbol = &symbols_.at(inputIndex);
    while (true)
    {
        int stateId = stack.top().stateId;
        if(actionTable_.find(stateId) == actionTable_.end())
        {
            throw std::runtime_error("no state in table");
        }
        if (actionTable_.at(stateId).find(*current_lookahead_symbol) == actionTable_.at(stateId).end())
        {
            throw std::runtime_error("no action in table");
        }
        else
        {
            Action action = actionTable_.at(stateId).at(*current_lookahead_symbol);
            if (action.type == ActionType::Shift)
            {
                stack.push({action.shiftStateId, *current_lookahead_symbol, tokens.at(inputIndex)});
                inputIndex++;

                if (inputIndex < symbols_.size()) {
                    current_lookahead_symbol = &symbols_.at(inputIndex);
                }
            }
            else if (action.type == ActionType::Reduce)
            {
                const ProductionRule& rule = grammar_.getRuleById(action.productionRuleId);

                std::vector<StackItem> stackItems;
                for(size_t i = 0; i < rule.getRightSize(); i++)
                {
                    stackItems.push_back(std::move(stack.top()));
                    stack.pop();
                }
                std::reverse(stackItems.begin(), stackItems.end());

                int oldState = stack.top().stateId;
                if(gotoTable_.find(oldState) == gotoTable_.end())
                {
                    throw std::runtime_error("no goto in table");
                }
                if(gotoTable_.at(oldState).find(rule.getLeft()) == gotoTable_.at(oldState).end())
                {
                    throw std::runtime_error("no goto in table");
                }
                std::unique_ptr<AstNode> node = rule.applySemanticAction(std::move(stackItems));
                int nextState = gotoTable_.at(oldState).at(rule.getLeft());
                stack.push({nextState, rule.getLeft(), std::move(node)});
            }
            else if (action.type == ActionType::Accept)
            {
                if(stack.size() != 2)
                {
                    throw std::runtime_error("stack size not 2");
                }
                std::get<std::unique_ptr<AstNode>>(stack.top().value)->evaluate();
                std::get<std::unique_ptr<AstNode>>(stack.top().value)->genIRCode(program_);
                std::cout << std::endl;

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
    else if (token.type == TokenType::MINUS)
    {
        return PredefineSymbol::SYMBOL_MINUS;
    }
    else if (token.type == TokenType::MULTIPLY)
    {
        return PredefineSymbol::SYMBOL_MULTIPLY;
    }
    else if (token.type == TokenType::DIVIDE)
    {
        return PredefineSymbol::SYMBOL_DIVIDE;
    }
    else if (token.type == TokenType::ASSIGN)
    {
        return PredefineSymbol::SYMBOL_ASSIGN;
    }
    else if (token.type == TokenType::EOF_TOKEN)
    {
        return PredefineSymbol::SYMBOL_END;
    }
    else if (token.type == TokenType::L_PAREN)
    {
        return PredefineSymbol::LEFT_PAREN;
    }
    else if (token.type == TokenType::R_PAREN)
    {
        return PredefineSymbol::RIGHT_PAREN;
    }
    else
    {
        throw std::runtime_error("Unknown token type");
    }
}
