#ifndef _IR_INSTRUCTION_H_
#define _IR_INSTRUCTION_H_

#include <variant>
#include <string>
#include <vector>
#include <stdexcept>

enum class IROpCode
{
    NOP,
    MOV,
    ADD,
    SUB,
    MUL,
    DIV,

    UNKNOWN,
};

enum class IROperandType
{
    INTEGER_CONSTANT,
    IDENTIFIER,
    TEMPORARY,
};

struct IROperand
{
    IROperandType type_;
    std::variant<int, std::string> valueData_;

    IROperand(IROperandType type, std::variant<int, std::string> value)
        : type_(type), valueData_(value) {}

    int getIntValue() const
    {
        return std::get<int>(valueData_);
    }

    std::string getStringValue() const
    {
        return std::get<std::string>(valueData_);
    }

    static IROperand createInt(int value)
    {
        return IROperand(IROperandType::INTEGER_CONSTANT, value);
    }

    static IROperand createIdentifier(std::string value)
    {
        return IROperand(IROperandType::IDENTIFIER, value);
    }

    static IROperand createTemporary(int value)
    {
        return IROperand(IROperandType::TEMPORARY, value);
    }

    std::string toString() const 
    {
        switch(type_)
        {
            case IROperandType::INTEGER_CONSTANT:
                return std::to_string(getIntValue());
            case IROperandType::IDENTIFIER:
                return getStringValue();
            case IROperandType::TEMPORARY:
                return "t" + std::to_string(getIntValue());
            default:
                throw std::runtime_error("bad type.");
        }
    }
};

class IRInstruction
{
public:
    IRInstruction(IROpCode opCode, std::vector<IROperand> &&operands);
    ~IRInstruction() = default;

    IROpCode getOpCode() const;
    IROperand getOperand(size_t index) const;

private:
    IROpCode opCode_;
    std::vector<IROperand> operands_;

    static const int OPRAND_NUM = 3;
};

#endif // _IR_INSTRUCTION_H_