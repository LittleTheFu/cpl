#include "regExParser.h"
#include <stdexcept>

RegExParser::RegExParser(const std::string &regExStr)
{
    regExStr_ = regExStr;
    index_ = 0;
}

void RegExParser::parseRegEx()
{
    if (isEnd())
    {
        throw std::runtime_error("invalid regex");
    }

    parseTerm();

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

            parseTerm();
        }
        else
        {
            break;
        }
    }
}

void RegExParser::parseTerm()
{
    if (isEnd())
    {
        throw std::runtime_error("invalid regex");
    }

    parseFactor();

    while (true)
    {
        char c = peekChar();

        if (isInFactorFirstSet(c))
        {
            parseFactor();
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
}

void RegExParser::parseFactor()
{
    if (isEnd())
    {
        throw std::runtime_error("invalid regex");
    }

    parseAtom();

    char c = peekChar();

    if (c == '*' || c == '+' || c == '?')
    {
        // TODO :
        consumeChar();
    }
}

void RegExParser::parseAtom()
{
    if (isEnd())
    {
        throw std::runtime_error("invalid regex");
    }

    char c = peekChar();

    if (c == '(')
    {
        consumeChar();
        parseRegEx();

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
    }
    else
    {
        parseChar();
    }
}

void RegExParser::parseChar()
{
    if (isEnd())
    {
        return;
    }

    char c = peekChar();

    if(isLetter(c) || isDigit(c) || isUnderscore(c))
    {
        consumeChar();
    }
    else
    {
        throw std::runtime_error("invalid regex");
    }
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

bool RegExParser::isInFactorFirstSet(char c)
{
    return isLetter(c) || isDigit(c) || isUnderscore(c) || (c == '(');
}

bool RegExParser::isInFactorFollowSet(char c)
{
    return c == ')' || c == '|' || c == '\0';
}
