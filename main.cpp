#include "token.h"
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
#include "vmCodeGenerator.h"
#include <nlohmann/json.hpp>
#include <csignal>
#include "network/socket_server_module.h"



std::string handle_request(const std::string& request_payload, Parser& parser, VmCodeGenerator& code_gen, VirtualMachine& vm) {
    std::cout << "Received request: " << request_payload << std::endl;
    return "hello";

    // nlohmann::json root;
    // try {
    //     root = nlohmann::json::parse(request_payload);
    // } catch (const nlohmann::json::parse_error& e) {
    //     return "{\"status\": \"error\", \"message\": \"Invalid JSON request: " + std::string(e.what()) + "\"}";
    // }

    // std::string command;
    // if (root.contains("command")) {
    //     command = root["command"].get<std::string>();
    // } else {
    //     return "{\"status\": \"error\", \"message\": \"Request missing 'command' field.\"}";
    // }

    // if (command == "compile") {
    //     std::string source_code;
    //     if (root.contains("code")) {
    //         source_code = root["code"].get<std::string>();
    //     } else {
    //         return "{\"status\": \"error\", \"message\": \"'compile' command missing 'code' field.\"}";
    //     }
    //     std::cout << "Compiling code: " << source_code.substr(0, std::min(source_code.length(), (size_t)50)) << "..." << std::endl;

    //     try {
    //         // 编译逻辑
    //         Lexer lexer(source_code);
    //         std::vector<Token> tokens;
    //         while(auto token = lexer.getNextToken()) {
    //             tokens.push_back(*token);
    //             if(token->type == TokenType::EOF_TOKEN) break;
    //         }
    //         if (tokens.empty() || tokens.back().type != TokenType::EOF_TOKEN) {
    //             tokens.push_back(Token{TokenType::EOF_TOKEN, ""});
    //         }

    //         parser.parse(tokens);
    //         auto ir_program = parser.getIRProgram();
    //         auto vm_instructions = code_gen.translate(ir_program.getInstructions());
    //         vm.loadProgram(vm_instructions); // 将编译好的指令加载到VM

    //         nlohmann::json assembly_array = nlohmann::json::array();
    //         for (const auto& instr : vm_instructions) {
    //             assembly_array.push_back(instr.toString()); // 使用 Instruction::toString()
    //         }

    //         nlohmann::json response;
    //         response["status"] = "success";
    //         response["type"] = "compiled_code";
    //         response["assembly_lines"] = assembly_array; // 汇编代码作为字符串数组
    //         response["message"] = "Code compiled successfully.";
    //         return response.dump(); // dump() 生成紧凑的JSON字符串
    //     } catch (const std::exception& e) {
    //         nlohmann::json response;
    //         response["status"] = "error";
    //         response["message"] = std::string("Compilation error: ") + e.what();
    //         return response.dump();
    //     }
    // } else if (command == "vm_step") {
    //     if (!vm.isLoaded()) {
    //          return "{\"status\": \"error\", \"message\": \"VM program not loaded.\"}";
    //     }
    //     if (vm.isHalted()) {
    //          return "{\"status\": \"success\", \"type\": \"vm_state\", \"pc\": " + std::to_string(vm.getProgramCounter()) + ", \"halted\": true, \"message\": \"VM already halted.\"}";
    //     }

    //     int vm_result = vm.executeSingleInstruction(); // 0 for continue, 1 for halt
    //     bool halted = (vm_result == 1);

    //     nlohmann::json response;
    //     response["status"] = "success";
    //     response["type"] = "vm_state";
    //     response["pc"] = vm.getProgramCounter(); // 程序计数器
    //     response["halted"] = halted;

    //     nlohmann::json registers_array = nlohmann::json::array();
    //     for(int i = 0; i < vm.getNumberOfRegisters(); ++i) { // 假设VM有获取寄存器数量的方法
    //         registers_array.push_back(vm.getRegisterValue(i));
    //     }
    //     response["registers"] = registers_array;

    //     // 内存状态可以根据你的VM设计来获取和返回
    //     // 比如，返回特定范围的内存数据，或者只返回已使用的内存区域
    //     nlohmann::json memory_map = nlohmann::json::object();
    //     // 示例：返回一些关键的内存地址或数据段
    //     // 你需要根据你的VirtualMachine类实际的内存访问方法来填充
    //     // 例如，如果你的VM有 getMemoryRegion(start_addr, size)
    //     // 或者 getMemoryMap() 返回一个 map<int, int>
        
    //     // 假设你有一个方法可以获取部分内存
    //     // For demonstration, let's just get a few specific memory cells
    //     // In a real scenario, you might want to return a range or specific named variables
    //     // if (vm.getMemorySize() > 0) { // 假设VM有内存大小
    //     //     for (int i = 0; i < std::min(vm.getMemorySize(), 100); ++i) { // 只返回前100个字节作为示例
    //     //         memory_map[std::to_string(i)] = vm.getMemoryValue(i);
    //     //     }
    //     // }
        
    //     // 更常见和有用的方法是返回局部变量、栈帧等。
    //     // 这取决于你的VM状态如何导出。
    //     // 例如，你可以设计一个 `vm.getDebugState()` 方法，返回一个包含
    //     // PC, 寄存器, 栈顶, 局部变量映射等所有调试信息的 JSON 对象
        
    //     response["memory"] = memory_map; // 示例为空，你需要根据VM实现来填充

    //     response["message"] = halted ? "VM halted." : "VM stepped.";
    //     return response.dump();
    // } else if (command == "vm_reset") {
    //     vm.reset();
    //     return "{\"status\": \"success\", \"message\": \"VM reset.\" }";
    // } else if (command == "shutdown") {
    //     // 收到关闭命令，调用 SocketServer::stop()
    //     // 注意：此响应可能不会被客户端接收到，因为服务器会立即关闭
    //     SocketServer::stop();
    //     return "{\"status\": \"success\", \"message\": \"Server shutting down.\" }";
    // }

    // nlohmann::json response;
    // response["status"] = "error";
    // response["message"] = "Unknown command: " + command;
    // return response.dump();
}

// 信号处理函数，用于捕获 Ctrl+C (SIGINT) 等信号
void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\nSIGINT (Ctrl+C) received. Shutting down server..." << std::endl;
        SocketServer::stop(); // 调用 SocketServer 的停止方法
    }
}

int main()
{
    // VirtualMachine vm;

    // vm.loadProgram();
    // vm.execute();

    Lexer lexer("(3+2)*8*9+11 * 123");
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

    std::cout << "--------------------------" << std::endl;

    VmCodeGenerator codeGenerator;
    std::vector<Instruction> instructions = codeGenerator.translate(parser.getIRProgram().getInstructions());
    for (const auto& instruction : instructions)
    {
        std::cout << instruction.toString() << std::endl;
    }

    VirtualMachine vm;

    //----------------------------------------------------
    // 启动 Socket 服务器
    int port = 12345;
    SocketServer::start(port, handle_request, parser, codeGenerator, vm);

    std::cout << "Server running. Send {\"command\": \"shutdown\"} to stop, or press Ctrl+C." << std::endl;
    // 阻塞 main 线程，等待服务器停止
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 用于命令行测试

    std::cout << "Compiler backend shut down gracefully." << std::endl;



    return 0;
}