#include "gtest/gtest.h"
#include "regExParser.h"
#include "regExNode.h"
#include "regExCharNode.h"
#include "regExKleeneStarNode.h"
#include "regExPlusNode.h"
#include "regExOptionalNode.h"
#include "regExConcatenationNode.h"
#include "regExAlternationNode.h"
#include "regExCharSetNode.h" // 新增：为了测试字符集节点
#include <string>
#include <memory>
#include <stdexcept>
#include <set> // 新增：为了验证字符集内容

// --- 辅助函数：用于将 AST 转换为字符串表示，以便于测试验证 ---
// 注意：这个函数假设了你的 RegExNode 派生类的 getter 方法存在：
// RegExCharNode::getCharactar()
// RegExKleeneStarNode::getNode()
// RegExPlusNode::getNode()
// RegExOptionalNode::getNode()
// RegExConcatenationNode::getLeft(), getRight()
// RegExAlternationNode::getLeft(), getRight()
// RegExCharSetNode::getCharSet(), isNegated()
std::string astToString(std::shared_ptr<RegExNode> node) {
    if (!node) {
        return "nullptr";
    }

    if (auto charNode = std::dynamic_pointer_cast<RegExCharNode>(node)) {
        // 对于特殊字符，可能需要转义才能在字符串中正确显示
        char c = charNode->getCharactar();
        if (c == '\n') return "\\n";
        if (c == '\t') return "\\t";
        if (c == '\r') return "\\r";
        // 如果是元字符，在测试输出时也显示为转义形式，方便理解
        // 注意：这里只是为了测试输出的可读性，不是真实正则的转义逻辑
        if (c == '*' || c == '+' || c == '?' || c == '|' || c == '(' || c == ')' || c == '[' || c == ']' || c == '\\' || c == '-' || c == '^') {
            return std::string("\\") + c;
        }
        return std::string(1, c);
    } else if (auto kleeneStarNode = std::dynamic_pointer_cast<RegExKleeneStarNode>(node)) {
        return "(" + astToString(kleeneStarNode->getNode()) + ")*";
    } else if (auto plusNode = std::dynamic_pointer_cast<RegExPlusNode>(node)) {
        return "(" + astToString(plusNode->getNode()) + ")+";
    } else if (auto optionalNode = std::dynamic_pointer_cast<RegExOptionalNode>(node)) {
        return "(" + astToString(optionalNode->getNode()) + ")?";
    } else if (auto concatenationNode = std::dynamic_pointer_cast<RegExConcatenationNode>(node)) {
        // 连接操作符的优先级高于或，但低于量词和原子，需要括号来正确表示
        // 为了AST字符串的清晰，这里总是加上括号
        return "(" + astToString(concatenationNode->getLeft()) + astToString(concatenationNode->getRight()) + ")";
    } else if (auto alternationNode = std::dynamic_pointer_cast<RegExAlternationNode>(node)) {
        // 或操作符优先级最低，也需要括号
        return "(" + astToString(alternationNode->getLeft()) + "|" + astToString(alternationNode->getRight()) + ")";
    } else if (auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(node)) {
        std::string set_str = "[";
        if (charSetNode->isNegated()) {
            set_str += "^";
        }
        // 为了确保顺序稳定，可以将 set 转换为 sorted vector
        std::string chars_in_set;
        for (char c : charSetNode->getCharSet()) {
            // 对字符集内部的特殊字符进行转义，方便调试和验证
            if (c == '\n') chars_in_set += "\\n";
            else if (c == '\t') chars_in_set += "\\t";
            else if (c == '\r') chars_in_set += "\\r";
            else if (c == '\\') chars_in_set += "\\\\"; // 转义反斜杠
            else if (c == '-') chars_in_set += "\\-";   // 转义连字符
            else if (c == ']') chars_in_set += "\\]";   // 转义右方括号
            else if (c == '^' && set_str == "[") chars_in_set += "\\^"; // 转义字符集开头的 ^
            else chars_in_set += c;
        }
        set_str += chars_in_set + "]";
        return set_str;
    }
    return "UNKNOWN_NODE";
}


// --- RegExParser 单元测试 ---

// ---
// ### 基本功能测试 (Basic Functionality Tests)
// ---

TEST(RegExParserTest, ValidChar) {
    RegExParser parser("a");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "a");
}

TEST(RegExParserTest, ValidConcatenation) {
    RegExParser parser("ab");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "(ab)");
}

