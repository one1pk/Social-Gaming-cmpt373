#pragma once

#include "ASTVisitor.h"

class ExpressionResolver : public ASTVisitor {
public:
    
    void visit(ASTNode& node) override;
    
    void visit(NameNode& name) override;

    void visit(ListNode& listNode) override;

    void visit(UnaryOperator& uOp) override;

    void visit(BinaryOperator& bOp) override;

    ElementSptr getResult();

private:
    ElementSptr result;
};

