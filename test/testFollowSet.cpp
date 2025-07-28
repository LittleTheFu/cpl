#include "gtest/gtest.h"
#include "grammar.h"
#include "grammarSymbol.h" 
#include <vector>
#include <set>
#include <map>

// 定义一个测试套件，继承自 ::testing::Test
class FollowSetTest : public ::testing::Test {
protected:
    // 如果需要任何通用设置或辅助函数，可以在这里添加
};

// --- Test Cases for Follow Sets ---

// 测试案例 1: 简单文法，无 ε 产生式
// S ::= A B
// A ::= a
// B ::= b
TEST_F(FollowSetTest, SimpleGrammar) {
    GrammarSymbol S_nt("S", SymbolType::NonTerminal);
    GrammarSymbol A_nt("A", SymbolType::NonTerminal);
    GrammarSymbol B_nt("B", SymbolType::NonTerminal);
    GrammarSymbol a_t("a", SymbolType::Terminal);
    GrammarSymbol b_t("b", SymbolType::Terminal);
    GrammarSymbol dollar_sym("$", SymbolType::End); // 定义结束标记 $

    ProductionRule rule1(S_nt, {A_nt, B_nt});
    ProductionRule rule2(A_nt, {a_t});
    ProductionRule rule3(B_nt, {b_t});

    std::vector<ProductionRule> rules = {rule1, rule2, rule3};
    Grammar grammar(std::move(rules), S_nt); // S 是起始符号

    // 必须先计算 First 集合，Follow 集合依赖它
    grammar.calculateFirstSets(); 
    grammar.calculateFollowSets();

    // 验证 Follow 集合结果
    // 规则 1: Follow(S) 总是包含 $
    EXPECT_EQ(grammar.getCalculatedFollowSets().at(S_nt), (std::set<GrammarSymbol>{dollar_sym}));

    // 规则 2: S ::= A B. B 在 A 后面。Follow(A) 应该包含 First(B)。
    // First(B) = {b}
    EXPECT_EQ(grammar.getCalculatedFollowSets().at(A_nt), (std::set<GrammarSymbol>{b_t}));

    // 规则 3: S ::= A B. B 是右部最后一个符号。Follow(B) 应该包含 Follow(S)。
    // Follow(S) = {$}
    EXPECT_EQ(grammar.getCalculatedFollowSets().at(B_nt), (std::set<GrammarSymbol>{dollar_sym}));
}

// 测试案例 2: 带有 ε 产生式的文法，验证 ε 传播
// S ::= A B
// A ::= a | ε
// B ::= b | ε
TEST_F(FollowSetTest, GrammarWithEpsilonPropagation) {
    GrammarSymbol S_nt("S", SymbolType::NonTerminal);
    GrammarSymbol A_nt("A", SymbolType::NonTerminal);
    GrammarSymbol B_nt("B", SymbolType::NonTerminal);
    GrammarSymbol a_t("a", SymbolType::Terminal);
    GrammarSymbol b_t("b", SymbolType::Terminal);
    GrammarSymbol epsilon_sym = GrammarSymbol::getEpsilonSymbol(); // ε 符号
    GrammarSymbol dollar_sym("$", SymbolType::End); // 结束标记 $

    ProductionRule rule1(S_nt, {A_nt, B_nt});
    ProductionRule rule2(A_nt, {a_t});
    ProductionRule rule3(A_nt, {}); // A ::= ε
    ProductionRule rule4(B_nt, {b_t});
    ProductionRule rule5(B_nt, {}); // B ::= ε

    std::vector<ProductionRule> rules = {rule1, rule2, rule3, rule4, rule5};
    Grammar grammar(std::move(rules), S_nt);

    grammar.calculateFirstSets();
    grammar.calculateFollowSets();

    // 期望的 First 集合 (为了 Follow Set 验证，先列出来):
    // First(A) = {a, ε}
    // First(B) = {b, ε}
    // First(S) = {a, b, ε}

    // 验证 Follow 集合结果
    // 1. Follow(S) 总是包含 $
    EXPECT_EQ(grammar.getCalculatedFollowSets().at(S_nt), (std::set<GrammarSymbol>{dollar_sym}));

    // 2. S ::= A B
    // B 在 A 后面。Follow(A) 包含 First(B) 中非 ε 的符号。
    // First(B) = {b, ε} => 非 ε 部分是 {b}。
    // 此外，因为 B 可以推导出 ε，所以 Follow(A) 还应该包含 Follow(S)。
    // Follow(A) = {b} U Follow(S) = {b, $}
    EXPECT_EQ(grammar.getCalculatedFollowSets().at(A_nt), (std::set<GrammarSymbol>{b_t, dollar_sym}));

    // 3. S ::= A B
    // B 是右部最后一个符号。Follow(B) 包含 Follow(S)。
    // Follow(B) = Follow(S) = {$}
    EXPECT_EQ(grammar.getCalculatedFollowSets().at(B_nt), (std::set<GrammarSymbol>{dollar_sym}));
}


