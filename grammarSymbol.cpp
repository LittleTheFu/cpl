#include "grammarSymbol.h"

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
