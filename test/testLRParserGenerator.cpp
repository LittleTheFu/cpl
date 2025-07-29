#include "gtest/gtest.h"
#include "grammar.h"
#include "lrParserGenerator.h"
#include "grammarSymbol.h"
#include "productionRule.h"
#include <vector>
#include <set>

// A test fixture for the LRParserGenerator
class LRParserGeneratorTest : public ::testing::Test {
protected:
    // Define Grammar Symbols
    GrammarSymbol nt_E_prime{"E'", SymbolType::NonTerminal};
    GrammarSymbol nt_E{"E", SymbolType::NonTerminal};
    GrammarSymbol nt_T{"T", SymbolType::NonTerminal};
    GrammarSymbol nt_F{"F", SymbolType::NonTerminal};

    GrammarSymbol t_plus{"+", SymbolType::Terminal};
    GrammarSymbol t_star{"*", SymbolType::Terminal};
    GrammarSymbol t_lp{"(", SymbolType::Terminal};
    GrammarSymbol t_rp{")", SymbolType::Terminal};
    GrammarSymbol t_id{"id", SymbolType::Terminal};
    GrammarSymbol t_dollar{"$", SymbolType::End};

    std::unique_ptr<Grammar> grammar;
    std::unique_ptr<LRParserGenerator> generator;

    void SetUp() override {
        // Define Production Rules
        std::vector<ProductionRule> rules;
        rules.emplace_back(nt_E, std::vector<GrammarSymbol>{nt_E, t_plus, nt_T}); // E -> E + T
        rules.emplace_back(nt_E, std::vector<GrammarSymbol>{nt_T});              // E -> T
        rules.emplace_back(nt_T, std::vector<GrammarSymbol>{nt_T, t_star, nt_F});// T -> T * F
        rules.emplace_back(nt_T, std::vector<GrammarSymbol>{nt_F});              // T -> F
        rules.emplace_back(nt_F, std::vector<GrammarSymbol>{t_lp, nt_E, t_rp});  // F -> ( E )
        rules.emplace_back(nt_F, std::vector<GrammarSymbol>{t_id});              // F -> id

        // Create the grammar object
        grammar = std::make_unique<Grammar>(std::move(rules), nt_E);
        
        // The First and Follow sets must be calculated before generating the parser
        grammar->calculateFirstSets();
        grammar->calculateFollowSets();

        // Create the parser generator
        generator = std::make_unique<LRParserGenerator>(*grammar);
    }
};

// Test case to verify the number of states generated
TEST_F(LRParserGeneratorTest, StateCount) {
    // The classic expression grammar should generate 12 states (I0 to I11)
    EXPECT_EQ(generator->getDFA().size(), 12);
}

// Test case to verify the GOTO table
TEST_F(LRParserGeneratorTest, GotoTable) {
    const auto& goto_table = generator->getGotoTable();

    // GOTO(I0, E) = 1
    EXPECT_EQ(goto_table.at(0).at(nt_E), 1);
    // GOTO(I0, T) = 2
    EXPECT_EQ(goto_table.at(0).at(nt_T), 2);
    // GOTO(I0, F) = 3
    EXPECT_EQ(goto_table.at(0).at(nt_F), 3);

    // GOTO(I4, E) = 8
    EXPECT_EQ(goto_table.at(4).at(nt_E), 8);
    // GOTO(I4, T) = 2
    EXPECT_EQ(goto_table.at(4).at(nt_T), 2);
    // GOTO(I4, F) = 3
    EXPECT_EQ(goto_table.at(4).at(nt_F), 3);

    // GOTO(I6, T) = 9
    EXPECT_EQ(goto_table.at(6).at(nt_T), 9);
    // GOTO(I6, F) = 3
    EXPECT_EQ(goto_table.at(6).at(nt_F), 3);
    
    // GOTO(I7, F) = 10
    EXPECT_EQ(goto_table.at(7).at(nt_F), 10);
}

// Test case to verify the ACTION table
TEST_F(LRParserGeneratorTest, ActionTable) {
    const auto& action_table = generator->getActionTable();

    // State 0
    // ACTION(0, id) = SHIFT 5
    const auto& action_0_id = action_table.at(0).at(t_id);
    EXPECT_EQ(action_0_id.type, ActionType::Shift);
    EXPECT_EQ(action_0_id.shiftStateId, 5);
    // ACTION(0, '(') = SHIFT 4
    const auto& action_0_lp = action_table.at(0).at(t_lp);
    EXPECT_EQ(action_0_lp.type, ActionType::Shift);
    EXPECT_EQ(action_0_lp.shiftStateId, 4);

    // State 1
    // ACTION(1, $) = ACCEPT
    const auto& action_1_dollar = action_table.at(1).at(t_dollar);
    EXPECT_EQ(action_1_dollar.type, ActionType::Accept);

    // State 2
    // ACTION(2, +) = SHIFT 6
    const auto& action_2_plus = action_table.at(2).at(t_plus);
    EXPECT_EQ(action_2_plus.type, ActionType::Shift);
    EXPECT_EQ(action_2_plus.shiftStateId, 6);
    // ACTION(2, ')') = REDUCE by E -> T (rule index 1)
    const auto& action_2_rp = action_table.at(2).at(t_rp);
    EXPECT_EQ(action_2_rp.type, ActionType::Reduce);
    EXPECT_EQ(action_2_rp.productionRuleId, 1); 

    // State 5
    // ACTION(5, *) = REDUCE by F -> id (rule index 5)
    const auto& action_5_star = action_table.at(5).at(t_star);
    EXPECT_EQ(action_5_star.type, ActionType::Reduce);
    EXPECT_EQ(action_5_star.productionRuleId, 5);

    // State 8
    // ACTION(8, ')') = SHIFT 11
    const auto& action_8_rp = action_table.at(8).at(t_rp);
    EXPECT_EQ(action_8_rp.type, ActionType::Shift);
    EXPECT_EQ(action_8_rp.shiftStateId, 11);

    // State 9
    // ACTION(9, $) = REDUCE by E -> E + T (rule index 0)
    const auto& action_9_dollar = action_table.at(9).at(t_dollar);
    EXPECT_EQ(action_9_dollar.type, ActionType::Reduce);
    EXPECT_EQ(action_9_dollar.productionRuleId, 0);

    // State 11
    // ACTION(11, $) = REDUCE by F -> ( E ) (rule index 4)
    const auto& action_11_dollar = action_table.at(11).at(t_dollar);
    EXPECT_EQ(action_11_dollar.type, ActionType::Reduce);
    EXPECT_EQ(action_11_dollar.productionRuleId, 4);
}