// 测试案例 3: 带有左递归的算术表达式文法 (E ::= E + T | T)
// E ::= E + T | T
// T ::= T * F | F
// F ::= ( E ) | id
TEST_F(FollowSetTest, GrammarWithLeftRecursionAndChaining) {
    // 定义 GrammarSymbol 实例
    GrammarSymbol E_nt("E", SymbolType::NonTerminal);
    GrammarSymbol T_nt("T", SymbolType::NonTerminal);
    GrammarSymbol F_nt("F", SymbolType::NonTerminal);
    GrammarSymbol plus_t("+", SymbolType::Terminal);
    GrammarSymbol star_t("*", SymbolType::Terminal);
    GrammarSymbol lparen_t("(", SymbolType::Terminal);
    GrammarSymbol rparen_t(")", SymbolType::Terminal);
    GrammarSymbol id_t("id", SymbolType::Terminal);
    GrammarSymbol dollar_sym("$", SymbolType::End); // 结束标记 $

    ProductionRule rule1(E_nt, {E_nt, plus_t, T_nt}); // E ::= E + T
    ProductionRule rule2(E_nt, {T_nt});               // E ::= T
    ProductionRule rule3(T_nt, {T_nt, star_t, F_nt}); // T ::= T * F
    ProductionRule rule4(T_nt, {F_nt});               // T ::= F
    ProductionRule rule5(F_nt, {lparen_t, E_nt, rparen_t}); // F ::= ( E )
    ProductionRule rule6(F_nt, {id_t});               // F ::= id

    std::vector<ProductionRule> rules = {rule1, rule2, rule3, rule4, rule5, rule6};
    Grammar grammar(std::move(rules), E_nt); // E 是起始符号

    grammar.calculateFirstSets(); // 必须先计算 First 集合
    grammar.calculateFollowSets();

    // 期望的 First 集合 (来自之前的 FirstSetTest):
    // First(E) = {(, id}
    // First(T) = {(, id}
    // First(F) = {(, id}
    // First(+) = {+}
    // First(*) = {*}
    // First(() = {(}
    // First()) = {)}
    // First(id) = {id}

    // 验证 Follow 集合结果
    // 1. Follow(E)
    //    a. E 是起始符号: Follow(E) 包含 $
    //    b. F ::= ( E ) : ) 在 E 后面。Follow(E) 包含 First( ) ) = {)}
    //    c. E ::= E + T : T 在 E 后面。Follow(E) 包含 First(+) = {+}
    //    d. T ::= T * F : F 在 T 后面，但 T 之后没有符号。这里没有直接影响 E 的 Follow。
    // 总结: Follow(E) = {$, +, )}
    EXPECT_EQ(grammar.getCalculatedFollowSets().at(E_nt), (std::set<GrammarSymbol>{dollar_sym, plus_t, rparen_t}));

    // 2. Follow(T)
    //    a. E ::= E + T : T 在 E + 后面。Follow(T) 包含 Follow(E)
    //    b. E ::= T : T 是右部最后一个符号。Follow(T) 包含 Follow(E)
    //    c. T ::= T * F : F 在 T * 后面。Follow(T) 包含 First(*) = {*}
    // 总结: Follow(T) = Follow(E) U {*} = {$, +, ), *}
    EXPECT_EQ(grammar.getCalculatedFollowSets().at(T_nt), (std::set<GrammarSymbol>{dollar_sym, plus_t, rparen_t, star_t}));

    // 3. Follow(F)
    //    a. T ::= T * F : F 是右部最后一个符号。Follow(F) 包含 Follow(T)
    //    b. T ::= F : F 是右部最后一个符号。Follow(F) 包含 Follow(T)
    // 总结: Follow(F) = Follow(T) = {$, +, ), *}
    EXPECT_EQ(grammar.getCalculatedFollowSets().at(F_nt), (std::set<GrammarSymbol>{dollar_sym, plus_t, rparen_t, star_t}));
}

