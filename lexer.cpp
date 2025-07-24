#include "lexer.h"

Lexer::Lexer(const std::string &input)
{
    input_ = input;
    currentPos_ = 0;

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
            return std::make_shared<Token>(TokenType::EOF_TOKEN, "");
        }

        std::pair<std::optional<size_t>, std::shared_ptr<Token>> longestResult;

        matchRE(regExNumber_, TokenType::INTEGER, longestResult);
        matchRE(regExIdentifier_, TokenType::IDENTIFIER, longestResult);
        matchRE(regExGreater_, TokenType::GREATER, longestResult);
        matchRE(regExLess_, TokenType::LESS, longestResult);
        matchRE(regExGreaterEqual_, TokenType::GREATER_EQUAL, longestResult);
        matchRE(regExLessEqual_, TokenType::LESS_EQUAL, longestResult);
        matchRE(regExEqual_, TokenType::EQUAL, longestResult);
        matchRE(regExNotEqual_, TokenType::NOT_EQUAL, longestResult);
        matchRE(regExPlus_, TokenType::PLUS, longestResult);
        matchRE(regExMinus_, TokenType::MINUS, longestResult);
        matchRE(regExMultiply_, TokenType::MULTIPLY, longestResult);
        matchRE(regExDivide_, TokenType::DIVIDE, longestResult);
        matchRE(regExEqual_, TokenType::EQUAL, longestResult);
        matchRE(regExNotEqual_, TokenType::NOT_EQUAL, longestResult);

        if(longestResult.first.has_value())
        {
            currentPos_ += longestResult.first.value();
            return longestResult.second;
        }
    }

    return std::make_shared<Token>(TokenType::EOF_TOKEN, "");
}

void Lexer::matchRE(const std::unique_ptr<RegEx> &regEx,
                    TokenType tokenType,
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
