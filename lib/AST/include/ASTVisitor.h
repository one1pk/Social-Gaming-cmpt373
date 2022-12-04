#pragma once


#include "list.h"

class ASTVisitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor, ElementMap& elements) = 0;
};

class NameNode : public ASTNode {
public:
    NameNode(std::string name) : name(name) { }

    void accept(ASTVisitor& visitor, ElementMap& elements) override;
    
    std::string name;
};

class ListNode : public ASTNode { 
public:
    ListNode(std::string nameOfList, ElementSptr list) : nameOfList(nameOfList), list(list) { }

    void accept(ASTVisitor& visitor, ElementMap& elements) override;

    std::string nameOfList;
    ElementSptr list;
};

class PlayersNode : public ASTNode { 
public:
    PlayersNode(std::shared_ptr<PlayerMap> connectionPlayerPairs) : connectionPlayerPairs(connectionPlayerPairs) { }

    void accept(ASTVisitor& visitor, ElementMap& elements) override;

    std::shared_ptr<PlayerMap> connectionPlayerPairs;
};


class BinaryOperator : public ASTNode {
public:
    BinaryOperator(std::string kind, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right)
    : kind(kind), left(left), right(right) { }

    void accept(ASTVisitor& visitor, ElementMap& elements) override;

    std::string kind;
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
};

class CollectOperator : public ASTNode {
    public:
    CollectOperator(std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> middle, std::shared_ptr<ASTNode> right)
    : left(left), middle(middle), right(right) { }

    void accept(ASTVisitor& visitor, ElementMap& elements) override;

    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> middle;
    std::shared_ptr<ASTNode> right;
};

class UnaryOperator : public ASTNode {
public: 
    UnaryOperator(std::string kind, std::shared_ptr<ASTNode> operand)
    : kind(kind), operand(operand) { }

    void accept(ASTVisitor& visitor, ElementMap& elements) override;

    std::string kind;
    std::shared_ptr<ASTNode> operand;
};


class ASTVisitor {
    public:
    virtual void visit(ASTNode& node, ElementMap& elements) = 0;
    virtual void visit(NameNode& name, ElementMap& elements) = 0;
    virtual void visit(ListNode& list, ElementMap& elements) = 0;
    virtual void visit(PlayersNode& players, ElementMap& elements) = 0;
    virtual void visit(BinaryOperator& bOp, ElementMap& elements) = 0;
    virtual void visit(CollectOperator& bOp, ElementMap& elements) = 0;
    virtual void visit(UnaryOperator& uOp, ElementMap& elements) = 0;
};

inline void NameNode::accept(ASTVisitor& visitor, ElementMap& elements) {
    visitor.visit(*this, elements);
}

inline void ListNode::accept(ASTVisitor& visitor, ElementMap& elements) {
    visitor.visit(*this, elements);
}

inline void PlayersNode::accept(ASTVisitor& visitor, ElementMap& elements) {
    visitor.visit(*this, elements);
}

inline void BinaryOperator::accept(ASTVisitor& visitor, ElementMap& elements) {
    visitor.visit(*this, elements);
}

inline void CollectOperator::accept(ASTVisitor& visitor, ElementMap& elements) {
    visitor.visit(*this, elements);
}

inline void UnaryOperator::accept(ASTVisitor& visitor, ElementMap& elements) {
    visitor.visit(*this, elements);
}

