#ifndef _REG_EX_PARSER_H_
#define _REG_EX_PARSER_H_

#include <string>
#include <memory>

// regex       ::= term { '|' term }
// term        ::= factor { factor }
// factor      ::= atom [ '*' | '+' | '?' ]
// atom        ::= char | '(' regex ')'
// char        ::= letter | digit | '_' | ...

//-----------------------------------------------------------------------
// regex       ::=  term { '|' term }
// term        ::=  factor { factor }
// factor      ::=  atom [ '*' | '+' | '?' ]
// atom        ::=  char_or_char_set | '(' regex ')'

// // char_or_char_set can be a single escaped or unescaped character, or a character set like [a-z0-9]
// char_or_char_set ::=  escaped_char | literal_char | char_set

// // literal_char is any character that is NOT a regex meta-character and NOT a backslash
// literal_char ::=  <any character except '\', '(', ')', '*', '+', '?', '|', '[', ']'>

// // escaped_char handles backslash sequences
// escaped_char ::=  '\' ( 's' | 'd' | 'w' | '.' | '+' | '*' | '?' | '(' | ')' | '[' | ']' | '|' | '\' | <other specific escapes like 't', 'n', 'r'> )

// // char_set allows defining custom character ranges/lists, e.g., [a-zA-Z0-9_]
// char_set    ::=  '[' (literal_char | escaped_char | range) { (literal_char | escaped_char | range) } ']'
// range       ::=  (literal_char | escaped_char) '-' (literal_char | escaped_char) // e.g., 'a-z', '0-9'

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

    bool isWhiteSpace(char c);
    bool isOperator(char c);


    bool isInFactorFirstSet(char c);
    bool isInFactorFollowSet(char c);

private:
    std::string regExStr_;
    std::size_t index_;

    std::shared_ptr<RegExNode> root_;
};

#endif // _REG_EX_PARSER_H_