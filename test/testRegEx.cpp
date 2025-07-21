#include "gtest/gtest.h"
#include "regEx.h"
#include "regExCharNode.h"           // 字符节点
#include "regExConcatenationNode.h"  // 连接节点
#include "regExAlternationNode.h"    // 或操作节点
#include "regExKleeneStarNode.h"     // 闭包节点
#include "nfaState.h"                // NFAState, 假设其中定义了 CompareNfaStateSharedPtr
#include "alphaBet.h"                // 字母表单例

// 假设 NFAState 中有 operator== 或一个 unique ID 机制用于 CompareNfaStateSharedPtr
// 如果没有，你需要实现 CompareNfaStateSharedPtr，可能通过 NFAState 的 ID 或地址比较
// 例如：
/*
struct CompareNfaStateSharedPtr {
    bool operator()(const std::shared_ptr<NFAState>& a, const std::shared_ptr<NFAState>& b) const {
        return a.get() < b.get(); // 简单的指针地址比较，确保唯一性
        // 更健壮的可能是比较 NFAState 内部的唯一 ID
    }
};
*/

// --- Test Fixture ---
// 使用测试夹具来在每个测试前初始化 Alphabet
class RegExTest : public ::testing::Test {
protected:
    // 在每个测试用例运行前初始化 AlphaBet
    void SetUp() override {
        // 清理旧的字符集（如果 AlphaBet 是持久的）
        // 这里假设 AlphaBet::instance() 每次调用都会返回同一个单例，
        // 并且它的字符集可能会累积。如果它没有 clear() 方法，
        // 并且只在首次使用时初始化，那么可以省略这部分。
        // 但为了测试的独立性，最好确保字符集是干净的。
        // 由于你的 AlphaBet::instance().getcharactars() 可能是构建时确定的，
        // 你可能需要在测试运行前手动确保它包含所有必要的字符。

        // 示例：确保 Alphabet 包含测试中会用到的字符
        // 如果你的 Alphabet 是预定义的，这些调用可能是多余的
        // AlphaBet::instance().addChar('a');
        // AlphaBet::instance().addChar('b');
        // AlphaBet::instance().addChar('c');
        // AlphaBet::instance().addChar('0');
        // AlphaBet::instance().addChar('1');
        // AlphaBet::instance().addChar(' ');
        // 确保你的 Regex 可以处理的字符都在这里
        // 例如，如果正则表达式支持元字符，也需要考虑它们
    }

    void TearDown() override {
        // 可选：在每个测试用例运行后清理 AlphaBet，如果它有 clear 方法
        // AlphaBet::instance().clearcharactars();
    }
};

// --- 辅助函数：构建 RegExNode 树 ---
// 这些辅助函数是为了方便构建 RegExNode 树，模仿解析器生成 AST 的过程。
// **重要：这里我假设你的 RegExNode 派生类的构造函数已经改为接受 shared_ptr。**
// 如果不是，你需要自行调整这些辅助函数的实现，或者直接在测试中用裸指针构造。

// 构建字面量节点
std::shared_ptr<RegExNode> Char(char c) {
    return std::make_shared<RegExCharNode>(c);
}

// 构建连接节点
std::shared_ptr<RegExNode> Concat(std::shared_ptr<RegExNode> left, std::shared_ptr<RegExNode> right) {
    return std::make_shared<RegExConcatenationNode>(left, right);
}

// 构建或操作节点
std::shared_ptr<RegExNode> Alternation(std::shared_ptr<RegExNode> left, std::shared_ptr<RegExNode> right) {
    return std::make_shared<RegExAlternationNode>(left, right);
}

// 构建闭包节点
std::shared_ptr<RegExNode> KleeneStar(std::shared_ptr<RegExNode> node) {
    return std::make_shared<RegExKleeneStarNode>(node);
}

// --- 单元测试用例 ---

// 测试单个字符匹配
TEST_F(RegExTest, SingleCharMatch) {
    auto root = Char('a'); // 匹配 'a'
    RegEx regex(root);
    EXPECT_TRUE(regex.match("a"));
    EXPECT_FALSE(regex.match("b"));
    EXPECT_FALSE(regex.match("aa"));
    EXPECT_FALSE(regex.match(""));
}

// 测试连接操作 (ab)
TEST_F(RegExTest, ConcatenationBasic) {
    auto root = Concat(Char('a'), Char('b')); // 匹配 "ab"
    RegEx regex(root);
    EXPECT_TRUE(regex.match("ab"));
    EXPECT_FALSE(regex.match("a"));
    EXPECT_FALSE(regex.match("b"));
    EXPECT_FALSE(regex.match("ac"));
    EXPECT_FALSE(regex.match("ba"));
    EXPECT_FALSE(regex.match("abc"));
    EXPECT_FALSE(regex.match(""));
}

// 测试或操作 (a|b)
TEST_F(RegExTest, AlternationBasic) {
    auto root = Alternation(Char('a'), Char('b')); // 匹配 "a" 或 "b"
    RegEx regex(root);
    EXPECT_TRUE(regex.match("a"));
    EXPECT_TRUE(regex.match("b"));
    EXPECT_FALSE(regex.match("c"));
    EXPECT_FALSE(regex.match("ab"));
    EXPECT_FALSE(regex.match("ba"));
    EXPECT_FALSE(regex.match(""));
}

