#include "lexer.h"

Lexer::Lexer(const std::string &input)
    : input_(input), index_(0)
{
}

std::shared_ptr<Token> Lexer::getNextToken()
{
    
}
