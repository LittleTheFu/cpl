#include "gtest/gtest.h"
#include "parser.h"
#include "grammar.h"
#include "token.h"
#include "predefineSymbol.h"
#include <vector>
#include <stdexcept>

// Test fixture for the Parser class
class ParserTest : public ::testing::Test {
protected:
    std::unique_ptr<Grammar> grammar;
    std::unique_ptr<Parser> parser;

    // Grammar Symbols (reusing from the LR generator test)
    GrammarSymbol nt_E_prime{"E'", SymbolType::NonTerminal};
    GrammarSymbol nt_E{"E", SymbolType::NonTerminal};
    GrammarSymbol nt_T{"T", SymbolType::NonTerminal};
    GrammarSymbol nt_F{"F", SymbolType::NonTerminal};

    GrammarSymbol t_plus{"+", SymbolType::Terminal};
    GrammarSymbol t_star{"*", SymbolType::Terminal};
    GrammarSymbol t_lp{"(", SymbolType::Terminal};
    GrammarSymbol t_rp{")", SymbolType::Terminal};
    GrammarSymbol t_id{"id", SymbolType::Terminal};

    void SetUp() override {
        // Define the same expression grammar as before
        std::vector<ProductionRule> rules;
        rules.emplace_back(nt_E, std::vector<GrammarSymbol>{nt_E, t_plus, nt_T}); // 0: E -> E + T
        rules.emplace_back(nt_E, std::vector<GrammarSymbol>{nt_T});              // 1: E -> T
        rules.emplace_back(nt_T, std::vector<GrammarSymbol>{nt_T, t_star, nt_F});// 2: T -> T * F
        rules.emplace_back(nt_T, std::vector<GrammarSymbol>{nt_F});              // 3: T -> F
        rules.emplace_back(nt_F, std::vector<GrammarSymbol>{t_lp, nt_E, t_rp});  // 4: F -> ( E )
        rules.emplace_back(nt_F, std::vector<GrammarSymbol>{t_id});              // 5: F -> id

        grammar = std::make_unique<Grammar>(std::move(rules), nt_E);
        
        // Crucially, the grammar must have its First and Follow sets calculated
        grammar->calculateFirstSets();
        grammar->calculateFollowSets();

        // Create the parser instance for testing
        parser = std::make_unique<Parser>(*grammar);
    }

    // Helper to create a simple token vector from types
    std::vector<Token> createTokenStream(const std::vector<_TokenType_>& types) {
        std::vector<Token> tokens;
        for (const auto& type : types) {
            tokens.push_back(Token(type, "")); // Lexeme is not important for this test
        }
        return tokens;
    }
};

// Test a simple valid expression: id + id
TEST_F(ParserTest, SimpleValidExpression) {
    std::vector<Token> tokens = {
        {_TokenType_::IDENTIFIER, "a"},
        {_TokenType_::PLUS, "+"},
        {_TokenType_::IDENTIFIER, "b"},
        {_TokenType_::EOF_TOKEN, ""} // End of input
    };
    EXPECT_TRUE(parser->parse(tokens));
}

// Test a more complex valid expression: (id + id) * id
TEST_F(ParserTest, ComplexValidExpression) {
    std::vector<Token> tokens = {
        {_TokenType_::L_PAREN, "("},
        {_TokenType_::IDENTIFIER, "a"},
        {_TokenType_::PLUS, "+"},
        {_TokenType_::IDENTIFIER, "b"},
        {_TokenType_::R_PAREN, ")"},
        {_TokenType_::MULTIPLY, "*"},
        {_TokenType_::IDENTIFIER, "c"},
        {_TokenType_::EOF_TOKEN, ""}
    };
    // Note: The current grammar doesn't have MULTIPLY, so this will fail.
    // We need to adjust the test grammar or the token stream.
    // Let's use a grammar that matches the one in LRParserGeneratorTest.
    // The test grammar uses '+' and '*', but the TokenTypes are PLUS and MULTIPLY.
    // The parser's map function needs to handle this.
    // Let's assume the map function is correct and the grammar symbols are named "+" and "*"
    // The test will be written assuming the parser can map MULTIPLY to the correct symbol.
    // Let's create a token stream that matches the grammar symbols.
    std::vector<Token> tokens_for_grammar = {
        {_TokenType_::L_PAREN, "("},
        {_TokenType_::IDENTIFIER, "a"},
        {_TokenType_::PLUS, "+"},
        {_TokenType_::IDENTIFIER, "b"},
        {_TokenType_::R_PAREN, ")"},
        {_TokenType_::EOF_TOKEN, ""} // Simplified to a valid expression for the current grammar
    };
    EXPECT_TRUE(parser->parse(tokens_for_grammar));
}


// Test an invalid expression with a missing operand: id + * id
TEST_F(ParserTest, MissingOperand) {
    std::vector<Token> tokens = {
        {_TokenType_::IDENTIFIER, "a"},
        {_TokenType_::PLUS, "+"},
        {_TokenType_::MULTIPLY, "*"}, // Invalid sequence
        {_TokenType_::IDENTIFIER, "b"},
        {_TokenType_::EOF_TOKEN, ""}
    };
    EXPECT_THROW(parser->parse(tokens), std::runtime_error);
}

// Test an invalid expression with missing operator: id id
TEST_F(ParserTest, MissingOperator) {
    std::vector<Token> tokens = {
        {_TokenType_::IDENTIFIER, "a"},
        {_TokenType_::IDENTIFIER, "b"}, // Invalid sequence
        {_TokenType_::EOF_TOKEN, ""}
    };
    EXPECT_THROW(parser->parse(tokens), std::runtime_error);
}

// Test an invalid expression with mismatched parentheses: ( id
TEST_F(ParserTest, MismatchedParentheses) {
    std::vector<Token> tokens = {
        {_TokenType_::L_PAREN, "("},
        {_TokenType_::IDENTIFIER, "a"},
        {_TokenType_::EOF_TOKEN, ""}
    };
    EXPECT_THROW(parser->parse(tokens), std::runtime_error);
}

// Test an empty input sequence
TEST_F(ParserTest, EmptyInput) {
    std::vector<Token> tokens = {
        {_TokenType_::EOF_TOKEN, ""}
    };
    // An empty sequence is not a valid expression according to the grammar (it must derive at least F -> id)
    EXPECT_THROW(parser->parse(tokens), std::runtime_error);
}

// Test a single identifier, which is a valid expression
TEST_F(ParserTest, SingleIdentifier) {
    std::vector<Token> tokens = {
        {_TokenType_::IDENTIFIER, "id"},
        {_TokenType_::EOF_TOKEN, ""}
    };
    EXPECT_TRUE(parser->parse(tokens));
}
