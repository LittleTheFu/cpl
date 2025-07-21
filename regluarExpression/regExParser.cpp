#include "regExParser.h"

RegExParser::RegExParser(const std::string &regExStr)
{
    regExStr_ = regExStr;
    index_ = 0;
}

void RegExParser::parseRegEx()
{
    // std::pair<bool, char> ch = nextChar();
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
}

void RegExParser::parseFactor()
{
    parseAtom();
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
    
    index_++;

    return {true, regExStr_[index_ - 1]};
}