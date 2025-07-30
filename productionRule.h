#ifndef _PRODUCTION_RULE_H_
#define _PRODUCTION_RULE_H_

#include "grammarSymbol.h"
#include <vector>
#include <functional>
#include "astNode.h"
#include <memory>

struct StackItem;

using SemanticActionFn = std::function<std::unique_ptr<AstNode>(std::vector<StackItem>&&)>;

class ProductionRule
{
public:
    explicit ProductionRule(const GrammarSymbol& left, 
        const std::vector<GrammarSymbol>& right,
        SemanticActionFn semanticAction);
    ~ProductionRule() = default;

    std::string toString() const;

    GrammarSymbol getLeft() const;
    const std::vector<GrammarSymbol>& getRight() const;
    size_t getRightSize() const;

    bool operator==(const ProductionRule& other) const;
    bool operator!=(const ProductionRule& other) const;
    bool operator<(const ProductionRule& other) const;

    std::unique_ptr<AstNode> applySemanticAction(std::vector<StackItem>&& stackItems) const;

private:
    GrammarSymbol left_;
    std::vector<GrammarSymbol> right_;
    SemanticActionFn semanticAction_;
};

#endif // _PRODUCTION_RULE_H_