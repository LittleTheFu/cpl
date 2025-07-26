#ifndef _GRAMMAR_SYMBOL_H_
#define _GRAMMAR_SYMBOL_H_

#include <string>

enum class SymbolType
{
    NonTerminal,
    Terminal,
    Epsilon,
};

class GrammarSymbol
{
public:
    explicit GrammarSymbol(const std::string& name, SymbolType type);
    ~GrammarSymbol() = default;

    static GrammarSymbol getEpsilonSymbol();

    bool operator==(const GrammarSymbol& other) const;
    bool operator!=(const GrammarSymbol& other) const;

    std::string getName() const;
    SymbolType getType() const;

private:
    std::string name_;
    SymbolType type_;
};

#endif // _GRAMMAR_SYMBOL_H_