#include "TreePrinter.h"
#include <iostream>
#include <cassert>

std::string TreePrinter::getResult() { return result; }

void TreePrinter::visit(ASTNode& node, ElementMap& elements){
    assert(false && "Invalid node during evaluation");
}
    
void TreePrinter::visit(NameNode& nameNode, ElementMap& elements) {
    std::cout << nameNode.name ;
}

void TreePrinter::visit(NumberNode& numNode, ElementMap& elements) {
    std::cout << numNode.num ;
}

void TreePrinter::visit(ListNode& listNode, ElementMap& elements) {
    std::cout << listNode.nameOfList;
}

void TreePrinter::visit(PlayersNode& playerNode, ElementMap& elements) {
    std::cout << "players";
}

void TreePrinter::visit(TernaryOperator& tOp, ElementMap& elements) {
    std::cout << '(';
    tOp.left->accept(*this, elements);
    std::cout << ' ' << tOp.kind << ' ';
    tOp.right->accept(*this, elements);
    std::cout << ')';

}

void TreePrinter::visit(UnaryOperator& uOp, ElementMap& elements) {
    std::cout << '(';
    uOp.operand->accept(*this, elements);
    std::cout << ' ' << uOp.kind;
    std::cout << ')';
}

void TreePrinter::visit(BinaryOperator& bOp, ElementMap& elements) {
    std::cout << '(';
    bOp.left->accept(*this, elements);
    std::cout << ' ' << bOp.kind << ' ';
    bOp.right->accept(*this, elements);
    std::cout << ')';
}
