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
    std::string input_;
    size_t currentPos_;
};

#endif // _LEXER_H_