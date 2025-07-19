#include "virtualMachine.h"

int main()
{
    VirtualMachine vm;
    vm.loadProgram();

    vm.printStack();
    vm.printRegister();

    vm.execute();

    vm.printStack();
    vm.printRegister();

    return 0;
}