TEST(RegExParserTest, ValidAlternation) {
    RegExParser parser("a|b");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "(a|b)");
}

TEST(RegExParserTest, ValidKleeneStar) {
    RegExParser parser("a*");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "(a)*");
}

TEST(RegExParserTest, ValidPlus) {
    RegExParser parser("a+");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "(a)+");
}

TEST(RegExParserTest, ValidOptional) {
    RegExParser parser("a?");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "(a)?");
}

TEST(RegExParserTest, ValidGroup) {
    RegExParser parser("(a)");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // 括号只改变优先级，不引入额外的 AST 节点层级（如果内部只有一个原子）
    ASSERT_EQ(astToString(root), "a");
}

TEST(RegExParserTest, ValidNestedGroup) {
    RegExParser parser("((a)b)");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // (a) 解析为 'a' 节点
    // ((a)b) 相当于 (ab)
    ASSERT_EQ(astToString(root), "(ab)");
}

TEST(RegExParserTest, ValidComplex) {
    RegExParser parser("a(b|c)*d+");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // 预期 AST: ((a((b|c))*)(d)+)
    // a -> a
    // (b|c) -> (b|c)
    // (b|c)* -> ((b|c))*
    // a(b|c)* -> (a((b|c))*)
    // d+ -> (d)+
    // (a(b|c)*)d+ -> ((a((b|c))*)(d)+)
    ASSERT_EQ(astToString(root), "((a((b|c))*)(d)+)");
}

TEST(RegExParserTest, ValidAlternationWithConcatenation) {
    RegExParser parser("ab|cd");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // 预期 AST: ((ab)|(cd))
    // ab -> (ab)
    // cd -> (cd)
    // (ab)|(cd) -> ((ab)|(cd))
    ASSERT_EQ(astToString(root), "((ab)|(cd))");
}

TEST(RegExParserTest, ValidConcatenationWithAlternation) {
    RegExParser parser("a(b|c)d");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // 预期 AST: ((a(b|c))d)
    // (b|c) -> (b|c)
    // a(b|c) -> (a(b|c))
    // (a(b|c))d -> ((a(b|c))d)
    ASSERT_EQ(astToString(root), "((a(b|c))d)");
}

TEST(RegExParserTest, ValidQuantifierOnAlternationGroup) {
    RegExParser parser("(a|b)*");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // 预期 AST: ((a|b))*
    // (a|b) -> (a|b)
    // (a|b)* -> ((a|b))*
    ASSERT_EQ(astToString(root), "((a|b))*");
}

// 注意：根据你的 `isLiteralChar` 定义，`_` 是字面字符，`1` 是字面字符。
// 所以 `a_1` 是合法的。
TEST(RegExParserTest, ValidLiteralCharsIncludingUnderscoreDigit) {
    RegExParser parser("a_1");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // 预期 AST: ((a_)1)
    ASSERT_EQ(astToString(root), "((a_)1)");
}

// ---
// ### 转义字符测试 (Escaped Characters Tests)
// ---

TEST(RegExParserTest, ValidEscapedMetaChar) {
    RegExParser parser("\\*");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "\\*");
}

TEST(RegExParserTest, ValidEscapedSpecialChar_n) {
    RegExParser parser("\\n");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "\\n"); // 确保 AST 存储的是 '\n'，但 astToString 转换为 "\\n"
    ASSERT_EQ(std::dynamic_pointer_cast<RegExCharNode>(root)->getCharactar(), '\n');
}

TEST(RegExParserTest, ValidEscapedSpecialChar_t) {
    RegExParser parser("\\t");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "\\t");
    ASSERT_EQ(std::dynamic_pointer_cast<RegExCharNode>(root)->getCharactar(), '\t');
}

TEST(RegExParserTest, ValidEscapedSpecialChar_r) {
    RegExParser parser("\\r");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "\\r");
    ASSERT_EQ(std::dynamic_pointer_cast<RegExCharNode>(root)->getCharactar(), '\r');
}

TEST(RegExParserTest, ValidEscapedCharClass_s) {
    RegExParser parser("\\s");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    ASSERT_FALSE(charSetNode->isNegated());
    // AlphaBet::instance().getWhiteSpaceChar() 应该包含 ' ', '\t', '\n', '\r'
    std::set<char> expected_set = {' ', '\t', '\n', '\r'};
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);
}

