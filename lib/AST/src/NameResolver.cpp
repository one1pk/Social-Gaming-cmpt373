#include "ASTVisitor.h"

class NameResolver : public ASTVisitor {
public:
    
    void visit(ASTNode& node) override {
        assert(false && "Invalid node during evaluation");
    }

    void visit(NameNode& name) override {
        result = std::make_shared<Element<std::string>>(name.name);
    }

    void visit(ListNode& listNode) override {
        result = listNode.list;
    }

    void visit(UnaryOperator& uOp) override {
        uOp.operand.accept(*this);
        ElementSptr operand = result;
        std::string kind = uOp.kind;

        if(kind == "!")
            result = std::make_shared<Element<bool>>(!operand);
        else if(kind == "size")
            result = std::make_shared<Element<int>>(operand->getSizeAsInt());
        
    }

    void visit(BinaryOperator& bOp) override {
        bOp.left.accept(*this);
        ElementSptr left = result;
        bOp.right.accept(*this);
        ElementSptr right = result;
        std::string kind = bOp.kind;

        if(kind == ".") 
            result = left->getMapElement(right->getString());
        else if(kind == "upfrom")
            result = left->upfrom(stoi(right->getString()));
        else if(kind == "sublist")
            result = std::make_shared<Element<ElementVector>>(left->getSubList(right->getString()));
        else if(kind == "collect")
            assert(false && "Collect not supported yet");
        else if(kind == "==") {
            if(left->type == Type::STRING && right->type == Type::STRING)
                result = std::make_shared<Element<bool>>(left->getString() == right->getString());
            else if(left->type == Type::INT && right->type == Type::INT)
                result = std::make_shared<Element<bool>>(left->getInt() == right->getInt());
            else 
                assert(false && "Invalid node during evaluation");
        }
        else if(kind == "!-") {
            if(left->type == Type::STRING && right->type == Type::STRING)
                result = std::make_shared<Element<bool>>(left->getString() != right->getString());
            else if(left->type == Type::INT && right->type == Type::INT)
                result = std::make_shared<Element<bool>>(left->getInt() != right->getInt());
            else 
                assert(false && "Invalid node during evaluation");
        }
        else if(kind == ">") {
            if(left->type == Type::STRING && right->type == Type::STRING)
                result = std::make_shared<Element<bool>>(left->getString() > right->getString());
            else if(left->type == Type::INT && right->type == Type::INT)
                result = std::make_shared<Element<bool>>(left->getInt() > right->getInt());
            else 
                assert(false && "Invalid node during evaluation");
        }
        else if(kind == "<") {
            if(left->type == Type::STRING && right->type == Type::STRING)
                result = std::make_shared<Element<bool>>(left->getString() < right->getString());
            else if(left->type == Type::INT && right->type == Type::INT)
                result = std::make_shared<Element<bool>>(left->getInt() < right->getInt());
            else 
                assert(false && "Invalid node during evaluation");
        }
        else if(kind == "<=") {
            if(left->type == Type::STRING && right->type == Type::STRING)
                result = std::make_shared<Element<bool>>(left->getString() <= right->getString());
            else if(left->type == Type::INT && right->type == Type::INT)
                result = std::make_shared<Element<bool>>(left->getInt() <= right->getInt());
            else 
                assert(false && "Invalid node during evaluation");
        }
        else if(kind == ">="){
            if(left->type == Type::STRING && right->type == Type::STRING)
                result = std::make_shared<Element<bool>>(left->getString() >= right->getString());
            else if(left->type == Type::INT && right->type == Type::INT)
                result = std::make_shared<Element<bool>>(left->getInt() >= right->getInt());
            else 
                assert(false && "Invalid node during evaluation");
        }
    }

    ElementSptr getResult() { return result; }

private:
    ElementSptr result;
};

