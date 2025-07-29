#include "predefineSymbol.h"

namespace PredefineSymbol
{
    const GrammarSymbol SYMBOL_END = GrammarSymbol("$", SymbolType::End);
    const GrammarSymbol SYMBOL_PLUS = GrammarSymbol("+", SymbolType::Terminal);
    const GrammarSymbol SYMBOL_IDENTIFIER = GrammarSymbol("identifier", SymbolType::Terminal);
    const GrammarSymbol SYMBOL_NUMBER = GrammarSymbol("number", SymbolType::Terminal);
    const GrammarSymbol SYMBOL_ASSIGN = GrammarSymbol("=", SymbolType::Terminal);
}
