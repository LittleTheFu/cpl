#include "vmCodeGenerator.h"

VmCodeGenerator::VmCodeGenerator()
{
}

std::vector<Instruction> VmCodeGenerator::translate(const std::vector<IRInstruction> &irInstructions)
{
    std::vector<Instruction> instructions;

    for(const auto& irInstruction : irInstructions)
    {
        auto instruction = translate(irInstruction);
        instructions.insert(instructions.end(), instruction.begin(), instruction.end());
    }

    return instructions;
}

std::vector<Instruction> VmCodeGenerator::translate(const IRInstruction &irInstruction)
{
    switch(irInstruction.getOpCode())
    {
        case IROpCode::ADD:
        case IROpCode::MUL:
            return translateBinaryOperation(irInstruction);
        default:
            throw std::runtime_error("bad ir opcode.");
    }
}

std::vector<Instruction> VmCodeGenerator::translateBinaryOperation(const IRInstruction &irInstruction)
{
    IROperand result = irInstruction.getOperand(0);
    if(result.type_ != IROperandType::TEMPORARY)
    {
        throw std::runtime_error("bad ir opcode.");
    }
    std::string resultVar = "t" + std::to_string(result.getIntValue());
    size_t resultAddress = 0;

    if(varMap_.find(resultVar) == varMap_.end())
    {
        resultAddress = currentVarAddress;
        varMap_[resultVar] = resultAddress;
        currentVarAddress += 1;
    }
    else
    {
        resultAddress = varMap_[resultVar];
    }

    int firstInt = 0;
    IROperand operand1 = irInstruction.getOperand(1);
    if(operand1.type_ == IROperandType::IDENTIFIER)
    {        
        firstInt = varMap_.at(operand1.getStringValue());
    }
    else if(operand1.type_ == IROperandType::TEMPORARY)
    {
        firstInt = varMap_.at("t" + std::to_string(operand1.getIntValue()));
    }
    else
    {
        firstInt = operand1.getIntValue();
    }

    int secondInt = 0;
    IROperand operand2 = irInstruction.getOperand(2);
    if(operand2.type_ == IROperandType::TEMPORARY)
    {
        secondInt = varMap_.at("t" +  std::to_string(operand2.getIntValue()));
    }
    else if(operand2.type_ == IROperandType::IDENTIFIER)
    {
        secondInt = varMap_.at(operand2.getStringValue());
    }
    else
    {
        secondInt = operand2.getIntValue();
    }

    OperandType firstAccessType = OperandType::IMMEDIATE;
    if(operand1.type_ == IROperandType::TEMPORARY || operand1.type_ == IROperandType::IDENTIFIER)
    {
        firstAccessType = OperandType::MEMORY;
    }

    OperandType secondAccessType = OperandType::IMMEDIATE;
    if(operand2.type_ == IROperandType::TEMPORARY || operand2.type_ == IROperandType::IDENTIFIER)
    {
        secondAccessType = OperandType::MEMORY;
    }

    OpCode operatorCode = OpCode::ADD;
    if(irInstruction.getOpCode() == IROpCode::MUL)
    {
        operatorCode = OpCode::MUL;
    }

    int regId = getNextRegisterId();
    Instruction moveToRegister{OpCode::MOV,
                               Operand(OperandType::REGISTER, regId),
                               Operand(firstAccessType, firstInt)};
    Instruction op{operatorCode,
                    Operand(OperandType::REGISTER, regId),
                    Operand(secondAccessType, secondInt)};
    Instruction moveToAddress{OpCode::MOV,
                             Operand(OperandType::MEMORY, resultAddress),
                             Operand(OperandType::REGISTER, regId)};

    std::vector<Instruction> instructions;
    instructions.push_back(moveToRegister);
    instructions.push_back(op);
    instructions.push_back(moveToAddress);

    return instructions;
}


int VmCodeGenerator::getNextRegisterId()
{
    return registerId++;
}
