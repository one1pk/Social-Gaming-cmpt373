#pragma once

#include "game.h"
#include "list.h"

class ASTVisitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

class NameNode : public ASTNode {
public:
    
    NameNode(std::string name) : name(name) { }

    void accept(ASTVisitor& visitor) override;
    
    std::string name;
};

class ListNode : public ASTNode { 
public:
    
    ListNode(ElementSptr list) :  list(list) { }

    void accept(ASTVisitor& visitor) override;

    ElementSptr list;
};


class BinaryOperator : public ASTNode {
public:
  
    BinaryOperator(std::string kind, ASTNode& left, ASTNode& right)
    : kind(kind), left(left), right(right) { }

    void accept(ASTVisitor& visitor) override;

    std::string kind;
    ASTNode& left;
    ASTNode& right;
};

class UnaryOperator : public ASTNode {
public: 
    
  
    UnaryOperator(std::string kind, ASTNode& operand)
    : kind(kind), operand(operand) { }

    void accept(ASTVisitor& visitor) override;

    std::string kind;
    ASTNode& operand;
};


class ASTVisitor {
    public:
    virtual void visit(ASTNode& node) = 0;
    virtual void visit(NameNode& name) = 0;
    virtual void visit(ListNode& list) = 0;
    virtual void visit(BinaryOperator& bOp) = 0;
    virtual void visit(UnaryOperator& uOp) = 0;
};

void NameNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ListNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void BinaryOperator::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void UnaryOperator::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

