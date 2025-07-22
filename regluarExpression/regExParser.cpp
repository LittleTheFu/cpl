#include "regExParser.h"
#include <stdexcept>
#include "regExCharNode.h"
#include "regExKleeneStarNode.h"
#include "regExConcatenationNode.h"
#include "regExAlternationNode.h"
#include "regExPlusNode.h"
#include "regExOptionalNode.h"

RegExParser::RegExParser(const std::string &regExStr)
{
    regExStr_ = regExStr;
    index_ = 0;

    root_ = parseRegEx();

    if (!isEnd())
    {
        throw std::runtime_error("invalid regex");
    }
}

std::shared_ptr<RegExNode> RegExParser::parseRegEx()
{
    if (isEnd())
    {
        throw std::runtime_error("invalid regex");
    }

    auto node = parseTerm();

    while (true)
    {
        char c = peekChar();

        if (c == '|')
        {
            consumeChar();

            if(isEnd())
            {
                throw std::runtime_error("invalid regex");
            }

            node = std::make_shared<RegExAlternationNode>(node, parseTerm());
        }
        else
        {
            break;
        }
    }

    return node;
}

std::shared_ptr<RegExNode> RegExParser::parseTerm()
{
    if (isEnd())
    {
        throw std::runtime_error("invalid regex");
    }

    auto node = parseFactor();

    while (true)
    {
        char c = peekChar();

        if (isInFactorFirstSet(c))
        {
            node = std::make_shared<RegExConcatenationNode>(node, parseFactor());
        }
        else if(isInFactorFollowSet(c))
        {
            break;
        }
        else
        {
            throw std::runtime_error("invalid regex");
        }
    }

    return node;
}

std::shared_ptr<RegExNode> RegExParser::parseFactor()
{
    if (isEnd())
    {
        throw std::runtime_error("invalid regex");
    }

    auto node = parseAtom();

    char c = peekChar();
    if (c == '*')
    {
        consumeChar();
        node = std::make_shared<RegExKleeneStarNode>(node);
    }
    else if(c == '+')
    {
        consumeChar();
        node = std::make_shared<RegExPlusNode>(node);
    }
    else if(c == '?')
    {
        consumeChar();
        node = std::make_shared<RegExOptionalNode>(node);
    }

    return node;
}

std::shared_ptr<RegExNode> RegExParser::parseAtom()
{
    if (isEnd())
    {
        throw std::runtime_error("invalid regex");
    }

    char c = peekChar();

    if (c == '(')
    {
        consumeChar();
        auto node = parseRegEx();

        if (isEnd())
        {
            throw std::runtime_error("invalid regex");
        }

        char cc = peekChar();
        if (cc != ')')
        {
            throw std::runtime_error("invalid regex");
        }

        consumeChar();
        return node;
    }
    else
    {
        return parseChar();
    }

    return nullptr;
}

std::shared_ptr<RegExNode> RegExParser::parseChar()
{
    if (isEnd())
    {
        throw std::runtime_error("invalid regex");
    }

    char c = peekChar();

    if(isLetter(c) || isDigit(c) || isUnderscore(c) || isWhiteSpace(c))
    {
        consumeChar();
        return std::make_shared<RegExCharNode>(c);
    }
    else
    {
        throw std::runtime_error("invalid regex");
    }

    return nullptr;
}

char RegExParser::peekChar()
{
    if (isEnd())
    {
        return '\0';
    }

    return regExStr_.at(index_);
}

void RegExParser::consumeChar()
{
    index_++;
}

bool RegExParser::isEnd()
{
    return index_ >= regExStr_.size();
}

bool RegExParser::isLetter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool RegExParser::isDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool RegExParser::isUnderscore(char c)
{
    return c == '_';
}

bool RegExParser::isWhiteSpace(char c)
{
    return c == ' ' ||
           c == '\t' ||
           c == '\n' ||
           c == '\r' ||
           c == '\f' ||
           c == '\v';
}

bool RegExParser::isInFactorFirstSet(char c)
{
    return isLetter(c) || isDigit(c) || isUnderscore(c) || (c == '(');
}

bool RegExParser::isInFactorFollowSet(char c)
{
    return c == ')' || c == '|' || c == '\0';
}
