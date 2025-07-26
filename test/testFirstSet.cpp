#include "gtest/gtest.h"
#include "grammar.h" // 确保这是你的 Grammar 类头文件的正确路径
// 确保 GrammarSymbol.h 在这里被包含，因为我们需要直接使用 GrammarSymbol 构造函数
#include "grammarSymbol.h" 
#include <vector>
#include <set>
#include <map>


// 定义一个测试套件，继承自 ::testing::Test
// 辅助函数已移除
class FirstSetTest : public ::testing::Test {
protected:
    // 辅助函数已全部移除，直接使用 GrammarSymbol 构造函数
};

// --- Test Cases for First Sets ---

// 测试案例 1: 简单文法，无 ε 产生式
// S ::= A B
// A ::= a
// B ::= b
TEST_F(FirstSetTest, SimpleGrammar) {
    // 定义 GrammarSymbol 实例
    GrammarSymbol S_nt("S", SymbolType::NonTerminal);
    GrammarSymbol A_nt("A", SymbolType::NonTerminal);
    GrammarSymbol B_nt("B", SymbolType::NonTerminal);
    GrammarSymbol a_t("a", SymbolType::Terminal);
    GrammarSymbol b_t("b", SymbolType::Terminal);

    // 定义产生式规则
    ProductionRule rule1(S_nt, {A_nt, B_nt});
    ProductionRule rule2(A_nt, {a_t});
    ProductionRule rule3(B_nt, {b_t});

    std::vector<ProductionRule> rules = {rule1, rule2, rule3};
    Grammar grammar(rules, S_nt); // 以 S 为起始符号

    // 计算 First 集合
    grammar.calculateFirstSets();

    // 验证 First 集合结果
    // 对于非终结符
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(S_nt), (std::set<GrammarSymbol>{a_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(A_nt), (std::set<GrammarSymbol>{a_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(B_nt), (std::set<GrammarSymbol>{b_t}));
    // 对于终结符 (First(Terminal) = {Terminal} )
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(a_t), (std::set<GrammarSymbol>{a_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(b_t), (std::set<GrammarSymbol>{b_t}));
}

// 测试案例 2: 带有 ε 产生式的文法，验证 ε 传播
// S ::= A B
// A ::= a | ε
// B ::= b
TEST_F(FirstSetTest, GrammarWithEpsilon) {
    // 定义 GrammarSymbol 实例
    GrammarSymbol S_nt("S", SymbolType::NonTerminal);
    GrammarSymbol A_nt("A", SymbolType::NonTerminal);
    GrammarSymbol B_nt("B", SymbolType::NonTerminal);
    GrammarSymbol a_t("a", SymbolType::Terminal);
    GrammarSymbol b_t("b", SymbolType::Terminal);
    GrammarSymbol epsilon_sym = GrammarSymbol::getEpsilonSymbol();

    ProductionRule rule1(S_nt, {A_nt, B_nt});
    ProductionRule rule2(A_nt, {a_t});
    ProductionRule rule3(A_nt, {}); // A ::= ε
    ProductionRule rule4(B_nt, {b_t});

    std::vector<ProductionRule> rules = {rule1, rule2, rule3, rule4};
    Grammar grammar(rules, S_nt);

    grammar.calculateFirstSets();

    EXPECT_EQ(grammar.getCalculatedFirstSets().at(S_nt), (std::set<GrammarSymbol>{a_t, b_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(A_nt), (std::set<GrammarSymbol>{a_t, epsilon_sym}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(B_nt), (std::set<GrammarSymbol>{b_t}));
}

// 测试案例 3: 所有非终结符都能推导出 ε
// S ::= A B
// A ::= x | ε
// B ::= y | ε
TEST_F(FirstSetTest, AllCanDeriveEpsilon) {
    // 定义 GrammarSymbol 实例
    GrammarSymbol S_nt("S", SymbolType::NonTerminal);
    GrammarSymbol A_nt("A", SymbolType::NonTerminal);
    GrammarSymbol B_nt("B", SymbolType::NonTerminal);
    GrammarSymbol x_t("x", SymbolType::Terminal);
    GrammarSymbol y_t("y", SymbolType::Terminal);
    GrammarSymbol epsilon_sym = GrammarSymbol::getEpsilonSymbol();

    ProductionRule rule1(S_nt, {A_nt, B_nt});
    ProductionRule rule2(A_nt, {x_t});
    ProductionRule rule3(A_nt, {}); // A ::= ε
    ProductionRule rule4(B_nt, {y_t});
    ProductionRule rule5(B_nt, {}); // B ::= ε

    std::vector<ProductionRule> rules = {rule1, rule2, rule3, rule4, rule5};
    Grammar grammar(rules, S_nt);

    grammar.calculateFirstSets();

    EXPECT_EQ(grammar.getCalculatedFirstSets().at(S_nt), (std::set<GrammarSymbol>{x_t, y_t, epsilon_sym}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(A_nt), (std::set<GrammarSymbol>{x_t, epsilon_sym}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(B_nt), (std::set<GrammarSymbol>{y_t, epsilon_sym}));
}

// 测试案例 4: 带有左递归的文法 (E ::= E + T | T)
// E ::= E + T | T
// T ::= T * F | F
// F ::= ( E ) | id
TEST_F(FirstSetTest, GrammarWithLeftRecursion) {
    // 定义 GrammarSymbol 实例
    GrammarSymbol E_nt("E", SymbolType::NonTerminal);
    GrammarSymbol T_nt("T", SymbolType::NonTerminal);
    GrammarSymbol F_nt("F", SymbolType::NonTerminal);
    GrammarSymbol plus_t("+", SymbolType::Terminal);
    GrammarSymbol star_t("*", SymbolType::Terminal);
    GrammarSymbol lparen_t("(", SymbolType::Terminal);
    GrammarSymbol rparen_t(")", SymbolType::Terminal);
    GrammarSymbol id_t("id", SymbolType::Terminal);

    ProductionRule rule1(E_nt, {E_nt, plus_t, T_nt});
    ProductionRule rule2(E_nt, {T_nt});
    ProductionRule rule3(T_nt, {T_nt, star_t, F_nt});
    ProductionRule rule4(T_nt, {F_nt});
    ProductionRule rule5(F_nt, {lparen_t, E_nt, rparen_t});
    ProductionRule rule6(F_nt, {id_t});

    std::vector<ProductionRule> rules = {rule1, rule2, rule3, rule4, rule5, rule6};
    Grammar grammar(rules, E_nt);

    grammar.calculateFirstSets();

    EXPECT_EQ(grammar.getCalculatedFirstSets().at(E_nt), (std::set<GrammarSymbol>{lparen_t, id_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(T_nt), (std::set<GrammarSymbol>{lparen_t, id_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(F_nt), (std::set<GrammarSymbol>{lparen_t, id_t}));

    EXPECT_EQ(grammar.getCalculatedFirstSets().at(plus_t), (std::set<GrammarSymbol>{plus_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(star_t), (std::set<GrammarSymbol>{star_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(lparen_t), (std::set<GrammarSymbol>{lparen_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(rparen_t), (std::set<GrammarSymbol>{rparen_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(id_t), (std::set<GrammarSymbol>{id_t}));
}

// 测试案例 5: 间接递归和 ε 产生式的复杂组合
// S ::= A C
// A ::= B d | ε
// B ::= S e | f
// C ::= g | ε
TEST_F(FirstSetTest, IndirectRecursionWithEpsilon) {
    // 定义 GrammarSymbol 实例
    GrammarSymbol S_nt("S", SymbolType::NonTerminal);
    GrammarSymbol A_nt("A", SymbolType::NonTerminal);
    GrammarSymbol B_nt("B", SymbolType::NonTerminal);
    GrammarSymbol C_nt("C", SymbolType::NonTerminal);
    GrammarSymbol d_t("d", SymbolType::Terminal);
    GrammarSymbol e_t("e", SymbolType::Terminal);
    GrammarSymbol f_t("f", SymbolType::Terminal);
    GrammarSymbol g_t("g", SymbolType::Terminal);
    GrammarSymbol epsilon_sym = GrammarSymbol::getEpsilonSymbol();

    ProductionRule rule1(S_nt, {A_nt, C_nt});
    ProductionRule rule2(A_nt, {B_nt, d_t});
    ProductionRule rule3(A_nt, {}); // A ::= ε
    ProductionRule rule4(B_nt, {S_nt, e_t});
    ProductionRule rule5(B_nt, {f_t});
    ProductionRule rule6(C_nt, {g_t});
    ProductionRule rule7(C_nt, {}); // C ::= ε

    std::vector<ProductionRule> rules = {rule1, rule2, rule3, rule4, rule5, rule6, rule7};
    Grammar grammar(rules, S_nt);

    grammar.calculateFirstSets();

    EXPECT_EQ(grammar.getCalculatedFirstSets().at(A_nt), (std::set<GrammarSymbol>{f_t, g_t, epsilon_sym}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(B_nt), (std::set<GrammarSymbol>{f_t, g_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(C_nt), (std::set<GrammarSymbol>{g_t, epsilon_sym}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(S_nt), (std::set<GrammarSymbol>{f_t, g_t, epsilon_sym}));

    EXPECT_EQ(grammar.getCalculatedFirstSets().at(d_t), (std::set<GrammarSymbol>{d_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(e_t), (std::set<GrammarSymbol>{e_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(f_t), (std::set<GrammarSymbol>{f_t}));
    EXPECT_EQ(grammar.getCalculatedFirstSets().at(g_t), (std::set<GrammarSymbol>{g_t}));
}

// TODO: 添加更多边缘案例和复杂文法测试，例如：
// - 只包含终结符的规则：A ::= a b c
// - 递归深度非常大的文法
// - 产生式右侧包含多个 ε-推导非终结符的文法：A ::= B C D (B, C, D 都能推导出 ε)