#pragma once
#include "ASTVisitor.h"
#include <algorithm>

class ExpressionTree{
public:
    enum nodeType{
        OPEN_BRACE,
        CLOSE_BRACE,
        UNARY,
        BINARY,
        TERNARY,
        PLAYERS,
        NAME,
        NUMBER,
        LIST
    };

    ExpressionTree() = default;
    ExpressionTree(ElementMap& gameState, std::shared_ptr<PlayerMap> playerMap);

    nodeType getType(std::string token);
    void build(std::string expression);
    int getPrecedence(std::string value);
    void buildOperatorNode(nodeType type);
    std::deque<std::string> splitString(std::string expression);

    std::vector<std::string> unaryOperators = {"size", "!"};
    std::vector<std::string> binaryOperators = {".", "upfrom", "sublist", "contains", 
        "==", "!=", ">", ">=", "<=", "<"};
    std::vector<std::string> ternaryOperators = {"collect"};

    std::map<std::string, int> precedenceMap = {
        {".", 100}, {"upfrom", 10}, {"sublist", 10}, {"size", 15}, {"collect", 10}, {"contains", 10}, 
        {">", 10}, {">=", 10}, {"<", 10}, {"<=", 10}, {"==", 10}, {"!=", 10}, {"!", 5}, 
        {"(", 0}, {")", 0} 
    }; //higher precedence has prioirty

    ElementMap gameState;
    std::shared_ptr<PlayerMap> playerMap;

    std::shared_ptr<ASTNode> getRoot();

private:
    std::shared_ptr<ASTNode> root;
    std::deque<std::string> operatorStack;
    std::deque<std::shared_ptr<ASTNode>> nodeStack;
};