TEST(RegExParserTest, ValidEscapedCharClass_d) {
    RegExParser parser("\\d");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    ASSERT_FALSE(charSetNode->isNegated());
    // AlphaBet::instance().getDigitChar() 应该包含 '0' - '9'
    std::set<char> expected_set;
    for (char c = '0'; c <= '9'; ++c) {
        expected_set.insert(c);
    }
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);
}

TEST(RegExParserTest, ValidEscapedCharClass_w) {
    RegExParser parser("\\w");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    ASSERT_FALSE(charSetNode->isNegated());
    // AlphaBet::instance().getAlphaChar() 应该包含所有字母和下划线
    std::set<char> expected_set;
    for (char c = 'a'; c <= 'z'; ++c) expected_set.insert(c);
    for (char c = 'A'; c <= 'Z'; ++c) expected_set.insert(c);
    expected_set.insert('_');
    // 注意：\\w 通常也包含数字，根据你的 AlphaBet 实现调整
    for (char c = '0'; c <= '9'; ++c) expected_set.insert(c); // 根据你的AlphaBet::getAlphaChar()是否包含数字而定
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);
}


// ---
// ### 字符集测试 (Character Set Tests)
// ---

TEST(RegExParserTest, ValidCharSet_Simple) {
    RegExParser parser("[abc]");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    ASSERT_FALSE(charSetNode->isNegated());
    std::set<char> expected_set = {'a', 'b', 'c'};
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);
}

TEST(RegExParserTest, ValidCharSet_Negated) {
    RegExParser parser("[^abc]");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    ASSERT_TRUE(charSetNode->isNegated());
    std::set<char> expected_set = {'a', 'b', 'c'};
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);
}

TEST(RegExParserTest, ValidCharSet_RangeDigits) {
    RegExParser parser("[0-3]");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    ASSERT_FALSE(charSetNode->isNegated());
    std::set<char> expected_set = {'0', '1', '2', '3'};
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);
}

TEST(RegExParserTest, ValidCharSet_RangeLetters) {
    RegExParser parser("[a-d]");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    ASSERT_FALSE(charSetNode->isNegated());
    std::set<char> expected_set = {'a', 'b', 'c', 'd'};
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);
}

TEST(RegExParserTest, InvalidCharSet_RangeMixedCaseLetters) {
    // [a-C] 是一种无效的范围，因为 'a' 的 ASCII 值大于 'C'
    // 根据你的规则，当 start > end 时应抛出错误
    ASSERT_THROW(RegExParser parser("[a-C]"), std::runtime_error);
}

TEST(RegExParserTest, ValidCharSet_EscapedChar) {
    RegExParser parser("[\\*]");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    ASSERT_FALSE(charSetNode->isNegated());
    std::set<char> expected_set = {'*'};
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);
}

TEST(RegExParserTest, ValidCharSet_EscapedCharClass) {
    RegExParser parser("[\\s]");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    ASSERT_FALSE(charSetNode->isNegated());
    std::set<char> expected_set = {' ', '\t', '\n', '\r'};
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);
}

TEST(RegExParserTest, ValidCharSet_MixedItems) {
    RegExParser parser("[a-d\\sB]");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    ASSERT_FALSE(charSetNode->isNegated());
    std::set<char> expected_set = {'a', 'b', 'c', 'd', ' ', '\t', '\n', '\r', 'B'};
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);
}

TEST(RegExParserTest, ValidCharSet_DashLiteral) {
    RegExParser parser("[-abc]"); // 字符集开头或结尾的'-'是字面字符
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    std::set<char> expected_set = {'-', 'a', 'b', 'c'};
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);

    RegExParser parser2("[abc-]");
    std::shared_ptr<RegExNode> root2 = parser2.getRoot();
    ASSERT_NE(root2, nullptr);
    auto charSetNode2 = std::dynamic_pointer_cast<RegExCharSetNode>(root2);
    ASSERT_NE(charSetNode2, nullptr);
    ASSERT_EQ(charSetNode2->getCharSet(), expected_set);
}

TEST(RegExParserTest, ValidCharSet_EscapedDash) {
    RegExParser parser("[a\\-b]");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    std::set<char> expected_set = {'a', '-', 'b'};
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);
}

TEST(RegExParserTest, ValidCharSet_EscapedBracket) {
    RegExParser parser("[a\\]b]"); // 匹配字面量 ']'
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    std::set<char> expected_set = {'a', ']', 'b'};
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);
}

