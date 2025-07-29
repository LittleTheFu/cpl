#include "predefineSymbol.h"

namespace PredefineSymbol
{
    const GrammarSymbol SYMBOL_END = GrammarSymbol("$", SymbolType::End);
    const GrammarSymbol SYMBOL_PLUS = GrammarSymbol("+", SymbolType::Terminal);
    const GrammarSymbol SYMBOL_MINUS = GrammarSymbol("-", SymbolType::Terminal);
    const GrammarSymbol SYMBOL_MULTIPLY = GrammarSymbol("*", SymbolType::Terminal);
    const GrammarSymbol SYMBOL_DIVIDE = GrammarSymbol("/", SymbolType::Terminal);
    const GrammarSymbol SYMBOL_IDENTIFIER = GrammarSymbol("id", SymbolType::Terminal);
    const GrammarSymbol SYMBOL_NUMBER = GrammarSymbol("number", SymbolType::Terminal);
    const GrammarSymbol SYMBOL_ASSIGN = GrammarSymbol("=", SymbolType::Terminal);
    const GrammarSymbol SYMBOL_STACK_BOTTOM = GrammarSymbol("stack bottom", SymbolType::StackBottom);
    const GrammarSymbol LEFT_PAREN = GrammarSymbol("(", SymbolType::Terminal);
    const GrammarSymbol RIGHT_PAREN = GrammarSymbol(")", SymbolType::Terminal);
}
