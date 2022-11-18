#include "ExpressionResolver.h"


ElementSptr ExpressionResolver::getResult() { return result; }

void ExpressionResolver::visit(ASTNode& node)  {
    assert(false && "Invalid node during evaluation");
}

void ExpressionResolver::visit(NameNode& name)  {
    result = std::make_shared<Element<std::string>>(name.name);
}

void ExpressionResolver::visit(ListNode& listNode)  {
    result = listNode.list;
}

void ExpressionResolver::visit(UnaryOperator& uOp)  {
    uOp.operand.accept(*this);
    ElementSptr operand = result;
    std::string kind = uOp.kind;

    if(kind == "!")
        result = std::make_shared<Element<bool>>(!operand->getBool());
    else if(kind == "size")
        result = std::make_shared<Element<int>>(operand->getSizeAsInt());
    
}

void ExpressionResolver::visit(BinaryOperator& bOp)  {
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

    //might make collect a trinary operator
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

