#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include <string>

enum class OpCode
{
    NOP,
    PUSH,
    POP,
    ADD,
    SUB,
    MUL,
    DIV,
    JMP,
    JMPZ,
    JMPNZ,
    CALL,
    RET,
    HLT,
};

enum class OperandType
{
    REGISTER,
    MEMORY,
    IMMEDIATE,
};

struct Operand
{
    OperandType type;
    int value;
};

class Instruction
{
public:
    Instruction();
    ~Instruction() = default;

    void readInstruction(const std::string &line);

private:
    std::string trim(const std::string &str);
    OpCode stringToOpCode(const std::string &str);
    Operand stringToOperand(const std::string &str);

private:
    OpCode opCode_;

    Operand operandFirst_;
    Operand operandSecond_;
};

#endif // _INSTRUCTION_H_