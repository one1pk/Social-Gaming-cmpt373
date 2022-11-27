#pragma once
#include "ASTVisitor.h"
#include <algorithm>

class ExpressionTree{
private:
        std::shared_ptr<ASTNode> root;
        
public:
    ExpressionTree() = default;
    ExpressionTree(std::shared_ptr<ASTNode> root, ElementMap gameListsMap, std::shared_ptr<PlayerMap> playerMap);

    std::deque<std::string> split(std::string expression);
    void build(std::string expression);
    void buildOperatorNode(std::deque<std::string>& operatorStack, std::deque<std::shared_ptr<ASTNode>>& nodeStack, std::shared_ptr<ASTNode>& root);

    bool isOperator(std::string value);
    bool isBrace(std::string value);
    bool isBinary(std::string value);
    std::vector<std::string> binaryOperators = {".", "upfrom", "sublist", "contains", 
     "==", "!=", ">", ">=", "<=", "<"};

    int getPrecedence(std::string value);
    std::map<std::string, int> precedenceMap = {
        {".", 100}, {"upfrom", 10}, {"sublist", 10}, {"size", 10}, {"collect", 10}, {"contains", 10}, 
        {">", 50}, {">=", 50}, {"<", 50}, {"<=", 50}, {"==", 50}, {"!=", 50}, {"!", 5}, 
        {"(", 0}, {")", 0} 
    }; //higher precedence has prioirty

    bool isUnary(std::string value);
    std::vector<std::string> unaryOperators = {"size", "!"};

    ElementMap gameListsMap;
    std::shared_ptr<PlayerMap> playerMap;

    std::shared_ptr<ASTNode> getRoot();

};

