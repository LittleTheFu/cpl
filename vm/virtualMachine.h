#ifndef _VIRTUAL_MACHINE_H_
#define _VIRTUAL_MACHINE_H_

#include <vector>

class VirtualMachine
{
public:
    VirtualMachine();
    ~VirtualMachine() = default;

private:
    std::vector<int> memory_;
    size_t memorySize_;
};

#endif // _VIRTUAL_MACHINE_H_