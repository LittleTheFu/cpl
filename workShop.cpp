#include "workShop.h"
#include "predefineSymbol.h"
#include "binaryOpNode.h"
#include "identifierNode.h"
#include "intergerLiteralNode.h"
#include "vmCodeGenerator.h"
#include "parser.h"
#include "lrParserGenerator.h"
#include "grammar.h"
#include "stackItem.h"

WorkShop::WorkShop()
{

}

WorkShop& WorkShop::getInstance()
{
    static WorkShop instance;
    return instance;
}

std::vector<std::string> WorkShop::loadProgram(const std::string &source)
{
    Lexer lexer(source);
    std::vector<Token> tokens;

    while(auto token = lexer.getNextToken())
    {
        std::cout << tokenTypeToString(token->type) << " : " << token->lexeme << std::endl;
        tokens.push_back(*token);

        if(token->type == _TokenType_::EOF_TOKEN)
        {
            break;
        }
    }
    tokens.push_back(Token{_TokenType_::EOF_TOKEN, ""});
    
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

    std::cout << "--------------------------" << std::endl;

    VmCodeGenerator codeGenerator;
    std::vector<Instruction> instructions = codeGenerator.translate(parser.getIRProgram().getInstructions());
    std::string src;
    for (const auto& instruction : instructions)
    {
        src += instruction.toString() + "\n";
        std::cout << instruction.toString() << std::endl;
    }

    vm_.loadProgram(src);

    return vm_.getSourceCode();
}

std::vector<std::string> WorkShop::getVMSrc()
{
    return vm_.getSourceCode();
}

size_t WorkShop::getPC() const
{
    return vm_.getProgramCounter();
}

void WorkShop::step()
{
    vm_.step();
}

void WorkShop::resetProgram()
{
    vm_.resetProgram();
}

const std::vector<int> &WorkShop::getVMRegisters() const
{
    return vm_.getRegisters();
}

const std::vector<int> &WorkShop::getVMMemory() const
{
    return vm_.getMemory();
}

bool WorkShop::getVMZeroFlag() const
{
    return vm_.getZeroFlag();
}

bool WorkShop::getVMSignFlag() const
{
    return vm_.getSignFlag();
}
