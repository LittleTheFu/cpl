#include "virtualMachine.h"

int main()
{
    VirtualMachine vm;

    vm.loadProgram();
    vm.execute();

    return 0;
}