#ifndef _AST_NODE_H_
#define _AST_NODE_H_

class AstNode {
public:
    virtual ~AstNode() = default;
    virtual void evaluate() = 0;
};

#endif // _AST_NODE_H_