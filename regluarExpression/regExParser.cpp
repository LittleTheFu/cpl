#include "regExParser.h"

RegExParser::RegExParser(const std::string &regExStr)
{
    regExStr_ = regExStr;
    index_ = 0;
}

void RegExParser::parseRegEx()
{
    parseTerm();

    std::pair<bool, char> c = nextChar();

    if(c.first && c.second == '|')
    {
        parseChar();
    }
}

void RegExParser::parseTerm()
{
    parseFactor();

    std::pair<bool, char> c = nextChar();

    if(c.first && isInFactorFirstSet(c.second))
    {
        parseFactor();
    }
}

void RegExParser::parseFactor()
{
    parseAtom();

    std::pair<bool, char> c = nextChar();

    if(c.first && (c.second == '*' || c.second == '+' || c.second == '?'))
    {
        //TODO : 
    }
}

void RegExParser::parseAtom()
{
    parseChar();
}

void RegExParser::parseChar()
{
    if (index_ >= regExStr_.size())
    {
        return;
    }
}

std::pair<bool, char> RegExParser::nextChar()
{
    if (index_ >= regExStr_.size())
    {
        return {false, '\0'};
    }
    
    return {true, regExStr_.at(index_)};
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
