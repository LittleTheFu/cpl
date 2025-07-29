#include "gtest/gtest.h"
#include "parser.h"
#include "grammar.h"
#include "token.h"
#include "predefineSymbol.h"
#include <vector>
#include <stdexcept>

// A test fixture for the Parser with a minimal grammar.
class SimpleParserTest : public ::testing::Test {
protected:
    std::unique_ptr<Grammar> grammar;
    std::unique_ptr<Parser> parser;

    // Grammar: S -> id
    GrammarSymbol nt_S{"S", SymbolType::NonTerminal};
    GrammarSymbol t_id{"id", SymbolType::Terminal};

    void SetUp() override {
        // 1. Define the single production rule: S -> id
        std::vector<ProductionRule> rules;
        rules.emplace_back(nt_S, std::vector<GrammarSymbol>{t_id});

        // 2. Create the grammar object with S as the start symbol
        grammar = std::make_unique<Grammar>(std::move(rules), nt_S);
        
        // 3. The grammar's First and Follow sets MUST be calculated for the generator to work.
        grammar->calculateFirstSets();
        grammar->calculateFollowSets();

        // 4. Create the parser instance for this simple grammar.
        parser = std::make_unique<Parser>(*grammar);
    }
};

// Test Case 1: A single valid token sequence ("id")
// This is the only valid program for this grammar.
TEST_F(SimpleParserTest, ValidSequence) {
    std::vector<Token> tokens = {
        {TokenType::IDENTIFIER, "some_var"}, // The parser's map() will convert this to the 'id' symbol
        {TokenType::EOF_TOKEN, ""}
    };
    // The parse should succeed and return true.
    EXPECT_TRUE(parser->parse(tokens));
}

// // Test Case 2: An invalid sequence with too many tokens ("id id")
// TEST_F(SimpleParserTest, InvalidSequence_TooLong) {
//     std::vector<Token> tokens = {
//         {TokenType::IDENTIFIER, "a"},
//         {TokenType::IDENTIFIER, "b"}, // The second identifier is unexpected.
//         {TokenType::EOF_TOKEN, ""}
//     };
//     // The parser should find an error and throw an exception.
//     EXPECT_THROW(parser->parse(tokens), std::runtime_error);
// }

// // Test Case 3: An empty input sequence
// // The grammar cannot produce an empty string, so this must fail.
// TEST_F(SimpleParserTest, InvalidSequence_Empty) {
//     std::vector<Token> tokens = {
//         {TokenType::EOF_TOKEN, ""}
//     };
//     // The parser should throw an exception.
//     EXPECT_THROW(parser->parse(tokens), std::runtime_error);
// }

// // Test Case 4: An invalid token type
// TEST_F(SimpleParserTest, InvalidSequence_WrongToken) {
//     std::vector<Token> tokens = {
//         {TokenType::PLUS, "+"}, // The grammar does not expect a '+' symbol.
//         {TokenType::EOF_TOKEN, ""}
//     };
//     // The parser should throw an exception.
//     EXPECT_THROW(parser->parse(tokens), std::runtime_error);
// }
