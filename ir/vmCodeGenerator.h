#ifndef _VM_CODE_GENERATOR_H_
#define _VM_CODE_GENERATOR_H_

#include "instruction.h"
#include "irInstruction.h"

class VmCodeGenerator
{
public:
    VmCodeGenerator();
    ~VmCodeGenerator() = default;

    std::vector<Instruction> translate(const std::vector<IRInstruction>& irInstructions);
    
private:
    std::vector<Instruction> translate(const IRInstruction& irInstruction);
    std::vector<Instruction> translateAdd(const IRInstruction& irInstruction);
    std::vector<Instruction> translateMul(const IRInstruction& irInstruction);

    int getNextRegisterId();
    int registerId = 0;

    std::map<std::string, size_t> varMap_;
    size_t currentVarAddress = 200;
};

#endif // _VM_CODE_GENERATOR_H_