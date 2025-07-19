#ifndef _VIRTUAL_MACHINE_H_
#define _VIRTUAL_MACHINE_H_

#include <vector>
#include <string>

class VirtualMachine
{
public:
    VirtualMachine();
    ~VirtualMachine() = default;

    void run();

private:
    std::string sourceCode_;

    std::vector<int> memory_;
    size_t memorySize_;

    std::vector<int> register_;
    size_t registerSize_;
};

#endif // _VIRTUAL_MACHINE_H_