TEST(RegExParserTest, ValidCharSet_EscapedCaret) {
    RegExParser parser("[a\\^b]"); // 匹配字面量 '^'
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    auto charSetNode = std::dynamic_pointer_cast<RegExCharSetNode>(root);
    ASSERT_NE(charSetNode, nullptr);
    std::set<char> expected_set = {'a', '^', 'b'};
    ASSERT_EQ(charSetNode->getCharSet(), expected_set);
}


// ---
// ### 错误处理测试 (Error Handling Tests)
// ---

TEST(RegExParserTest, InvalidEmptyRegex) {
    ASSERT_THROW(RegExParser parser(""), std::runtime_error);
}

TEST(RegExParserTest, InvalidMissingClosingParen) {
    ASSERT_THROW(RegExParser parser("(a"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("a(b"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("a(b|c"), std::runtime_error);
}

TEST(RegExParserTest, InvalidEmptyGroup) {
    ASSERT_THROW(RegExParser parser("()"), std::runtime_error);
}

TEST(RegExParserTest, InvalidExtraClosingParen) {
    ASSERT_THROW(RegExParser parser("a)"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("ab)"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("(a))"), std::runtime_error);
}

TEST(RegExParserTest, InvalidOrAtEnd) {
    ASSERT_THROW(RegExParser parser("a|"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("(a|)"), std::runtime_error);
}

TEST(RegExParserTest, InvalidQuantifierAtStart) {
    ASSERT_THROW(RegExParser parser("*a"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("+a"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("?a"), std::runtime_error);
}

TEST(RegExParserTest, InvalidQuantifierAfterOr) {
    ASSERT_THROW(RegExParser parser("a|*b"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("a|+b"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("a|?b"), std::runtime_error);
}


// TEST(RegExParserTest, InvalidUnexpectedChar) {
//     ASSERT_THROW(RegExParser parser("a$b"), std::runtime_error); // $ 不是元字符或字面字符
//     ASSERT_THROW(RegExParser parser("a{1}"), std::runtime_error); // { 不是元字符或字面字符
// }

TEST(RegExParserTest, InvalidIncompleteRegex) {
    ASSERT_THROW(RegExParser parser("a(b|"), std::runtime_error); // 括号未闭合，或操作符后不完整
    ASSERT_THROW(RegExParser parser("a("), std::runtime_error);   // 括号未闭合
}

TEST(RegExParserTest, InvalidIncompleteRegexLeadingOr) {
    ASSERT_THROW(RegExParser parser("|a"), std::runtime_error); // 不能以 '|' 开头
}

TEST(RegExParserTest, InvalidIncompleteEscapedChar) {
    ASSERT_THROW(RegExParser parser("\\"), std::runtime_error); // 反斜杠后缺少字符
    ASSERT_THROW(RegExParser parser("a\\"), std::runtime_error); // 反斜杠后缺少字符
}

TEST(RegExParserTest, InvalidUnknownEscapedChar) {
    ASSERT_THROW(RegExParser parser("\\z"), std::runtime_error); // \\z 未知转义序列
    ASSERT_THROW(RegExParser parser("[\\z]"), std::runtime_error); // 字符集内部未知转义
}

// ---
// ### 字符集错误处理测试 (Character Set Error Handling Tests)
// ---

TEST(RegExParserTest, InvalidCharSet_Unclosed) {
    ASSERT_THROW(RegExParser parser("[a"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("[a-"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("[a-b"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("[^a"), std::runtime_error);
}

TEST(RegExParserTest, InvalidCharSet_Empty) {
    ASSERT_THROW(RegExParser parser("[]"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("[^]"), std::runtime_error);
}


TEST(RegExParserTest, InvalidCharSet_RangeMixedType) {
    ASSERT_THROW(RegExParser parser("[a-9]"), std::runtime_error); // 字母到数字范围
    ASSERT_THROW(RegExParser parser("[9-a]"), std::runtime_error); // 数字到字母范围
}

TEST(RegExParserTest, InvalidCharSet_RangeInverse) {
    ASSERT_THROW(RegExParser parser("[b-a]"), std::runtime_error); // 结束小于开始
    ASSERT_THROW(RegExParser parser("[2-1]"), std::runtime_error);
    ASSERT_THROW(RegExParser parser("[Z-A]"), std::runtime_error);
}