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

const std::set<char> RegExParser::REGEX_META_CHARS = {'*', '+', '?', '|', '(', ')', '[', ']', '\\', '-', '^'};

RegExParser::RegExParser(const std::string &regExStr)
{
    regExStr_ = regExStr;
    index_ = 0;

    root_ = parseRegEx();

    if (!isEnd())
    {
        throw std::runtime_error("Invalid regex: Unexpected characters found after the end of the expression at index " + std::to_string(index_) + ".");
    }
}

std::shared_ptr<RegExNode> RegExParser::parseRegEx()
{
    if (isEnd())
    {
        throw std::runtime_error("Invalid regex: Empty or incomplete expression. (at index " + std::to_string(index_) + ").");
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
                throw std::runtime_error("Invalid regex: Unexpected end of expression after '|' at index " + std::to_string(index_) + ".");
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
        throw std::runtime_error("Invalid regex: Unexpected end of expression. (at index " + std::to_string(index_) + ").");
    }

    auto node = parseFactor();

    while (!isEnd() && isInFactorFirstSet(peekChar()))
    {
        node = std::make_shared<RegExConcatenationNode>(node, parseFactor());
    }

    if (!isEnd() && !isInTermFollowSet(peekChar()))
    {
        throw std::runtime_error("Invalid regex: Unexpected character after term. (at index " + std::to_string(index_) + ").");
    }

    return node;
}

std::shared_ptr<RegExNode> RegExParser::parseFactor()
{
    if (isEnd())
    {
        throw std::runtime_error("Invalid regex: Unexpected end of expression. (at index " + std::to_string(index_) + ").");
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
        throw std::runtime_error("Invalid regex: Unexpected end of expression. (at index " + std::to_string(index_) + ").");
    }

    char c = peekChar();

    if (c == '(')
    {
        consumeChar();
        auto node = parseRegEx();

        if (isEnd())
        {
            throw std::runtime_error("Invalid regex: Unexpected end of expression after '(' at index " + std::to_string(index_) + ".");
        }

        char cc = peekChar();
        if (cc != ')')
        {
            throw std::runtime_error("Invalid regex: Missing ')' after '(' at index " + std::to_string(index_) + ".");
        }

        consumeChar();
        return node;
    }
    else
    {
        return parseCharOrCharSet();
    }
}

std::shared_ptr<RegExNode> RegExParser::parseCharOrCharSet()
{
    char c = peekChar();

    if (c == '\\')
    {
        return parseEscapedChar();
    }
    else if (c == '[')
    {
        return parseCharSet();
    }
    else if(isLiteralChar(c))
    {
        return parseLiteralChar();
    }
    else
    {
        throw std::runtime_error("Invalid regex: Unexpected character. (at index " + std::to_string(index_) + ").");
    }
}

std::shared_ptr<RegExNode> RegExParser::parseLiteralChar()
{
    char c = peekChar();
    if (!isLiteralChar(c))
    {
        //we should reconsider the literal character set
        // throw std::runtime_error("Invalid regex: Unexpected character. (at index " + std::to_string(index_) + ").");
    }

    consumeChar();
    return std::make_shared<RegExCharNode>(c);
}

std::shared_ptr<RegExNode> RegExParser::parseEscapedChar()
{
    char c = peekChar();

    consumeChar();
    if(isEnd())
    {
        throw std::runtime_error("Invalid regex: Unexpected end of expression after '\\' at index " + std::to_string(index_) + ".");
    }

    char cc = peekChar();
    if (!isEscapedLiteralChar(cc))
    {
        throw std::runtime_error("Invalid regex: Unexpected character after '\\' at index " + std::to_string(index_) + ".");
    }
    consumeChar();

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
        return std::make_shared<RegExCharSetNode>(AlphaBet::instance().getWordChar());
    }
    else
    {
        throw std::runtime_error("Invalid regex: Unexpected character after '\\' at index " + std::to_string(index_) + ".");
    }
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

        if (isEnd() || peekChar() == ']')
        {
            throw std::runtime_error("Invalid regex: Empty or malformed character set (missing items before ']') at index " + std::to_string(index_) + ".");
        }

        do
        {
            parseCharSetItem(charSet);
        } while (!isEnd() && (peekChar() != ']'));

        if (isEnd())
        {
            throw std::runtime_error("Invalid regex: Unclosed character set. Expected ']' before end of expression. (at index " + std::to_string(index_) + ").");
        }
        if (peekChar() != ']')
        {
            throw std::runtime_error("Invalid regex: Expected ']' to close character set, but found '" + std::string(1, peekChar()) + "'. (at index " + std::to_string(index_) + ").");
        }
        consumeChar();

        return std::make_shared<RegExCharSetNode>(charSet, isNegated);
    }
   
    throw std::runtime_error("Internal error: parseCharSet called when current character is not '['. (at index " + std::to_string(index_) + ").");
}

