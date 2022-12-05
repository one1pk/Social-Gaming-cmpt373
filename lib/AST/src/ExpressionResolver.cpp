#include "ExpressionResolver.h"
#include <cassert>
#include <algorithm>
#include <iostream>
#include <memory>

ElementSptr ExpressionResolver::getResult() { return result; }

void ExpressionResolver::visit(ASTNode& node, ElementMap& elements)  {
    assert(false && "Invalid node during evaluation");
}

/// TODO: resolver should have a map of game lists so that if provided a name, it can search constants, variables etc so you can just provide it "wins" instead of "variables.wins"
/// TODO: if "{}" it should return element refered to inside brackets
/// TODO: in input choice, "to:" should also be expression tree to facilitate sending to only certain players
void ExpressionResolver::visit(NameNode& nameNode, ElementMap& elements)  {
    auto elementIter = elements.find(nameNode.name);
    //if name is an element passed down from parent rule (eg. player, weapon, etc)
    if(elementIter != elements.end()){
        result = elementIter->second;
    }
        
    //otherwise, name is just a string
    else
        result = std::make_shared<Element<std::string>>(nameNode.name);
}

void ExpressionResolver::visit(NumberNode& numNode, ElementMap& elements){
    result = std::make_shared<Element<int>>(numNode.num);
}

void ExpressionResolver::visit(ListNode& listNode, ElementMap& elements)  {
    result = listNode.list;
}

void ExpressionResolver::visit(PlayersNode& playersNode, ElementMap& elements)  {
    ElementVector players;
    for(auto playerMap : *(playersNode.connectionPlayerPairs)){
        players.emplace_back(playerMap.second);
    }
    result = std::make_shared<Element<ElementVector>>(players);
}

void ExpressionResolver::visit(UnaryOperator& uOp, ElementMap& elements)  {
    std::string kind = uOp.kind;
    uOp.operand->accept(*this, elements);
    ElementSptr operand = result;
    
    if(kind == "!")
        result = std::make_shared<Element<bool>>(!operand->getBool());
    else if(kind == "size")
        result = std::make_shared<Element<int>>(operand->getSizeAsInt());
    
}

void ExpressionResolver::visit(CollectOperator& cOp, ElementMap& elements) {
    ElementVector collection;
    cOp.left->accept(*this, elements);
    auto left = result;

    for(auto elementIter : left->getVector()){
        //middle node should always be string and not found in elementMap
        ElementMap emptyMap;
        cOp.middle->accept(*this, emptyMap);
        std::string middle = result->getString();

        /// TODO: allow for more complex middle
        elements[middle] = elementIter;
        cOp.right->accept(*this, elements);
        ElementSptr right = result;
        if(result->getBool())
            collection.emplace_back(elementIter);
    }

    result = std::make_shared<Element<ElementVector>>(collection);
}

void ExpressionResolver::visit(BinaryOperator& bOp, ElementMap& elements)  {
    std::string kind = bOp.kind;
    bOp.left->accept(*this, elements);
    ElementSptr left = result;

    if(kind == ".") {
        //In case of dot operator, RHS should always be string an never be found in elementMap otherwise player.weapon will be the same weapon as in weapon.name
        ElementMap emptyMap;
        bOp.right->accept(*this, emptyMap);
        ElementSptr right = result;

        if(left->type == Type::VECTOR){
            result = std::make_shared<Element<ElementVector>>(left->getSubList(right->getString()));
        }
        else {
            result = left->getMapElement(right->getString());
        }
        return;
    }

    
    bOp.right->accept(*this, elements);
    ElementSptr right = result;
    

    if(kind == "upfrom")
        result = left->upfrom(stoi(right->getString()));

    else if(kind == "contains")
        result = std::make_shared<Element<bool>>(left->contains(right));

    else if(kind == "==") {
        if(left->type == right->type){
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

