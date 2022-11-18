#pragma once
#include "ASTVisitor.h"
#include <algorithm>

class TreeBuilder{
    public:
    TreeBuilder() = default;
    TreeBuilder(ElementMap gameListsMap);
    std::vector<std::string> split(std::string expression);
    std::shared_ptr<ASTNode> buildTree(std::string expression);

    bool isOperator(std::string value);

    bool isBinary(std::string value);
    std::vector<std::string> binaryOperators = {".", "upfrom", "sublist", "collect",
     "==", "!=", ">", ">=", "<=", "<"};

    int getPrecedence(std::string value);
    std::map<std::string, int> precedenceMap = {
        {".", 100}, {"upfrom", 10}, {"sublist", 10}, {"size", 10}, {"collect", 10},
        {">", 50}, {">=", 50}, {"<", 50}, {"<=", 50}, {"==", 50}, {"!=", 50}, {"!", 5}, 
        {"(", 0}, {")", 0} 
    };

    bool isUnary(std::string value);
    std::vector<std::string> unaryOperators = {"size", "!"};

    ElementMap gameListsMap;
};

