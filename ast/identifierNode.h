#ifndef _IDENTIFIER_NODE_H_
#define _IDENTIFIER_NODE_H_

#include "expressionNode.h"
#include <string>

class IdentifierNode : public ExpressionNode {
public:
    IdentifierNode(const std::string& name);
    virtual ~IdentifierNode() override = default;
    
    void evaluate() override;
    IROperand genIRCode(IRProgram& program) override;
private:
    std::string name_;
};

#endif // _IDENTIFIER_NODE_H_
