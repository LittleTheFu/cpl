#include "virtualMachine.h"
#include "lexer.h"

int main()
{
    // VirtualMachine vm;

    // vm.loadProgram();
    // vm.execute();

    Lexer lexer("+,{(12222}35-=)if=123<456=789<==!=111\tHELLO*/-\nworld");

    while(auto token = lexer.getNextToken())
    {
        std::cout << tokenTypeToString(token->type) << " : " << token->lexeme << std::endl;

        if(token->type == TokenType::EOF_TOKEN)
        {
            break;
        }
    }

    return 0;
}