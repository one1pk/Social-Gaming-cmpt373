#pragma once

#include "ASTVisitor.h"

class TreePrinter : public ASTVisitor {
public:
    
    void visit(ASTNode& node, ElementMap& elements) override;
    
    void visit(NameNode& name, ElementMap& elements) override;

    void visit(ListNode& listNode, ElementMap& elements) override;

    void visit(PlayersNode& playersNode, ElementMap& elements) override;

    void visit(UnaryOperator& uOp, ElementMap& elements) override;

    void visit(CollectOperator& uOp, ElementMap& elements) override;

    void visit(BinaryOperator& bOp, ElementMap& elements) override;

    std::string getResult();

private:
    std::string result;
};
