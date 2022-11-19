#include "ExpressionResolver.h"


ElementSptr ExpressionResolver::getResult() { return result; }

void ExpressionResolver::visit(ASTNode& node, ElementMap elements)  {
    assert(false && "Invalid node during evaluation");
}

/// TODO: resolver should have a map of game lists so that if provided a name, it can search constants, variables etc so you can just provide it "wins" instead of "variables.wins"
/// TODO: if "{}" it should return element refered to inside brackets
/// TODO: in input choice, "to:" should also be expression tree to facilitate sending to only certain players
void ExpressionResolver::visit(NameNode& nameNode, ElementMap elements)  {
    auto elementIter = elements.find(nameNode.name);
    //if name is an element passed down from parent rule (eg. player, weapon, etc)
    if(elementIter != elements.end())
        result = elementIter->second;
        
    //otherwise, name is just a string
    else
        result = std::make_shared<Element<std::string>>(nameNode.name);
}

void ExpressionResolver::visit(ListNode& listNode, ElementMap elements)  {
    result = listNode.list;
}

void ExpressionResolver::visit(UnaryOperator& uOp, ElementMap elements)  {
    uOp.operand->accept(*this, elements);
    ElementSptr operand = result;
    std::string kind = uOp.kind;

    if(kind == "!")
        result = std::make_shared<Element<bool>>(!operand->getBool());
    else if(kind == "size")
        result = std::make_shared<Element<int>>(operand->getSizeAsInt());
    
}

void ExpressionResolver::visit(BinaryOperator& bOp, ElementMap elements)  {
    bOp.left->accept(*this, elements);
    ElementSptr left = result;
    bOp.right->accept(*this, elements);
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

