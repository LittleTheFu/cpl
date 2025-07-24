#include "virtualMachine.h"
#include "lexer.h"

int main()
{
    // VirtualMachine vm;

    // vm.loadProgram();
    // vm.execute();

    Lexer lexer("+123<456=789<=111\tHELLO*/-\nworld");

    while(auto token = lexer.getNextToken())
    {
        std::cout << tokenTypeToString(token->type) << std::endl;

        if(token->type == TokenType::EOF_TOKEN)
        {
            break;
        }
    }

    return 0;
}