#include "virtualMachine.h"

int main()
{
    VirtualMachine vm;
    vm.loadProgram();

    vm.printStack();
    vm.printRegister();
    vm.printFlags();

    vm.execute();

    vm.printStack();
    vm.printRegister();
    vm.printFlags();

    return 0;
}