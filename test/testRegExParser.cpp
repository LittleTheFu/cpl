#include "gtest/gtest.h"
#include "regExParser.h"
#include "regExNode.h"
#include "regExCharNode.h"
#include "regExKleeneStarNode.h"
#include "regExPlusNode.h"
#include "regExOptionalNode.h"
#include "regExConcatenationNode.h"
#include "regExAlternationNode.h"
#include <string>
#include <memory>
#include <stdexcept>

// --- 辅助函数：用于将 AST 转换为字符串表示，以便于测试验证 ---
// 注意：这个函数假设了你的 RegExNode 派生类的 getter 方法存在：
// RegExCharNode::getCharactar()
// RegExKleeneStarNode::getNode()
// RegExPlusNode::getNode()
// RegExOptionalNode::getNode()
// RegExConcatenationNode::getLeft(), getRight()
// RegExAlternationNode::getLeft(), getRight()
std::string astToString(std::shared_ptr<RegExNode> node) {
    if (!node) {
        return "nullptr";
    }

    if (auto charNode = std::dynamic_pointer_cast<RegExCharNode>(node)) {
        return std::string(1, charNode->getCharactar());
    } else if (auto kleeneStarNode = std::dynamic_pointer_cast<RegExKleeneStarNode>(node)) {
        return "(" + astToString(kleeneStarNode->getNode()) + ")*";
    } else if (auto plusNode = std::dynamic_pointer_cast<RegExPlusNode>(node)) {
        return "(" + astToString(plusNode->getNode()) + ")+";
    } else if (auto optionalNode = std::dynamic_pointer_cast<RegExOptionalNode>(node)) {
        return "(" + astToString(optionalNode->getNode()) + ")?";
    } else if (auto concatenationNode = std::dynamic_pointer_cast<RegExConcatenationNode>(node)) {
        return "(" + astToString(concatenationNode->getLeft()) + astToString(concatenationNode->getRight()) + ")";
    } else if (auto alternationNode = std::dynamic_pointer_cast<RegExAlternationNode>(node)) {
        return "(" + astToString(alternationNode->getLeft()) + "|" + astToString(alternationNode->getRight()) + ")";
    }
    return "UNKNOWN_NODE";
}

// --- RegExParser 单元测试 ---

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
    // 括号只改变优先级，不引入额外的 AST 节点层级
    ASSERT_EQ(astToString(root), "a");
}

TEST(RegExParserTest, ValidNestedGroup) {
    RegExParser parser("((a)b)");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // 解释：
    // (a) 解析为 'a' 节点
    // ((a)b) 相当于 (ab)
    // 最终 AST 为 'a' 连接 'b'
    ASSERT_EQ(astToString(root), "(ab)");
}

TEST(RegExParserTest, ValidComplex) {
    RegExParser parser("a(b|c)*d+");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // 解释：
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
    // 解释：
    // ab -> (ab)
    // cd -> (cd)
    // (ab)|(cd) -> ((ab)|(cd))
    ASSERT_EQ(astToString(root), "((ab)|(cd))");
}

TEST(RegExParserTest, ValidConcatenationWithAlternation) {
    RegExParser parser("a(b|c)d");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // 解释：
    // (b|c) -> (b|c)
    // a(b|c) -> (a(b|c))
    // (a(b|c))d -> ((a(b|c))d)
    ASSERT_EQ(astToString(root), "((a(b|c))d)");
}

TEST(RegExParserTest, ValidQuantifierOnAlternationGroup) {
    RegExParser parser("(a|b)*");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // 解释：
    // (a|b) -> (a|b)
    // (a|b)* -> ((a|b))*
    ASSERT_EQ(astToString(root), "((a|b))*");
}

TEST(RegExParserTest, ValidDigitAndUnderscore) {
    RegExParser parser("a_1");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // 解释：
    // a -> a
    // _ -> _
    // 1 -> 1
    // (a_) -> (a_)
    // (a_1) -> ((a_)1)
    ASSERT_EQ(astToString(root), "((a_)1)"); // 注意这里是左结合的连接
}

// --- 错误处理测试：验证解析器在遇到无效正则表达式时是否能正确抛出异常 ---

TEST(RegExParserTest, InvalidEmptyRegex) {
    ASSERT_THROW(RegExParser parser(""), std::runtime_error);
}

TEST(RegExParserTest, InvalidMissingClosingParen) {
    ASSERT_THROW(RegExParser parser("(a"), std::runtime_error);
}

TEST(RegExParserTest, InvalidEmptyGroup) {
    ASSERT_THROW(RegExParser parser("()"), std::runtime_error);
}

TEST(RegExParserTest, InvalidExtraClosingParen) {
    ASSERT_THROW(RegExParser parser("a)"), std::runtime_error);
}

TEST(RegExParserTest, InvalidOrAtEnd) {
    ASSERT_THROW(RegExParser parser("a|"), std::runtime_error);
}

TEST(RegExParserTest, InvalidQuantifierAtStart) {
    ASSERT_THROW(RegExParser parser("*a"), std::runtime_error);
}

TEST(RegExParserTest, InvalidUnexpectedChar) {
    ASSERT_THROW(RegExParser parser("a$b"), std::runtime_error);
}

TEST(RegExParserTest, InvalidIncompleteRegex) {
    ASSERT_THROW(RegExParser parser("a(b|"), std::runtime_error);
}

// TEST(RegExParserTest, InvalidIncompleteRegexAfterQuantifier) {
//     RegExParser parser("a*"); // 如果解析器在消耗完 '*' 后还期望更多输入，但没有，这里应该捕获
//     // 检查你的 RegExParser::parse() 方法是否包含了 `if (!isEnd()) { throw ... }`
//     RegExParser parser("a*b"); // 这是合法的
//     std::shared_ptr<RegExNode> root = parser.getRoot();
//     ASSERT_NE(root, nullptr);
//     ASSERT_EQ(astToString(root), "((a)*b)"); // ( (a)* 连接 b )
// }

TEST(RegExParserTest, InvalidIncompleteRegexLeadingOr) {
    ASSERT_THROW(RegExParser parser("|a"), std::runtime_error); // 不能以 '|' 开头
}

// --- 注意：为了测试能够运行，RegExParser 需要一个公共的 parse() 方法 ---
// 你需要在 RegExParser.h 中添加：
/*
class RegExParser {
public:
    // ... 其他已有的 public 方法 ...
    std::shared_ptr<RegExNode> parse() { // 新增的公共入口方法
        index_ = 0; // 每次解析前重置索引，以便可以重复使用 parser 对象
        root_ = parseRegEx(); // 调用私有的顶级解析函数

        // 检查是否所有输入都被消耗。如果有剩余，说明是语法错误。
        if (!isEnd()) {
            throw std::runtime_error("Invalid regex: unexpected characters at end of input.");
        }
        return root_;
    }
    // ...
private:
    std::shared_ptr<RegExNode> parseRegEx(); // 这个方法保持私有，由 parse() 调用
    // ...
};
*/