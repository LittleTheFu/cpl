#include "lexer.h"
#include <stdexcept>

Lexer::Lexer()
{
    initRE();

    input_ = "";
    currentPos_ = 0;
}

Lexer::Lexer(const std::string &input)
{
    initRE();

    input_ = input;
    currentPos_ = 0;
}

void Lexer::setInput(const std::string &input)
{
    input_ = input;
    currentPos_ = 0;
}

std::shared_ptr<Token> Lexer::getNextToken()
{
    std::optional<size_t> matchedNum = 0;

    while (currentPos_ < input_.size())
    {
        bool skippedSomething = true;
        while (skippedSomething)
        {
            skippedSomething = false;

            std::optional<size_t> whitespaceLen = regExWhite_->match(input_.substr(currentPos_));
            if (whitespaceLen.has_value())
            {
                currentPos_ += whitespaceLen.value();
                skippedSomething = true;
                continue;
            }
        }

        if(currentPos_ >= input_.size())
        {
            return std::make_shared<Token>(_TokenType_::EOF_TOKEN, "");
        }

        std::pair<std::optional<size_t>, std::shared_ptr<Token>> longestResult;

        matchRE(regExNumber_, _TokenType_::INTEGER, longestResult);
        matchRE(regExIdentifier_, _TokenType_::IDENTIFIER, longestResult);
        matchRE(regExGreater_, _TokenType_::GREATER, longestResult);
        matchRE(regExLess_, _TokenType_::LESS, longestResult);
        matchRE(regExGreaterEqual_, _TokenType_::GREATER_EQUAL, longestResult);
        matchRE(regExLessEqual_, _TokenType_::LESS_EQUAL, longestResult);
        matchRE(regExEqual_, _TokenType_::EQUAL, longestResult);
        matchRE(regExNotEqual_, _TokenType_::NOT_EQUAL, longestResult);
        matchRE(regExPlus_, _TokenType_::PLUS, longestResult);
        matchRE(regExMinus_, _TokenType_::MINUS, longestResult);
        matchRE(regExMultiply_, _TokenType_::MULTIPLY, longestResult);
        matchRE(regExDivide_, _TokenType_::DIVIDE, longestResult);
        matchRE(regExEqual_, _TokenType_::EQUAL, longestResult);
        matchRE(regExNotEqual_, _TokenType_::NOT_EQUAL, longestResult);
        matchRE(regExLParen_, _TokenType_::L_PAREN, longestResult);
        matchRE(regExRParen_, _TokenType_::R_PAREN, longestResult);
        matchRE(regExLBrace_, _TokenType_::L_BRACE, longestResult);
        matchRE(regExRBrace_, _TokenType_::R_BRACE, longestResult);
        matchRE(regExSemicolon_, _TokenType_::SEMICOLON, longestResult);
        matchRE(regExComma_, _TokenType_::COMMA, longestResult);

        if(longestResult.first.has_value())
        {
            currentPos_ += longestResult.first.value();

            if(keywords_.find(longestResult.second->lexeme) != keywords_.end())
            {
                longestResult.second->type = keywords_[longestResult.second->lexeme];
            }
            
            return longestResult.second;
        }
        else
        {
            throw std::runtime_error("Unknown token");
        }
    }

    return std::make_shared<Token>(_TokenType_::EOF_TOKEN, "");
}

void Lexer::initRE()
{
    keywords_["if"] = _TokenType_::IF;
    keywords_["else"] = _TokenType_::ELSE;
    keywords_["while"] = _TokenType_::WHILE;
    keywords_["int"] = _TokenType_::INT;
    keywords_["return"] = _TokenType_::RETURN;


    // regExWhite_ = std::make_unique<RegEx>("( |\t|\n|\r)+");
    // regExNumber_ = std::make_unique<RegEx>("(0|1|2|3|4|5|6|7|8|9)+");
    // regExIdentifier_ = std::make_unique<RegEx>("(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|0|1|2|3|4|5|6|7|8|9|_)+");

    regExWhite_ = std::make_unique<RegEx>("\\s+");
    regExNumber_ = std::make_unique<RegEx>("\\d+");
    regExIdentifier_ = std::make_unique<RegEx>("\\w+");

    regExGreater_ = std::make_unique<RegEx>("(>)");
    regExLess_ = std::make_unique<RegEx>("(<)");
    regExGreaterEqual_ = std::make_unique<RegEx>("(>=)");
    regExLessEqual_ = std::make_unique<RegEx>("(<=)");
    regExEqual_ = std::make_unique<RegEx>("(=)");
    regExNotEqual_ = std::make_unique<RegEx>("(!=)");

    regExPlus_ = std::make_unique<RegEx>("(\\+)");
    regExMinus_ = std::make_unique<RegEx>("(\\-)");
    regExMultiply_ = std::make_unique<RegEx>("(\\*)");
    regExDivide_ = std::make_unique<RegEx>("(/)");

    regExEqual_ = std::make_unique<RegEx>("(=)");
    regExNotEqual_ = std::make_unique<RegEx>("(!=)");

    regExLParen_ = std::make_unique<RegEx>("(\\()");
    regExRParen_ = std::make_unique<RegEx>("(\\))");
    regExLBrace_ = std::make_unique<RegEx>("({)");
    regExRBrace_ = std::make_unique<RegEx>("(})");
    regExSemicolon_ = std::make_unique<RegEx>("(;)");
    regExComma_ = std::make_unique<RegEx>("(,)");
}

void Lexer::matchRE(const std::unique_ptr<RegEx> &regEx,
                    _TokenType_ tokenType,
                    std::pair<std::optional<size_t>, std::shared_ptr<Token>> &result)
{
    std::optional<size_t> matchedNum = regEx->match(input_.substr(currentPos_));
    if (matchedNum.has_value())
    {
        if (!result.first.has_value() || matchedNum.value() > result.first.value())
        {
            std::string matchedStr = input_.substr(currentPos_, matchedNum.value());
            // currentPos_ += matchedNum.value();
            result = std::make_pair(matchedNum, std::make_shared<Token>(tokenType, matchedStr));
        }
    }
}