// 测试案例 4: 间接递归和 ε 产生式的复杂组合 (与 First Set 的复杂例子相同)
// S ::= A C
// A ::= B d | ε
// B ::= S e | f
// C ::= g | ε
TEST_F(FollowSetTest, IndirectRecursionWithEpsilonComplex) {
    GrammarSymbol S_nt("S", SymbolType::NonTerminal);
    GrammarSymbol A_nt("A", SymbolType::NonTerminal);
    GrammarSymbol B_nt("B", SymbolType::NonTerminal);
    GrammarSymbol C_nt("C", SymbolType::NonTerminal);
    GrammarSymbol d_t("d", SymbolType::Terminal);
    GrammarSymbol e_t("e", SymbolType::Terminal);
    GrammarSymbol f_t("f", SymbolType::Terminal);
    GrammarSymbol g_t("g", SymbolType::Terminal);
    GrammarSymbol epsilon_sym = GrammarSymbol::getEpsilonSymbol(); // ε 符号
    GrammarSymbol dollar_sym("$", SymbolType::End); // 结束标记 $

    ProductionRule rule1(S_nt, {A_nt, C_nt});
    ProductionRule rule2(A_nt, {B_nt, d_t});
    ProductionRule rule3(A_nt, {}); // A ::= ε
    ProductionRule rule4(B_nt, {S_nt, e_t});
    ProductionRule rule5(B_nt, {f_t});
    ProductionRule rule6(C_nt, {g_t});
    ProductionRule rule7(C_nt, {}); // C ::= ε

    std::vector<ProductionRule> rules = {rule1, rule2, rule3, rule4, rule5, rule6, rule7};
    Grammar grammar(std::move(rules), S_nt);

    // 必须先计算 First 集合
    grammar.calculateFirstSets(); 
    grammar.calculateFollowSets();

    // 期望的 First 集合 (来自之前验证的正确结果):
    // First(S) = {f, g, e, ε}
    // First(A) = {f, g, e, ε}
    // First(B) = {f, g, e}
    // First(C) = {g, ε}
    // First(d) = {d}, First(e) = {e}, First(f) = {f}, First(g) = {g}


    // 验证 Follow 集合结果
    // 1. Follow(S)
    //    a. S 是起始符号: Follow(S) 包含 $
    //    b. B ::= S e : e 在 S 后面。Follow(S) 包含 First(e) = {e}
    // 总结: Follow(S) = {$, e}
    EXPECT_EQ(grammar.getCalculatedFollowSets().at(S_nt), (std::set<GrammarSymbol>{dollar_sym, e_t}));

    // 2. Follow(A)
    //    a. S ::= A C : C 在 A 后面。Follow(A) 包含 First(C) 中非 ε 的符号。
    //       First(C) = {g, ε} => 非 ε 部分是 {g}。
    //    b. S ::= A C : 因为 C 可以推导出 ε，Follow(A) 还要包含 Follow(S)。
    // 总结: Follow(A) = {g} U Follow(S) = {g, $, e}
    EXPECT_EQ(grammar.getCalculatedFollowSets().at(A_nt), (std::set<GrammarSymbol>{g_t, dollar_sym, e_t}));

    // 3. Follow(B)
    //    a. A ::= B d : d 在 B 后面。Follow(B) 包含 First(d) = {d}
    // 总结: Follow(B) = {d}
    EXPECT_EQ(grammar.getCalculatedFollowSets().at(B_nt), (std::set<GrammarSymbol>{d_t}));

    // 4. Follow(C)
    //    a. S ::= A C : C 是右部最后一个符号。Follow(C) 包含 Follow(S)。
    // 总结: Follow(C) = Follow(S) = {$, e}
    EXPECT_EQ(grammar.getCalculatedFollowSets().at(C_nt), (std::set<GrammarSymbol>{dollar_sym, e_t}));
}

// TODO: 添加更多边缘案例和复杂文法测试，例如：
// - 只有终结符的规则：A ::= a b c (Follow集相对简单)
// - 复杂的 ε 传播链：X ::= Y Z, Y ::= ε, Z ::= ε (Follow(X)会传递给Follow(Y)和Follow(Z))
// - 文法中存在非终结符永远不会出现在任何产生式右部的情况 (Follow集只有 $)