#include "gtest/gtest.h"
#include "regEx.h"
#include "regExCharNode.h"           // 字符节点
#include "regExConcatenationNode.h"  // 连接节点
#include "regExAlternationNode.h"    // 或操作节点
#include "regExKleeneStarNode.h"     // 闭包节点
#include "nfaState.h"                // NFAState, 假设其中定义了 CompareNfaStateSharedPtr
#include "alphaBet.h"                // 字母表单例

// 假设 CompareNfaStateSharedPtr 已经正确实现，用于 std::set<std::shared_ptr<NFAState>>

// --- Test Fixture ---
// 使用测试夹具来在每个测试前初始化 Alphabet
class RegExTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 这里的 Alphabet 初始化逻辑保持不变。
        // 确保你的 Alphabet 包含了所有测试中会用到的字符。
    }

    void TearDown() override {
        // 可选：在每个测试用例运行后清理 Alphabet
    }
};

// --- 辅助函数：构建 RegExNode 树 ---
// 这些辅助函数是为了方便构建 RegExNode 树，模仿解析器生成 AST 的过程。

std::shared_ptr<RegExNode> Char(char c) {
    return std::make_shared<RegExCharNode>(c);
}

std::shared_ptr<RegExNode> Concat(std::shared_ptr<RegExNode> left, std::shared_ptr<RegExNode> right) {
    return std::make_shared<RegExConcatenationNode>(left, right);
}

std::shared_ptr<RegExNode> Alternation(std::shared_ptr<RegExNode> left, std::shared_ptr<RegExNode> right) {
    return std::make_shared<RegExAlternationNode>(left, right);
}

std::shared_ptr<RegExNode> KleeneStar(std::shared_ptr<RegExNode> node) {
    return std::make_shared<RegExKleeneStarNode>(node);
}

// --- 单元测试用例 ---

// 测试单个字符匹配
TEST_F(RegExTest, SingleCharMatch) {
    auto root = Char('a'); // 匹配 'a'
    RegEx regex(root);

    // 匹配 "a"，期望成功，长度为 1
    auto result = regex.match("a");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);

    // 匹配 "b"，期望失败 (因为开头就不是 'a')
    result = regex.match("b");
    EXPECT_FALSE(result.has_value());

    // 匹配 "aa"，期望成功，但只匹配前缀 "a"，长度为 1
    result = regex.match("aa");
    EXPECT_TRUE(result.has_value()); // 能匹配到前缀 "a"
    EXPECT_EQ(result.value(), 1);    // 匹配长度是 1

    // 匹配空字符串 ""，期望失败 (因为 "a" 不能匹配空)
    result = regex.match("");
    EXPECT_FALSE(result.has_value());
}

// 测试连接操作 (ab)
TEST_F(RegExTest, ConcatenationBasic) {
    auto root = Concat(Char('a'), Char('b')); // 匹配 "ab"
    RegEx regex(root);

    // 匹配 "ab"，期望成功，长度为 2
    auto result = regex.match("ab");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 2);

    // 匹配 "a"，期望失败 (不完整的匹配，"ab" 无法匹配 "a")
    result = regex.match("a");
    EXPECT_FALSE(result.has_value());

    // 匹配 "b"，期望失败
    result = regex.match("b");
    EXPECT_FALSE(result.has_value());

    // 匹配 "ac"，期望失败 (第二个字符不匹配)
    result = regex.match("ac");
    EXPECT_FALSE(result.has_value());

    // 匹配 "ba"，期望失败 (开头就不匹配)
    result = regex.match("ba");
    EXPECT_FALSE(result.has_value());

    // 匹配 "abc"，期望成功，匹配前缀 "ab"，长度为 2
    result = regex.match("abc");
    EXPECT_TRUE(result.has_value()); // 能匹配到前缀 "ab"
    EXPECT_EQ(result.value(), 2);    // 匹配长度是 2

    // 匹配空字符串 ""，期望失败
    result = regex.match("");
    EXPECT_FALSE(result.has_value());
}

