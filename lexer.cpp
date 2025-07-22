#include "lexer.h"

Lexer::Lexer(const std::string &input)
{
    input_ = input;
    currentPos_ = 0;

    regExWhite_ = std::make_unique<RegEx>("( |\t|\n|\r)+");
    regExNumber_ = std::make_unique<RegEx>("(0|1|2|3|4|5|6|7|8|9)+");
    regExIdentifier_ = std::make_unique<RegEx>("(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|0|1|2|3|4|5|6|7|8|9|_)+");
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

        // number
        std::optional<size_t> numberLen = regExNumber_->match(input_.substr(currentPos_));
        if (numberLen.has_value())
        {
            std::string numberStr = input_.substr(currentPos_, numberLen.value());
            currentPos_ += numberLen.value();
            return std::make_shared<Token>(TokenType::INTEGER, numberStr);
        }

        // identifier
        std::optional<size_t> identifierLen = regExIdentifier_->match(input_.substr(currentPos_));
        if (identifierLen.has_value())
        {
            std::string identifierStr = input_.substr(currentPos_, identifierLen.value());
            currentPos_ += identifierLen.value();
            return std::make_shared<Token>(TokenType::IDENTIFIER, identifierStr);
        }
    }

    return std::make_shared<Token>(TokenType::EOF_TOKEN, "");
}
