#pragma once


#include "list.h"

class ASTVisitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor, ElementMap elements) = 0;
};

class NameNode : public ASTNode {
public:
    NameNode(std::string name) : name(name) { }

    void accept(ASTVisitor& visitor, ElementMap elements) override;
    
    std::string name;
};

class ListNode : public ASTNode { 
public:
    ListNode(ElementSptr list) :  list(list) { }

    void accept(ASTVisitor& visitor, ElementMap elements) override;

    ElementSptr list;
};


class BinaryOperator : public ASTNode {
public:
    BinaryOperator(std::string kind, ASTNode& left, ASTNode& right)
    : kind(kind), left(left), right(right) { }

    void accept(ASTVisitor& visitor, ElementMap elements) override;

    std::string kind;
    ASTNode& left;
    ASTNode& right;
};

class UnaryOperator : public ASTNode {
public: 
    UnaryOperator(std::string kind, ASTNode& operand)
    : kind(kind), operand(operand) { }

    void accept(ASTVisitor& visitor, ElementMap elements) override;

    std::string kind;
    ASTNode& operand;
};


class ASTVisitor {
    public:
    virtual void visit(ASTNode& node, ElementMap elements) = 0;
    virtual void visit(NameNode& name, ElementMap elements) = 0;
    virtual void visit(ListNode& list, ElementMap elements) = 0;
    virtual void visit(BinaryOperator& bOp, ElementMap elements) = 0;
    virtual void visit(UnaryOperator& uOp, ElementMap elements) = 0;
};

inline void NameNode::accept(ASTVisitor& visitor, ElementMap elements) {
    visitor.visit(*this, elements);
}

inline void ListNode::accept(ASTVisitor& visitor, ElementMap elements) {
    visitor.visit(*this, elements);
}

inline void BinaryOperator::accept(ASTVisitor& visitor, ElementMap elements) {
    visitor.visit(*this, elements);
}

inline void UnaryOperator::accept(ASTVisitor& visitor, ElementMap elements) {
    visitor.visit(*this, elements);
}

