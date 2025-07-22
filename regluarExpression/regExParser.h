#ifndef _REG_EX_PARSER_H_
#define _REG_EX_PARSER_H_

#include <string>
#include <memory>

// regex       ::= term { '|' term }
// term        ::= factor { factor }
// factor      ::= atom [ '*' | '+' | '?' ]
// atom        ::= char | '(' regex ')'
// char        ::= letter | digit | '_' | ...

class RegExNode;

class RegExParser
{
public:
    RegExParser(const std::string &regExStr);
    ~RegExParser() = default;

    std::shared_ptr<RegExNode> getRoot() { return root_; }

private:
    std::shared_ptr<RegExNode> parseRegEx();
    std::shared_ptr<RegExNode> parseTerm();
    std::shared_ptr<RegExNode> parseFactor();
    std::shared_ptr<RegExNode> parseAtom();
    std::shared_ptr<RegExNode> parseChar();

    //should be a peek function
    char peekChar();
    void consumeChar();
    bool isEnd();

    bool isLetter(char c);
    bool isDigit(char c);
    bool isUnderscore(char c);
    bool isBlank(char c);
    bool isTab(char c);
    bool isNewline(char c);
    bool isCarriageReturn(char c);


    bool isInFactorFirstSet(char c);
    bool isInFactorFollowSet(char c);

private:
    std::string regExStr_;
    std::size_t index_;

    std::shared_ptr<RegExNode> root_;
};

#endif // _REG_EX_PARSER_H_