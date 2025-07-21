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

    std::pair<bool, char> nextChar();

private:
    std::string regExStr_;
    std::size_t index_;

}


#endif // _REG_EX_PARSER_H_