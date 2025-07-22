#ifndef _LEXER_H_
#define _LEXER_H_

#include <string>
#include <memory>
#include "token.h"

class Lexer
{
public:
    Lexer(const std::string &input);
    ~Lexer() = default;

    std::shared_ptr<Token> getNextToken();

private:

private:
    std::string input_;
    std::size_t index_;
};

#endif // _LEXER_H_