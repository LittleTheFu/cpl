#ifndef _AST_NODE_H_
#define _AST_NODE_H_

class AstNode {
public:
    AstNode() = default;
    AstNode(AstNode &&other) = default;
    virtual ~AstNode() = default;
    virtual void evaluate() = 0;
};

#endif // _AST_NODE_H_