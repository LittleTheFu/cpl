#ifndef _IR_PROGRAM_H_
#define _IR_PROGRAM_H_

#include <vector>
#include "irInstruction.h"

class IRProgram
{
public:
    IRProgram();
    ~IRProgram() = default;

    void addInstruction(IRInstruction &&instruction);
    int getTempVarCount();

private:
    std::vector<IRInstruction> instructions_;
    int temp_var_count_ = 0;
};


#endif // _IR_PROGRAM_H_