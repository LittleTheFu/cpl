#include "gtest/gtest.h"
#include "regExParser.h" // 包含你的 RegExParser 类
#include "regExNode.h" // 确保包含所有 AST 节点基类和派生类的头文件
#include "regExCharNode.h"
#include "regExKleeneStarNode.h"
#include "regExPlusNode.h"
#include "regExOptionalNode.h"
#include "regExConcatenationNode.h"
#include "regExAlternationNode.h"
#include <string>
#include <memory>
#include <stdexcept> // 用于捕获 std::runtime_error

// --- 辅助函数：用于将 AST 转换为字符串表示，以便于测试验证 ---
// 注意：这个函数假设了你的 RegExNode 派生类的构造函数和成员变量名称。
// 实际的 AST 节点类结构需要与之匹配。
std::string astToString(std::shared_ptr<RegExNode> node) {
    if (!node) {
        return "nullptr";
    }

    // 尝试向下转型以识别具体的节点类型
    if (auto charNode = std::dynamic_pointer_cast<RegExCharNode>(node)) {
        return std::string(1, charNode->getCharactar());
    } else if (auto kleeneStarNode = std::dynamic_pointer_cast<RegExKleeneStarNode>(node)) {
        return "(" + astToString(kleeneStarNode->getNode()) + ")*";
    } else if (auto plusNode = std::dynamic_pointer_cast<RegExPlusNode>(node)) {
        return "(" + astToString(plusNode->getNode()) + ")+";
    } else if (auto optionalNode = std::dynamic_pointer_cast<RegExOptionalNode>(node)) {
        return "(" + astToString(optionalNode->getNode()) + ")?";
    } else if (auto concatenationNode = std::dynamic_pointer_cast<RegExConcatenationNode>(node)) {
        // 连接操作在 AST 中通常表现为左右子树的并置
        // 这里为了清晰地展示结构，用括号包起来
        return "(" + astToString(concatenationNode->getLeft()) + astToString(concatenationNode->getRight()) + ")";
    } else if (auto alternationNode = std::dynamic_pointer_cast<RegExAlternationNode>(node)) {
        // 或操作
        return "(" + astToString(alternationNode->getLeft()) + "|" + astToString(alternationNode->getRight()) + ")";
    }
    return "UNKNOWN_NODE"; // 如果遇到未知的节点类型
}

// --- RegExParser 单元测试 ---

// 测试基本字符解析
TEST(RegExParserTest, ValidChar) {
    RegExParser parser("a");
    std::shared_ptr<RegExNode> root = parser.getRoot(); // 调用公共 parse() 方法
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "a");
}

// 测试连接操作解析 (例如 "ab")
TEST(RegExParserTest, ValidConcatenation) {
    RegExParser parser("ab");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "(ab)"); // (a 连接 b)
}

// 测试或操作解析 (例如 "a|b")
TEST(RegExParserTest, ValidAlternation) {
    RegExParser parser("a|b");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "(a|b)"); // (a 或 b)
}

// 测试 Kleene Star (*) 量词解析
TEST(RegExParserTest, ValidKleeneStar) {
    RegExParser parser("a*");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "(a)*"); // (a 的 Kleene Star)
}

// 测试 Plus (+) 量词解析
TEST(RegExParserTest, ValidPlus) {
    RegExParser parser("a+");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "(a)+"); // (a 的 Plus)
}

// 测试 Optional (?) 量词解析
TEST(RegExParserTest, ValidOptional) {
    RegExParser parser("a?");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "(a)?"); // (a 的 Optional)
}

// 测试分组解析 (例如 "(a)")
TEST(RegExParserTest, ValidGroup) {
    RegExParser parser("(a)");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "a"); // 括号通常只改变优先级，AST 结构直接返回内部节点
}

// 测试嵌套分组解析 (例如 "((a)b)")
TEST(RegExParserTest, ValidNestedGroup) {
    RegExParser parser("((a)b)");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // 预期 AST: ((a连接b))
    ASSERT_EQ(astToString(root), "((ab))");
}

