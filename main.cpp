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
#include "workShop.h"


int main()
{
    std::vector<std::string> src = WorkShop::getInstance().getVMSrc();

    return 0;
}