// 测试或操作 (a|b)
TEST_F(RegExTest, AlternationBasic) {
    auto root = Alternation(Char('a'), Char('b')); // 匹配 "a" 或 "b"
    RegEx regex(root);

    // 匹配 "a"，期望成功，长度为 1
    auto result = regex.match("a");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);

    // 匹配 "b"，期望成功，长度为 1
    result = regex.match("b");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);

    // 匹配 "c"，期望失败
    result = regex.match("c");
    EXPECT_FALSE(result.has_value());

    // 匹配 "ab"，期望成功，匹配前缀 "a"，长度为 1 (优先匹配到 'a')
    // 如果实现是“最长匹配”，这里会匹配到 'a'
    result = regex.match("ab");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);

    // 匹配 "ba"，期望成功，匹配前缀 "b"，长度为 1
    result = regex.match("ba");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);

    // 匹配空字符串 ""，期望失败
    result = regex.match("");
    EXPECT_FALSE(result.has_value());
}

// 测试闭包操作 (a*)
TEST_F(RegExTest, KleeneStarBasic) {
    auto root = KleeneStar(Char('a')); // 匹配 "a*"
    RegEx regex(root);

    // 匹配空字符串 ""，期望成功，长度为 0
    auto result = regex.match("");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);

    // 匹配 "a"，期望成功，长度为 1
    result = regex.match("a");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);

    // 匹配 "aa"，期望成功，长度为 2
    result = regex.match("aa");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 2);

    // 匹配 "aaaaa"，期望成功，长度为 5
    result = regex.match("aaaaa");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 5);

    // 匹配 "b"，期望成功，匹配长度为 0 (因为 'a*' 可以匹配空字符串)
    result = regex.match("b");
    EXPECT_TRUE(result.has_value()); // 期望有值 (匹配了空字符串)
    EXPECT_EQ(result.value(), 0);    // 期望匹配长度是 0

    // 匹配 "aab"，期望成功，匹配前缀 "aa"，长度为 2
    result = regex.match("aab");
    EXPECT_TRUE(result.has_value()); // 能匹配到前缀 "aa"
    EXPECT_EQ(result.value(), 2);    // 匹配长度是 2

    // --- 最终修正点 ---
    // 匹配 "ba"，期望成功，匹配长度为 0 (因为 'a*' 可以匹配空字符串)
    result = regex.match("ba");
    EXPECT_TRUE(result.has_value()); // 期望有值 (匹配了空字符串)
    EXPECT_EQ(result.value(), 0);    // 期望匹配长度是 0
}

// 组合测试：(ab)*
TEST_F(RegExTest, KleeneStarOfConcatenation) {
    auto ab = Concat(Char('a'), Char('b'));
    auto root = KleeneStar(ab); // 匹配 "(ab)*"
    RegEx regex(root);

    // 匹配空字符串 ""，期望成功，长度为 0
    auto result = regex.match("");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);

    // 匹配 "ab"，期望成功，长度为 2
    result = regex.match("ab");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 2);

    // 匹配 "abab"，期望成功，长度为 4
    result = regex.match("abab");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 4);

    // 匹配 "ababab"，期望成功，长度为 6
    result = regex.match("ababab");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 6);

    // --- 修正点 ---
    // 匹配 "a"，期望成功，匹配长度为 0 (因为 '(ab)*' 可以匹配空字符串)
    result = regex.match("a");
    EXPECT_TRUE(result.has_value()); // 期望有值 (匹配了空字符串)
    EXPECT_EQ(result.value(), 0);    // 期望匹配长度是 0

    // --- 修正点 ---
    // 匹配 "b"，期望成功，匹配长度为 0 (因为 '(ab)*' 可以匹配空字符串)
    result = regex.match("b");
    EXPECT_TRUE(result.has_value()); // 期望有值 (匹配了空字符串)
    EXPECT_EQ(result.value(), 0);    // 期望匹配长度是 0

    // 匹配 "aba"，期望成功，匹配前缀 "ab"，长度为 2
    result = regex.match("aba");
    EXPECT_TRUE(result.has_value()); // 能匹配到前缀 "ab"
    EXPECT_EQ(result.value(), 2);    // 匹配长度是 2

    // 匹配 "abb"，期望成功，匹配前缀 "ab"，长度为 2
    result = regex.match("abb");
    EXPECT_TRUE(result.has_value()); // 能匹配到前缀 "ab"
    EXPECT_EQ(result.value(), 2);    // 匹配长度是 2
}

