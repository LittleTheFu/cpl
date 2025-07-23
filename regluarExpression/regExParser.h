#ifndef _REG_EX_PARSER_H_
#define _REG_EX_PARSER_H_

#include <string>
#include <memory>
#include <set>


//-----------------------------------------------------------------------
// # 核心结构 - 优先级从低到高：或 -> 连接 -> 量词 -> 原子

// regex       ::=  term { '|' term }
//               # 一个正则表达式是一个或多个 'term' 通过 '|' (或) 连接

// term        ::=  factor { factor }
//               # 一个 'term' 是一个或多个 'factor' 的连接 (隐式连接，例如 'ab' 表示 'a' 连接 'b')

// factor      ::=  atom [ '*' | '+' | '?' ]
//               # 一个 'factor' 是一个 'atom' 后面可选跟着一个量词 ('*', '+', '?')

// atom        ::=  char_or_char_set | '(' regex ')'
//               # 一个 'atom' 是一个单个字符/字符集，或者是一个用括号括起来的子正则表达式

// # 字符和字符集定义 - 这是核心变化所在

// char_or_char_set ::=  escaped_char | literal_char | char_set
//                      # 一个字符或字符集可以是转义字符、字面字符，或者一个字符集定义

// literal_char ::=  <任何字符，除了 '\', '(', ')', '*', '+', '?', '|', '[', ']', '.'>
//                 # 字面字符是除了正则表达式元字符（本身有特殊含义）以外的任何字符。
//                 # 例如：'a', 'B', '0', '_', '=', '<', '!', '/', ';', ':', ',', '-' 等。

// escaped_char ::=  '\' ( 's' | 'd' | 'w' |          # 字符类：空白、数字、单词字符
//                        't' | 'n' | 'r' | 'f' | 'v' | # 控制字符：制表、换行、回车、换页、垂直制表
//                        '.' | '+' | '*' | '?' |      # 转义元字符：点、加、星、问
//                        '(' | ')' | '[' | ']' |      # 转义括号：左右圆括号、左右方括号
//                        '|' | '\' )                  # 转义管道、转义反斜杠
//                 # 转义字符是反斜杠 '\' 后面跟着一个特定的字符，
//                 # 用于表示字符类、控制字符或元字符的字面含义。

// char_set    ::=  '[' [ '^' ] (char_set_item { char_set_item }) ']'
//                 # 字符集以 '[' 开头，以 ']' 结尾。
//                 # 可选的 '^' 在 '[' 后面表示否定字符集 (匹配除了集合中所有字符以外的任何字符)。
//                 # 内部包含一个或多个 char_set_item。

// char_set_item ::=  literal_char | escaped_char | range
//                    # 字符集中的项可以是一个字面字符、一个转义字符，或者一个范围。

// range ::= (digit_char | lowercase_letter | uppercase_letter) '-' (digit_char | lowercase_letter | uppercase_letter)

class RegExNode;

class RegExParser
{
public:
    RegExParser(const std::string &regExStr);
    ~RegExParser() = default;

    std::shared_ptr<RegExNode> getRoot() { return root_; }

private:
    static const std::set<char> REGEX_META_CHARS;

private:
    std::shared_ptr<RegExNode> parseRegEx();
    std::shared_ptr<RegExNode> parseTerm();
    std::shared_ptr<RegExNode> parseFactor();
    std::shared_ptr<RegExNode> parseAtom();
    std::shared_ptr<RegExNode> parseCharOrCharSet();
    std::shared_ptr<RegExNode> parseLiteralChar();
    std::shared_ptr<RegExNode> parseEscapedChar();
    std::shared_ptr<RegExNode> parseCharSet();
    std::shared_ptr<RegExNode> parseCharSetItem(std::set<char> &charSet);
    void parseRange(std::set<char> &charSet);

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

    bool isMetaChar(char c);
    bool isLiteralChar(char c);
    bool isEscapedLiteralChar(char c);
    bool isAlphaNumber(char c);

    bool isInFactorFirstSet(char c);
    bool isInTermFollowSet(char c);
    bool isInCharSetItemFirstSet(char c);
    bool isInCharSetItemFollowSet(char c);

private:
    std::string regExStr_;
    std::size_t index_;

    std::shared_ptr<RegExNode> root_;
};

#endif // _REG_EX_PARSER_H_