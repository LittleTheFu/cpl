#include "grammarSymbol.h"

GrammarSymbol::GrammarSymbol()
{
    name_ = "";
    type_ = SymbolType::Epsilon;
}

GrammarSymbol::GrammarSymbol(const std::string &name, SymbolType type)
{
    name_ = name;
    type_ = type;
}

GrammarSymbol GrammarSymbol::getEpsilonSymbol()
{
    static GrammarSymbol epsilon("ε", SymbolType::Epsilon);
    return epsilon;
}

GrammarSymbol GrammarSymbol::getEndSymbol()
{
    static GrammarSymbol end("$", SymbolType::End);
    return end;
}

bool GrammarSymbol::operator==(const GrammarSymbol &other) const
{
    return name_ == other.name_ && type_ == other.type_;
}

bool GrammarSymbol::operator!=(const GrammarSymbol &other) const
{
    return !(*this == other);
}

bool GrammarSymbol::operator<(const GrammarSymbol &other) const
{
    if (name_ != other.name_)
    {
        return name_ < other.name_;
    }

    return type_ < other.type_;
}

std::string GrammarSymbol::getName() const
{
    return name_;
}

SymbolType GrammarSymbol::getType() const
{
    return type_;
}

std::string GrammarSymbol::toString() const
{
    return name_ + "(" + symbolTypeToString(type_) + ")";
}

std::string symbolTypeToString(SymbolType type)
{
    switch (type)
    {
    case SymbolType::NonTerminal:
        return "NonTerminal";
    case SymbolType::Terminal:
        return "Terminal";
    case SymbolType::Epsilon:
        return "Epsilon";
    case SymbolType::End:
        return "End";
    default:
        return "Unknown";
    }
}
