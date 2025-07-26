#include "gtest/gtest.h"
#include "grammar.h" // 确保这是你的 Grammar 类头文件的正确路径

// 假设你的 GrammarSymbol 和 ProductionRule 类已经正确定义
// 并且 GrammarSymbol::getEpsilonSymbol() 返回一个表示 ε 的静态 GrammarSymbol 对象

// 定义一个测试套件，继承自 ::testing::Test
class FirstSetTest : public ::testing::Test {
protected:
    // 辅助函数，用于方便地创建 GrammarSymbol 对象
    GrammarSymbol T(const std::string& name) {
        return GrammarSymbol(name, SymbolType::Terminal);
    }
    GrammarSymbol NT(const std::string& name) {
        return GrammarSymbol(name, SymbolType::NonTerminal);
    }
    GrammarSymbol Epsilon() {
        return GrammarSymbol::getEpsilonSymbol();
    }
    // 注意: First 集合不包含 EndSymbol，所以这里不需要 EndSymbol() 辅助函数
};

// --- Test Cases for First Sets ---

// 测试案例 1: 简单文法，无 ε 产生式
// S ::= A B
// A ::= a
// B ::= b
TEST_F(FirstSetTest, SimpleGrammar) {
    // 定义产生式规则
    ProductionRule rule1(NT("S"), {NT("A"), NT("B")});
    ProductionRule rule2(NT("A"), {T("a")});
    ProductionRule rule3(NT("B"), {T("b")});

    std::vector<ProductionRule> rules = {rule1, rule2, rule3};
    Grammar grammar(rules, NT("S")); // 以 S 为起始符号

    // 计算 First 集合
    grammar.calculateFirstSets();

    // 验证 First 集合结果
    // 对于非终结符
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("S")), (std::set<GrammarSymbol>{T("a")}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("A")), (std::set<GrammarSymbol>{T("a")}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("B")), (std::set<GrammarSymbol>{T("b")}));
    // 对于终结符 (First(Terminal) = {Terminal} )
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(T("a")), (std::set<GrammarSymbol>{T("a")}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(T("b")), (std::set<GrammarSymbol>{T("b")}));
}

// 测试案例 2: 带有 ε 产生式的文法，验证 ε 传播
// S ::= A B
// A ::= a | ε
// B ::= b
TEST_F(FirstSetTest, GrammarWithEpsilon) {
    ProductionRule rule1(NT("S"), {NT("A"), NT("B")});
    ProductionRule rule2(NT("A"), {T("a")});
    ProductionRule rule3(NT("A"), {}); // A ::= ε
    ProductionRule rule4(NT("B"), {T("b")});

    std::vector<ProductionRule> rules = {rule1, rule2, rule3, rule4};
    Grammar grammar(rules, NT("S"));

    grammar.calculateFirstSets();

    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("S")), (std::set<GrammarSymbol>{T("a"), T("b")})); // First(A) = {a, ε}, First(B) = {b} => First(S) = First(A) - {ε} U First(B) = {a, b}
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("A")), (std::set<GrammarSymbol>{T("a"), Epsilon()}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("B")), (std::set<GrammarSymbol>{T("b")}));
}

// 测试案例 3: 所有非终结符都能推导出 ε
// S ::= A B
// A ::= x | ε
// B ::= y | ε
TEST_F(FirstSetTest, AllCanDeriveEpsilon) {
    ProductionRule rule1(NT("S"), {NT("A"), NT("B")});
    ProductionRule rule2(NT("A"), {T("x")});
    ProductionRule rule3(NT("A"), {}); // A ::= ε
    ProductionRule rule4(NT("B"), {T("y")});
    ProductionRule rule5(NT("B"), {}); // B ::= ε

    std::vector<ProductionRule> rules = {rule1, rule2, rule3, rule4, rule5};
    Grammar grammar(rules, NT("S"));

    grammar.calculateFirstSets();

    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("S")), (std::set<GrammarSymbol>{T("x"), T("y"), Epsilon()})); // First(S) = First(A) - {ε} U First(B) - {ε} U {ε} (因为 A, B 都能推导出 ε)
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("A")), (std::set<GrammarSymbol>{T("x"), Epsilon()}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("B")), (std::set<GrammarSymbol>{T("y"), Epsilon()}));
}