void RegExParser::parseCharSetItem(std::set<char> &charSet)
{
    if(isEnd())
    {
        throw std::runtime_error("Invalid regex: Unexpected end of expression. (at index " + std::to_string(index_) + ").");
    }

    char c = peekChar();
    char cc = peekChar(1);
    if(isAlphaNumber(c) && cc == '-' && !isEnd(2) && isAlphaNumber(peekChar(2)))
    {
        parseRange(charSet);
    }
    else if(c == '\\')
    {
        std::shared_ptr<RegExNode> node = parseEscapedChar();

        std::shared_ptr<RegExCharNode> charNode = std::dynamic_pointer_cast<RegExCharNode>(node);
        std::shared_ptr<RegExCharSetNode> charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(node);

        if(charNode)
        {
            charSet.insert(charNode->getCharactar());
        }
        else if(charSetNode)
        {
            charSet.insert(charSetNode->getCharSet().begin(), charSetNode->getCharSet().end());
        }
        else
        {
            throw std::runtime_error("Invalid regex: Unexpected character after '\\' at index " + std::to_string(index_) + ".");
        }
    }
    else
    {
        std::shared_ptr<RegExNode> node = parseLiteralChar();
        std::shared_ptr<RegExCharNode> charNode = std::dynamic_pointer_cast<RegExCharNode>(node);
        if(charNode)
        {
            charSet.insert(charNode->getCharactar());
        }
        else
        {
            throw std::runtime_error("Invalid regex: Unexpected character. (at index " + std::to_string(index_) + ").");
        }
    }
}

// range ::= (digit_char | lowercase_letter | uppercase_letter) '-' (digit_char | lowercase_letter | uppercase_letter)
void RegExParser::parseRange(std::set<char> &charSet)
{
    char start = peekChar();
    if(!isAlphaNumber(start))
    {
        throw std::runtime_error("Invalid regex: Unexpected character. (at index " + std::to_string(index_) + ").");
    }

    consumeChar();
    if(peekChar() != '-')
    {
        throw std::runtime_error("Invalid regex: Expected '-' to define range, but found '" + std::string(1, peekChar()) + "'. (at index " + std::to_string(index_) + ").");
    }

    consumeChar();
    char end = peekChar();
    if(!isAlphaNumber(end))
    {
        throw std::runtime_error("Invalid regex: Unexpected character. (at index " + std::to_string(index_) + ").");
    }

    if( start > end )
    {
        throw std::runtime_error("Invalid regex: Invalid range. Start character '" + std::string(1, start) + "' is greater than end character '" + std::string(1, end) + "'. (at index " + std::to_string(index_) + ").");
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
            if(isLetter(c))
            {
                charSet.insert(c);
            }
        }
        return ;
    }

    throw std::runtime_error("Invalid regex: Invalid range. Start character '" + std::string(1, start) + "' is not a letter, and end character '" + std::string(1, end) + "' is not a letter. (at index " + std::to_string(index_) + ").");
}


char RegExParser::peekChar(size_t offset)
{
    if (index_ + offset >= regExStr_.size())
    {
        return '\0';
    }

    return regExStr_.at(index_ + offset);
}

void RegExParser::consumeChar()
{
    index_++;
}

bool RegExParser::isEnd(size_t offset)
{
    return index_ + offset >= regExStr_.size();
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
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
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
    return c == '|' || c == '\0' || c == ')';
}