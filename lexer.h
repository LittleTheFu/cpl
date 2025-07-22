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
    void matchRE(const std::unique_ptr<RegEx> &regEx,
                 TokenType tokenType,
                 std::pair<std::optional<size_t>, std::shared_ptr<Token>> &result);

private:
    std::string input_;
    size_t currentPos_;

private:
    std::unique_ptr<RegEx> regExWhite_;
    std::unique_ptr<RegEx> regExNumber_;
    std::unique_ptr<RegEx> regExIdentifier_;

    std::unique_ptr<RegEx> regExGreater_;
    std::unique_ptr<RegEx> regExLess_;
    std::unique_ptr<RegEx> regExGreaterEqual_;
    std::unique_ptr<RegEx> regExLessEqual_;
    std::unique_ptr<RegEx> regExEqual_;
    std::unique_ptr<RegEx> regExNotEqual_;

};

#endif // _LEXER_H_