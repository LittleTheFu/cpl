#include "workShop.h"

WorkShop::WorkShop()
{
    vm_.loadProgram();
}

WorkShop& WorkShop::getInstance()
{
    static WorkShop instance;
    return instance;
}

std::vector<std::string> WorkShop::getVMSrc()
{
    return vm_.getSourceCode();
}
