#ifndef _AST_NODE_H_
#define _AST_NODE_H_

#include "irProgram.h"
#include "irInstruction.h"

class AstNode {
public:
    AstNode() = default;
    AstNode(AstNode &&other) = default;
    virtual ~AstNode() = default;
    virtual void evaluate() = 0;

    virtual IROperand genIRCode(IRProgram& program) = 0;
};

#endif // _AST_NODE_H_