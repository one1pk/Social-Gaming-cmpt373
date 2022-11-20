#pragma once

#include "ASTVisitor.h"

class ExpressionResolver : public ASTVisitor {
public:
    
    void visit(ASTNode& node, ElementMap elements) override;
    
    void visit(NameNode& name, ElementMap elements) override;

    void visit(ListNode& listNode, ElementMap elements) override;

    void visit(PlayersNode& playersNode, ElementMap elements) override;

    void visit(UnaryOperator& uOp, ElementMap elements) override;

    void visit(BinaryOperator& bOp, ElementMap elements) override;

    ElementSptr getResult();

private:
    ElementSptr result;
};

