#include "virtualMachine.h"
#include "lexer.h"
#include "grammarSymbol.h"
#include "predefineSymbol.h"
#include "productionRule.h"
#include "binaryOpNode.h"
#include <memory>
#include "stackItem.h"
#include "grammar.h"
#include "parser.h"
#include "intergerLiteralNode.h"
#include "identifierNode.h"

int main()
{
    // VirtualMachine vm;

    // vm.loadProgram();
    // vm.execute();

    Lexer lexer("(3+2)*8*9+alice * bob");
    std::vector<Token> tokens;

    while(auto token = lexer.getNextToken())
    {
        std::cout << tokenTypeToString(token->type) << " : " << token->lexeme << std::endl;
        tokens.push_back(*token);

        if(token->type == TokenType::EOF_TOKEN)
        {
            break;
        }
    }
    tokens.push_back(Token{TokenType::EOF_TOKEN, ""});

    // Expr   ::= Expr '+' Term
    //         | Term

    // Term   ::= Term '*' Factor
    //         | Factor

    // Factor ::= '(' Expr ')'
    //         | Identifier     // 变量，如 'x', 'y'
    //         | IntegerConstant // 数字，如 '123'

    // namespace PredefineSymbol
    // {
    //     const GrammarSymbol SYMBOL_END = GrammarSymbol("$", SymbolType::End);
    //     const GrammarSymbol SYMBOL_PLUS = GrammarSymbol("+", SymbolType::Terminal);
    //     const GrammarSymbol SYMBOL_MINUS = GrammarSymbol("-", SymbolType::Terminal);
    //     const GrammarSymbol SYMBOL_MULTIPLY = GrammarSymbol("*", SymbolType::Terminal);
    //     const GrammarSymbol SYMBOL_DIVIDE = GrammarSymbol("/", SymbolType::Terminal);
    //     const GrammarSymbol SYMBOL_IDENTIFIER = GrammarSymbol("id", SymbolType::Terminal);
    //     const GrammarSymbol SYMBOL_NUMBER = GrammarSymbol("number", SymbolType::Terminal);
    //     const GrammarSymbol SYMBOL_ASSIGN = GrammarSymbol("=", SymbolType::Terminal);
    //     const GrammarSymbol SYMBOL_STACK_BOTTOM = GrammarSymbol("stack bottom", SymbolType::StackBottom);
    //     const GrammarSymbol LEFT_PAREN = GrammarSymbol("(", SymbolType::Terminal);
    //     const GrammarSymbol RIGHT_PAREN = GrammarSymbol(")", SymbolType::Terminal);
    // }

    GrammarSymbol nt_Expr{"Expr", SymbolType::NonTerminal};
    GrammarSymbol nt_Term{"Term", SymbolType::NonTerminal};
    GrammarSymbol nt_Factor{"Factor", SymbolType::NonTerminal};

    std::vector<ProductionRule> rules;

    rules.emplace_back(nt_Expr,
                       std::vector<GrammarSymbol>{nt_Expr, PredefineSymbol::SYMBOL_PLUS, nt_Term},
                       [](std::vector<StackItem> &&stackItems) -> std::unique_ptr<AstNode>
                       {
                           if (stackItems.size() != 3)
                           {
                               throw std::runtime_error("Expr production rule error");
                           }

                           return std::make_unique<BinaryOpNode>(
                               std::move(std::get<std::unique_ptr<AstNode>>(stackItems[0].value)),
                               std::move(std::get<std::unique_ptr<AstNode>>(stackItems[2].value)),
                               BinaryOpType::ADD);
                       });

    rules.emplace_back(nt_Expr,
                       std::vector<GrammarSymbol>{nt_Term},
                       [](std::vector<StackItem> &&stackItems) -> std::unique_ptr<AstNode>
                       {
                           if (stackItems.size() != 1)
                           {
                               throw std::runtime_error("Expr production rule error");
                           }

                           return std::move(std::get<std::unique_ptr<AstNode>>(stackItems[0].value));
                       });

    rules.emplace_back(nt_Term,
                       std::vector<GrammarSymbol>{nt_Term, PredefineSymbol::SYMBOL_MULTIPLY, nt_Factor},
                       [](std::vector<StackItem> &&stackItems) -> std::unique_ptr<AstNode>
                       {
                           if (stackItems.size() != 3)
                           {
                               throw std::runtime_error("Term production rule error");
                           }

                           return std::make_unique<BinaryOpNode>(
                               std::move(std::get<std::unique_ptr<AstNode>>(stackItems[0].value)),
                               std::move(std::get<std::unique_ptr<AstNode>>(stackItems[2].value)),
                               BinaryOpType::MULTIPLY);
                       });

    rules.emplace_back(nt_Term,
                       std::vector<GrammarSymbol>{nt_Factor},
                       [](std::vector<StackItem> &&stackItems) -> std::unique_ptr<AstNode>
                       {
                           if (stackItems.size() != 1)
                           {
                               throw std::runtime_error("Term production rule error");
                           }

                           return std::move(std::get<std::unique_ptr<AstNode>>(stackItems[0].value));
                       });

    rules.emplace_back(nt_Factor,
                       std::vector<GrammarSymbol>{PredefineSymbol::LEFT_PAREN, nt_Expr, PredefineSymbol::RIGHT_PAREN},
                       [](std::vector<StackItem> &&stackItems) -> std::unique_ptr<AstNode>
                       {
                           if (stackItems.size() != 3)
                           {
                               throw std::runtime_error("Factor production rule error");
                           }

                           return std::move(std::get<std::unique_ptr<AstNode>>(stackItems[1].value));
                       });

    rules.emplace_back(nt_Factor,
                       std::vector<GrammarSymbol>{PredefineSymbol::SYMBOL_IDENTIFIER},
                       [](std::vector<StackItem> &&stackItems) -> std::unique_ptr<AstNode>
                       {
                           if (stackItems.size() != 1)
                           {
                               throw std::runtime_error("Factor production rule error");
                           }

                           return std::move(std::make_unique<IdentifierNode>(std::get<Token>(stackItems[0].value).lexeme));
                       });

    rules.emplace_back(nt_Factor,
                       std::vector<GrammarSymbol>{PredefineSymbol::SYMBOL_NUMBER},
                       [](std::vector<StackItem> &&stackItems) -> std::unique_ptr<AstNode>
                       {
                           if (stackItems.size() != 1)
                           {
                               throw std::runtime_error("Factor production rule error");
                           }

                           return std::move(std::make_unique<IntegerLiteralNode>(std::stoi(std::get<Token>(stackItems[0].value).lexeme)));
                       });

    Grammar grammar(std::move(rules), nt_Expr);

    grammar.calculateFirstSets();
    grammar.calculateFollowSets();

    Parser parser(grammar);
    parser.parse(tokens);

    parser.getIRProgram().print();

    return 0;
}