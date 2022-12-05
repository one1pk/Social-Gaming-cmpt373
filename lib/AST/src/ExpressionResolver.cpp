#include "ExpressionResolver.h"
#include <cassert>
#include <algorithm>
#include <iostream>
#include <memory>

ElementSptr ExpressionResolver::getResult() { return result; }

//ASTNode
void ExpressionResolver::visit(ASTNode& node, ElementMap& elements)  {
    assert(false && "Invalid node during evaluation");
}

//NameNode
void ExpressionResolver::visit(NameNode& nameNode, ElementMap& elements)  {
    auto name = nameNode.name;
    //if name is an element passed down from parent rule (eg. player, weapon, etc)
    auto elementIter = elements.find(name);
    if(elementIter != elements.end()){
        result = elementIter->second;
    }
    //otherwise, name is just a string
    else
        result = std::make_shared<Element<std::string>>(name);
}

//NumberNode
void ExpressionResolver::visit(NumberNode& numNode, ElementMap& elements){
    result = std::make_shared<Element<int>>(numNode.num);
}

//ListNode
void ExpressionResolver::visit(ListNode& listNode, ElementMap& elements)  {
    result = listNode.list;
}

//PlayersNode
void ExpressionResolver::visit(PlayersNode& playersNode, ElementMap& elements)  {
    ElementVector players;
    for(auto playerMap : *(playersNode.connectionPlayerPairs)){
        players.emplace_back(playerMap.second);
    }
    result = std::make_shared<Element<ElementVector>>(players);
}

//UnaryOperator
void ExpressionResolver::visit(UnaryOperator& uOp, ElementMap& elements)  {
    std::string kind = uOp.kind;
    uOp.operand->accept(*this, elements);
    ElementSptr operand = result;
    
    if(kind == "!")
        result = std::make_shared<Element<bool>>(!operand->getBool());
    else if(kind == "size")
        result = std::make_shared<Element<int>>(operand->getSizeAsInt());
    
}

//TernaryOperator
void ExpressionResolver::visit(TernaryOperator& tOp, ElementMap& elements) {
    if(tOp.kind == "collect"){
        ElementVector collection;
        tOp.left->accept(*this, elements);
        auto left = result;

        for(auto elementIter : left->getVector()){
            //middle node should always be string and not found in elementMap
            std::string middle = tOp.middle->getName();
            elements[middle] = elementIter;

            tOp.right->accept(*this, elements);
            ElementSptr right = result;
            if(result->getBool())
                collection.emplace_back(elementIter);
        }

        result = std::make_shared<Element<ElementVector>>(collection);
    }
}

void ExpressionResolver::visit(BinaryOperator& bOp, ElementMap& elements)  {
    std::string kind = bOp.kind;
    bOp.left->accept(*this, elements);
    ElementSptr left = result;

    if(kind == ".") {
        //since '.' before an operator is always ignored in expressionTree.build(),
        //rhs of '.' will always be a NameNode or ListNode and we just want the string/name to pass to sublist/getMapElement
        if(left->type == Type::VECTOR){
            result = std::make_shared<Element<ElementVector>>(left->getSubList(bOp.right->getName()));
        }
        else {
            result = left->getMapElement(bOp.right->getName());
        }
        return;
    }

    bOp.right->accept(*this, elements);
    ElementSptr right = result;
    
    if(kind == "upfrom")
        result = left->upfrom(right->getInt());

    else if(kind == "contains")
        result = std::make_shared<Element<bool>>(left->contains(right));

    else if(kind == "==") {
        if(left->type == right->type){
            //getString() already turns integer to string
            result = std::make_shared<Element<bool>>(left->getString() == right->getString());
        }
        else 
            assert(false && "Invalid node during evaluation");
    }

    else if(kind == "!=") {
        if(left->type == right->type)
            result = std::make_shared<Element<bool>>(left->getString() != right->getString());
        else 
            assert(false && "Invalid node during evaluation");
    }

    else if(kind == ">") {
        if(left->type == right->type)
            result = std::make_shared<Element<bool>>(left->getString() > right->getString());
        else 
            assert(false && "Invalid node during evaluation");
    }

    else if(kind == "<") {
        if(left->type == right->type)
            result = std::make_shared<Element<bool>>(left->getString() < right->getString());
        else 
            assert(false && "Invalid node during evaluation");
    }

    else if(kind == "<=") {
        if(left->type == right->type)
            result = std::make_shared<Element<bool>>(left->getString() <= right->getString());
        else 
            assert(false && "Invalid node during evaluation");
    }
    
    else if(kind == ">="){
        if(left->type == right->type)
            result = std::make_shared<Element<bool>>(left->getString() >= right->getString());
        else 
            assert(false && "Invalid node during evaluation");
    }
}

