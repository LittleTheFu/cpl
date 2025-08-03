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
#include <csignal>


int main()
{
    VirtualMachine vm;
    vm.loadProgram();
    std::vector<std::string> src = vm.toString();

    return 0;
}