// 测试闭包操作 (a*)
TEST_F(RegExTest, KleeneStarBasic) {
    auto root = KleeneStar(Char('a')); // 匹配 "a*"
    RegEx regex(root);
    EXPECT_TRUE(regex.match(""));   // 零次匹配
    EXPECT_TRUE(regex.match("a"));  // 一次匹配
    EXPECT_TRUE(regex.match("aa")); // 多次匹配
    EXPECT_TRUE(regex.match("aaaaa"));
    EXPECT_FALSE(regex.match("b")); // 非'a'字符
    EXPECT_FALSE(regex.match("aab")); // 严格匹配，尾部有多余字符
    EXPECT_FALSE(regex.match("ba"));
}

// 组合测试：(ab)*
TEST_F(RegExTest, KleeneStarOfConcatenation) {
    auto ab = Concat(Char('a'), Char('b'));
    auto root = KleeneStar(ab); // 匹配 "(ab)*"
    RegEx regex(root);
    EXPECT_TRUE(regex.match(""));
    EXPECT_TRUE(regex.match("ab"));
    EXPECT_TRUE(regex.match("abab"));
    EXPECT_TRUE(regex.match("ababab"));
    EXPECT_FALSE(regex.match("a"));
    EXPECT_FALSE(regex.match("b"));
    EXPECT_FALSE(regex.match("aba")); // 不完全匹配
    EXPECT_FALSE(regex.match("abb")); // 不完全匹配
}

// 组合测试：a(b|c)d
TEST_F(RegExTest, ComplexCombination1) {
    auto bc = Alternation(Char('b'), Char('c'));
    auto abd = Concat(Char('a'), Concat(bc, Char('d'))); // 匹配 "a(b|c)d"
    RegEx regex(abd);
    EXPECT_TRUE(regex.match("abd"));
    EXPECT_TRUE(regex.match("acd"));
    EXPECT_FALSE(regex.match("ad"));
    EXPECT_FALSE(regex.match("abcd"));
    EXPECT_FALSE(regex.match("axd"));
    EXPECT_FALSE(regex.match("ab"));
    EXPECT_FALSE(regex.match(""));
}

// 组合测试：(a|b)*abb
TEST_F(RegExTest, ComplexCombination2) {
    auto ab_union = Alternation(Char('a'), Char('b'));
    auto ab_union_star = KleeneStar(ab_union); // (a|b)*

    auto literal_a = Char('a');
    auto literal_b1 = Char('b');
    auto literal_b2 = Char('b');
    auto abb = Concat(literal_a, Concat(literal_b1, literal_b2)); // abb

    auto root = Concat(ab_union_star, abb); // (a|b)*abb
    RegEx regex(root);

    EXPECT_TRUE(regex.match("abb"));
    EXPECT_TRUE(regex.match("aabb"));
    EXPECT_TRUE(regex.match("babb"));
    EXPECT_TRUE(regex.match("aaabb"));
    EXPECT_TRUE(regex.match("abababb"));
    EXPECT_FALSE(regex.match("ab"));   // 不完全匹配
    EXPECT_FALSE(regex.match("abbb")); // 多余字符
    EXPECT_FALSE(regex.match("abx"));  // 错误字符
    EXPECT_FALSE(regex.match(""));     // 不匹配空
    EXPECT_FALSE(regex.match("a"));
    EXPECT_FALSE(regex.match("b"));
    EXPECT_FALSE(regex.match("xabb")); // 开头不匹配
}

// 匹配空字符串的正则表达式 (例如，当正则表达式本身是空字符串时，或通过 KleeneStar 实现)
// 假设 RegExNode 层次结构支持生成匹配空字符串的 NFA (例如，root 为一个接受空字符串的 NFA 片段)
TEST_F(RegExTest, EmptyStringRegex) {
    // 匹配空字符串的正则表达式通常可以通过 `epsilon` 或 `()` 来表示。
    // 在你的构建方式中，最简单的是 `a*` 能够匹配空字符串。
    // 如果你有一个表示“空字符串”的专用 RegExNode，你可以这样测试：
    // auto root = std::make_shared<RegExEmptyStringNode>();
    // RegEx regex(root);
    // EXPECT_TRUE(regex.match(""));
    // EXPECT_FALSE(regex.match("a"));

    // 假设 `KleeneStar(Char('a'))` 的行为已经包含了空字符串匹配
    auto root = KleeneStar(Char('a')); // a*
    RegEx regex(root);
    EXPECT_TRUE(regex.match(""));
}

// 不匹配任何字符串的正则表达式 (例如，一个只包含死状态的 DFA)
// 这通常是通过无法到达接受状态的模式来实现。例如 'a|b' 匹配 'c'
TEST_F(RegExTest, NeverMatch) {
    auto root = Char('a');
    RegEx regex(root);
    // 这里 Alphabet 应该包含 'x'
    // AlphaBet::instance().addChar('x');
    EXPECT_FALSE(regex.match("x")); // "a" 永远不会匹配 "x"
}