#ifndef _LEXER_H_
#define _LEXER_H_

#include <string>
#include <memory>
#include "token.h"
#include "regEx.h"


class Lexer
{
public:
    Lexer(const std::string &input);
    ~Lexer() = default;

    std::shared_ptr<Token> getNextToken();

private:
    std::string input_;
    size_t currentPos_;

private:
    std::unique_ptr<RegEx> regExWhite_;
    std::unique_ptr<RegEx> regExNumber_;
    std::unique_ptr<RegEx> regExIdentifier_;

};

#endif // _LEXER_H_