// 测试复杂正则表达式的解析
TEST(RegExParserTest, ValidComplex) {
    RegExParser parser("a(b|c)*d+");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    // 预期 AST 结构: (((a ( (b|c) * ) ) d)+)
    // 根据你的文法和优先级，连接操作优先级高于或操作，量词作用于atom
    // a(b|c)*d+ 会被解析为 (a ( (b|c)* ) d)+
    ASSERT_EQ(astToString(root), "((a((b|c))*d)))+"); // ( (a ( (b|c) * ) ) d ) +
}

// 测试包含数字和下划线的字符
TEST(RegExParserTest, ValidDigitAndUnderscore) {
    RegExParser parser("a_1");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "(a(_1))"); // (a 连接 (_ 连接 1))
}

// --- 错误处理测试：验证解析器在遇到无效正则表达式时是否能正确抛出异常 ---

// 测试空正则表达式
TEST(RegExParserTest, InvalidEmptyRegex) {
    RegExParser parser("");
    ASSERT_THROW(parser.getRoot(), std::runtime_error); // 假设空串是非法正则
}

// 测试缺少闭合括号
TEST(RegExParserTest, InvalidMissingClosingParen) {
    RegExParser parser("(a");
    ASSERT_THROW(parser.getRoot(), std::runtime_error);
}

// 测试空分组 (如果你的设计不允许 "()")
TEST(RegExParserTest, InvalidEmptyGroup) {
    RegExParser parser("()");
    ASSERT_THROW(parser.getRoot(), std::runtime_error); // 假设空组 () 是非法正则
}

// 测试多余的闭合括号
TEST(RegExParserTest, InvalidExtraClosingParen) {
    RegExParser parser("a)");
    ASSERT_THROW(parser.getRoot(), std::runtime_error);
}

// 测试在表达式末尾有多余的 '|'
TEST(RegExParserTest, InvalidOrAtEnd) {
    RegExParser parser("a|");
    ASSERT_THROW(parser.getRoot(), std::runtime_error);
}

// 测试在开头有多余的量词
TEST(RegExParserTest, InvalidQuantifierAtStart) {
    RegExParser parser("*a");
    ASSERT_THROW(parser.getRoot(), std::runtime_error); // factor 期望 atom，但遇到 '*'
}

// 测试包含未定义字符 (例如 '$' 不在你的合法字符集内)
TEST(RegExParserTest, InvalidUnexpectedChar) {
    RegExParser parser("a$b"); // 假设 '$' 不在 isLetter, isDigit, isUnderscore 中
    ASSERT_THROW(parser.getRoot(), std::runtime_error);
}

// 测试不完整的正则表达式（例如，连接操作后预期更多内容但已结束）
TEST(RegExParserTest, InvalidIncompleteRegex) {
    RegExParser parser("a(b|"); // 期望 b| 后有 term
    ASSERT_THROW(parser.getRoot(), std::runtime_error);
}

// 测试量词作用于整个表达式（如果你的文法不支持，比如 "(a|b)*" 作为 atom 的量词）
TEST(RegExParserTest, ValidQuantifierOnSubExpression) {
    // 你的文法是 factor ::= atom [ '*' | '+' | '?' ]
    // 其中 atom 是 char 或 (regex)。所以 (a|b) 是一个 atom，其后可以跟量词。
    RegExParser parser("(a|b)*");
    std::shared_ptr<RegExNode> root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(astToString(root), "((a|b))*"); // ( (a|b) 的 Kleene Star)
}

/*
// --- 在你的 RegExParser.h/cpp 中需要添加的公共 parse() 方法示例 ---
// RegExParser.h:
class RegExParser {
public:
    // ... 其他已有的 public 方法 ...
    std::shared_ptr<RegExNode> parse(); // 新增的公共入口方法
    // ...
private:
    std::shared_ptr<RegExNode> parseRegEx(); // 这个方法保持私有，由 parse() 调用
    // ...
};

// RegExParser.cpp:
std::shared_ptr<RegExNode> RegExParser::parse() {
    index_ = 0; // 每次解析前重置索引，以便可以重复使用 parser 对象
    root_ = parseRegEx(); // 调用私有的顶级解析函数

    // 检查是否所有输入都被消耗。如果有剩余，说明是语法错误。
    if (!isEnd()) {
        throw std::runtime_error("Invalid regex: unexpected characters at end of input.");
    }
    return root_;
}
*/