// 组合测试：a(b|c)d
TEST_F(RegExTest, ComplexCombination1) {
    auto bc = Alternation(Char('b'), Char('c'));
    auto abd = Concat(Char('a'), Concat(bc, Char('d'))); // 匹配 "a(b|c)d"
    RegEx regex(abd);

    // 匹配 "abd"，期望成功，长度为 3
    auto result = regex.match("abd");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3);

    // 匹配 "acd"，期望成功，长度为 3
    result = regex.match("acd");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3);

    // 匹配 "ad"，期望失败 (缺少中间字符)
    result = regex.match("ad");
    EXPECT_FALSE(result.has_value());

    // 匹配 "abcd"，期望失败 (因为 'c' 不是 'd'，模式不匹配)
    result = regex.match("abcd");
    EXPECT_FALSE(result.has_value()); // 期望没有值，因为 "abcd" 不能作为 "a(b|c)d" 的前缀被接受

    // 匹配 "axd"，期望失败 (中间字符不匹配)
    result = regex.match("axd");
    EXPECT_FALSE(result.has_value());

    // 匹配 "ab"，期望失败 (不完整匹配)
    result = regex.match("ab");
    EXPECT_FALSE(result.has_value());

    // 匹配空字符串 ""，期望失败
    result = regex.match("");
    EXPECT_FALSE(result.has_value());
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

    // 匹配 "abb"，期望成功，长度为 3
    auto result = regex.match("abb");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3);

    // 匹配 "aabb"，期望成功，长度为 4
    result = regex.match("aabb");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 4);

    // 匹配 "babb"，期望成功，长度为 4
    result = regex.match("babb");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 4);

    // 匹配 "aaabb"，期望成功，长度为 5
    result = regex.match("aaabb");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 5);

    // 匹配 "abababb"，期望成功，长度为 7
    result = regex.match("abababb");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 7);

    // 匹配 "ab"，期望失败 (不完全匹配，不是 (a|b)*abb 的前缀)
    result = regex.match("ab");
    EXPECT_FALSE(result.has_value());

    // 匹配 "abbb"，期望成功，匹配前缀 "abb"，长度为 3
    result = regex.match("abbb");
    EXPECT_TRUE(result.has_value()); // 能匹配到前缀 "abb"
    EXPECT_EQ(result.value(), 3);    // 匹配长度是 3

    // 匹配 "abx"，期望失败 (因为 'x' 破坏了 abb 结构)
    result = regex.match("abx");
    EXPECT_FALSE(result.has_value());

    // 匹配空字符串 ""，期望失败 (因为必须匹配至少 "abb")
    result = regex.match("");
    EXPECT_FALSE(result.has_value());

    // 匹配 "a"，期望失败
    result = regex.match("a");
    EXPECT_FALSE(result.has_value());

    // 匹配 "b"，期望失败
    result = regex.match("b");
    EXPECT_FALSE(result.has_value());

    // 匹配 "xabb"，期望失败 (开头不匹配)
    result = regex.match("xabb");
    EXPECT_FALSE(result.has_value());
}

// 匹配空字符串的正则表达式 (例如，当正则表达式本身是空字符串时，或通过 KleeneStar 实现)
TEST_F(RegExTest, EmptyStringRegex) {
    auto root = KleeneStar(Char('a')); // a*
    RegEx regex(root);

    // 匹配空字符串 ""，期望成功，长度为 0
    auto result = regex.match("");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);

    // 匹配 "a"，期望成功，长度为 1
    result = regex.match("a");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);
    
    // 匹配 "b"，期望成功，长度为 0
    result = regex.match("b");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);
}

// 不匹配任何字符串的正则表达式
TEST_F(RegExTest, NeverMatch) {
    auto root = Char('a'); // 匹配 'a'
    RegEx regex(root);

    // 匹配 "x"，期望失败 ("a" 永远不会匹配 "x")
    auto result = regex.match("x");
    EXPECT_FALSE(result.has_value());
}