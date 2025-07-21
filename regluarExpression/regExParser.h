#ifndef _REG_EX_PARSER_H_
#define _REG_EX_PARSER_H_

#include <string>

// regex       ::= term { '|' term }
// term        ::= factor { factor }
// factor      ::= atom [ '*' | '+' | '?' ]
// atom        ::= char | '(' regex ')'
// char        ::= letter | digit | '_' | ...

class RegExParser
{
public:
    RegExParser(const std::string &regExStr);
    ~RegExParser() = default;

private:
    void parseRegEx();
    void parseTerm();
    void parseFactor();
    void parseAtom();
    void parseChar();

    //should be a peek function
    char peekChar();
    void consumeChar();
    bool isEnd();

    bool isLetter(char c);
    bool isDigit(char c);
    bool isUnderscore(char c);

    bool isInFactorFirstSet(char c);
    bool isInFactorFollowSet(char c);

private:
    std::string regExStr_;
    std::size_t index_;
};

#endif // _REG_EX_PARSER_H_