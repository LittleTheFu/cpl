#ifndef _LEXER_H_
#define _LEXER_H_

#include <string>
#include <memory>
#include <map>
#include "token.h"
#include "regEx.h"


class Lexer
{
public:
    Lexer();
    Lexer(const std::string &input);
    ~Lexer() = default;

    void setInput(const std::string &input);
    std::shared_ptr<Token> getNextToken();

private:
    void initRE();
    void matchRE(const std::unique_ptr<RegEx> &regEx,
                 _TokenType_ tokenType,
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

    std::unique_ptr<RegEx> regExPlus_;
    std::unique_ptr<RegEx> regExMinus_;
    std::unique_ptr<RegEx> regExMultiply_;
    std::unique_ptr<RegEx> regExDivide_;
    
    std::unique_ptr<RegEx> regExEqual_;
    std::unique_ptr<RegEx> regExNotEqual_;

    std::unique_ptr<RegEx> regExLParen_;
    std::unique_ptr<RegEx> regExRParen_;
    std::unique_ptr<RegEx> regExLBrace_;
    std::unique_ptr<RegEx> regExRBrace_;
    std::unique_ptr<RegEx> regExSemicolon_;
    std::unique_ptr<RegEx> regExComma_;

private:
    std::map<std::string, _TokenType_> keywords_;
};

#endif // _LEXER_H_