#include "regExParser.h"
#include <stdexcept>
#include "regExCharNode.h"
#include "regExKleeneStarNode.h"
#include "regExConcatenationNode.h"
#include "regExAlternationNode.h"
#include "regExPlusNode.h"
#include "regExOptionalNode.h"
#include <alphaBet.h>
#include "regExCharSetNode.h"

const std::set<char> RegExParser::REGEX_META_CHARS = {'*', '+', '?', '|', '(', ')', '[', ']', '\\', '-'};

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

            if (isEnd())
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

    while (!isEnd() && isInFactorFirstSet(peekChar()))
    {
        node = std::make_shared<RegExConcatenationNode>(node, parseFactor());
    }

    if (!isEnd() && !isInTermFollowSet(peekChar()))
    {
        throw std::runtime_error("invalid regex");
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
    else if (c == '+')
    {
        consumeChar();
        node = std::make_shared<RegExPlusNode>(node);
    }
    else if (c == '?')
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
        return parseCharOrCharSet();
    }

    return nullptr;
}

std::shared_ptr<RegExNode> RegExParser::parseCharOrCharSet()
{
    char c = peekChar();

    if (c == '\\')
    {
        return parseEscapedChar();
    }
    else if (isLiteralChar(c))
    {
        return parseLiteralChar();
    }
    else
    {
        return parseCharSet();
    }

    throw std::runtime_error("invalid regex");

    return nullptr;
}

std::shared_ptr<RegExNode> RegExParser::parseLiteralChar()
{
    char c = peekChar();
    if (!isLiteralChar(c))
    {
        throw std::runtime_error("invalid regex");
    }

    consumeChar();
    return std::make_shared<RegExCharNode>(c);
}

std::shared_ptr<RegExNode> RegExParser::parseEscapedChar()
{
    char c = peekChar();
    if (c == '\\')
    {
        throw std::runtime_error("invalid regex");
    }

    consumeChar();
    if(isEnd())
    {
        throw std::runtime_error("invalid regex");
    }

    char cc = peekChar();
    if (!isEscapedLiteralChar(cc))
    {
        throw std::runtime_error("invalid regex");
    }

    if(isMetaChar(cc))
    {
        return std::make_shared<RegExCharNode>(cc);
    }
    else if( cc == 'n')
    {
        return std::make_shared<RegExCharNode>('\n');
    }
    else if( cc == 't')
    {
        return std::make_shared<RegExCharNode>('\t');
    }
    else if( cc == 'r')
    {
        return std::make_shared<RegExCharNode>('\r');
    }
    else if( cc == 's')
    {
        return std::make_shared<RegExCharSetNode>(AlphaBet::instance().getWhiteSpaceChar());
    }
    else if( cc == 'd')
    {
        return std::make_shared<RegExCharSetNode>(AlphaBet::instance().getDigitChar());
    }
    else if( cc == 'w')
    {
        return std::make_shared<RegExCharSetNode>(AlphaBet::instance().getAlphaChar());
    }
    else
    {
        throw std::runtime_error("invalid regex");
    }

    consumeChar();
    return std::make_shared<RegExCharNode>(c);
}

std::shared_ptr<RegExNode> RegExParser::parseCharSet()
{
    std::set<char> charSet;
    bool isNegated = false;
    char c = peekChar();
    if (c == '[')
    {
        consumeChar();
        if (peekChar() == '^')
        {
            isNegated = true;
            consumeChar();
        }

        std::shared_ptr<RegExNode> charSetItemNode = parseCharSetItem();

        while(!isEnd() && !isInCharSetItemFirstSet(peekChar()))
        {
            charSetItemNode = std::make_shared<RegExConcatenationNode>(charSetItemNode, parseCharSetItem());
        }

        if(!isInCharSetItemFollowSet(peekChar()))
        {
            throw std::runtime_error("invalid regex");
        }
        consumeChar();

        if(isEnd())
        {
            throw std::runtime_error("invalid regex");
        }

        char cc = peekChar();
        if(cc != ']')
        {
            throw std::runtime_error("invalid regex");
        }

        //?????
        return std::make_shared<RegExCharSetNode>(charSet, isNegated);
    }
   
    throw std::runtime_error("invalid regex");
    return nullptr;
}

std::shared_ptr<RegExNode> RegExParser::parseCharSetItem(std::set<char> &charSet)
{
}

// range ::= (digit_char | lowercase_letter | uppercase_letter) '-' (digit_char | lowercase_letter | uppercase_letter)
void RegExParser::parseRange(std::set<char> &charSet)
{
    char start = peekChar();
    if(!isAlphaNumber(start))
    {
        throw std::runtime_error("invalid regex");
    }

    consumeChar();
    if(peekChar() != '-')
    {
        throw std::runtime_error("invalid regex");
    }

    consumeChar();
    char end = peekChar();
    if(!isAlphaNumber(end))
    {
        throw std::runtime_error("invalid regex");
    }

    if( start > end )
    {
        throw std::runtime_error("invalid regex");
    }

    if( isdigit(start) && isdigit(end) )
    {
        for(char c = start; c <= end; c++)
        {
            charSet.insert(c);
        }
        return ;
    }

    if( isLetter(start) && isLetter(end) )
    {
        for(char c = start; c <= end; c++)
        {
            charSet.insert(c);
        }
        return ;
    }

    throw std::runtime_error("invalid regex");

    return ;
}

std::shared_ptr<RegExNode> RegExParser::parseChar()
{
    if (isEnd())
    {
        throw std::runtime_error("invalid regex");
    }

    char c = peekChar();

    if (isLetter(c) || isDigit(c) || isUnderscore(c) || isWhiteSpace(c))
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

bool RegExParser::isOperator(char c)
{
    return c == '<' || c == '>' || c == '=' || c == '!';
}

bool RegExParser::isMetaChar(char c)
{
    return REGEX_META_CHARS.find(c) != REGEX_META_CHARS.end();
}

bool RegExParser::isLiteralChar(char c)
{
    if (isMetaChar(c))
    {
        return false;
    }

    return !isMetaChar(c);
}

bool RegExParser::isEscapedLiteralChar(char c)
{
    if (isMetaChar(c))
    {
        return true;
    }

    return (c == 's' ||
            c == 'd' ||
            c == 'w' ||
            c == 't' ||
            c == 'n' ||
            c == 'r');
}

bool RegExParser::isAlphaNumber(char c)
{
    return isLetter(c) || isDigit(c);
}

bool RegExParser::isInFactorFirstSet(char c)
{
    if (c == '\\')
    {
        return true;
    }

    if (c == '[')
    {
        return true;
    }

    if (c == '(')
    {
        return true;
    }

    if (isLiteralChar(c))
    {
        return true;
    }

    return false;
}

bool RegExParser::isInTermFollowSet(char c)
{
    return c == '|' || c == '\0';
}

bool RegExParser::isInCharSetItemFirstSet(char c)
{
    return isLiteralChar(c) || c == '\\';
}

bool RegExParser::isInCharSetItemFollowSet(char c)
{
    return c == ']';
}