// 测试案例 4: 带有左递归的文法 (E ::= E + T | T)
// E ::= E + T | T
// T ::= T * F | F
// F ::= ( E ) | id
TEST_F(FirstSetTest, GrammarWithLeftRecursion) {
    ProductionRule rule1(NT("E"), {NT("E"), T("+"), NT("T")});
    ProductionRule rule2(NT("E"), {NT("T")});
    ProductionRule rule3(NT("T"), {NT("T"), T("*"), NT("F")});
    ProductionRule rule4(NT("T"), {NT("F")});
    ProductionRule rule5(NT("F"), {T("("), NT("E"), T(")")});
    ProductionRule rule6(NT("F"), {T("id")});

    std::vector<ProductionRule> rules = {rule1, rule2, rule3, rule4, rule5, rule6};
    Grammar grammar(rules, NT("E")); // 通常 E 是表达式文法的起始符号

    grammar.calculateFirstSets();

    // 预期结果：First(E) = First(T) = First(F) = { (, id }
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("E")), (std::set<GrammarSymbol>{T("("), T("id")}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("T")), (std::set<GrammarSymbol>{T("("), T("id")}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("F")), (std::set<GrammarSymbol>{T("("), T("id")}));

    EXPECT_EQ(grammar.getCalculatedFirstSets().at(T("+")), (std::set<GrammarSymbol>{T("+")}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(T("*")), (std::set<GrammarSymbol>{T("*")}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(T("(")), (std::set<GrammarSymbol>{T("(")}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(T(")")), (std::set<GrammarSymbol>{T(")")}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(T("id")), (std::set<GrammarSymbol>{T("id")}));
}

// 测试案例 5: 间接递归和 ε 产生式的复杂组合
// S ::= A C
// A ::= B d | ε
// B ::= S e | f
// C ::= g | ε
TEST_F(FirstSetTest, IndirectRecursionWithEpsilon) {
    ProductionRule rule1(NT("S"), {NT("A"), NT("C")});
    ProductionRule rule2(NT("A"), {NT("B"), T("d")});
    ProductionRule rule3(NT("A"), {}); // A ::= ε
    ProductionRule rule4(NT("B"), {NT("S"), T("e")});
    ProductionRule rule5(NT("B"), {T("f")});
    ProductionRule rule6(NT("C"), {T("g")});
    ProductionRule rule7(NT("C"), {}); // C ::= ε

    std::vector<ProductionRule> rules = {rule1, rule2, rule3, rule4, rule5, rule6, rule7};
    Grammar grammar(rules, NT("S"));

    grammar.calculateFirstSets();

    // 预期结果 (请手动验证这些，因为它们会通过多次迭代和传播来确定)
    // First(S) 应该包含 First(A) 的非 ε 部分，以及如果 A 导出 ε，则包含 First(C) 的非 ε 部分
    // S ::= A C
    // A ::= B d | ε
    // B ::= S e | f
    // C ::= g | ε
    //
    // Iteration 1:
    // First(A) = {f, ε} (from B d, f)
    // First(C) = {g, ε}
    // First(B) = {f} (from f)
    // First(S) = {f, g, ε} (from A C: First(A) - ε U First(C) - ε U ε)
    // First(B) = {f, S_first - ε} -> {f, f, g} = {f, g} (S e)
    //
    // Iteration 2:
    // First(A) = {f, g, ε} (from B d)
    // First(S) = {f, g, ε} (from A C)
    //
    // Final Expected:
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("A")), (std::set<GrammarSymbol>{T("f"), T("g"), Epsilon()}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("B")), (std::set<GrammarSymbol>{T("f"), T("g")}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("C")), (std::set<GrammarSymbol>{T("g"), Epsilon()}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(NT("S")), (std::set<GrammarSymbol>{T("f"), T("g"), Epsilon()}));

    EXPECT_EQ(grammar.getCalculatedFirstSets().at(T("d")), (std::set<GrammarSymbol>{T("d")}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(T("e")), (std::set<GrammarSymbol>{T("e")}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(T("f")), (std::set<GrammarSymbol>{T("f")}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(T("g")), (std::set<GrammarSymbol>{T("g")}));
}

// TODO: 添加更多边缘案例和复杂文法测试，例如：
// - 只包含终结符的规则：A ::= a b c
// - 递归深度非常大的文法
// - 产生式右侧包含多个 ε-推导非终结符的文法：A ::= B C D (B, C, D 都能推导出 ε)