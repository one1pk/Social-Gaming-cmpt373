#include "ExpressionTree.h"
#include <iostream>
ExpressionTree::ExpressionTree(std::shared_ptr<ASTNode> root, ElementMap gameListsMap, std::shared_ptr<PlayerMap> playerMap)
    : root(root), gameListsMap(gameListsMap), playerMap(playerMap) { }

std::shared_ptr<ASTNode> ExpressionTree::getRoot(){
    return root;
}
std::deque<std::string> ExpressionTree::split(std::string expression) {
    int pos = 0;
    bool alnum = false;
    bool punct = false;
    std::deque<std::string> splits;

    for (unsigned i = 0; i < expression.length(); i++) {
        if (isalnum(expression[i])) {
            if (punct) {
                splits.emplace_back(expression.substr(pos, i-pos));
                punct = false;
            }
            if (!alnum) {
                pos = i;
                alnum = true;
            }
        } else if (ispunct(expression[i]) && !(expression[i] == '-' || expression[i] == '>' || expression[i] == '{' || expression[i] == '}')) {
            if (alnum) {
                splits.emplace_back(expression.substr(pos, i-pos));
                alnum = false;
            }
            if (!punct) {
                pos = i;
                punct = true;
            }
        } else {
            if (alnum || punct) {
                splits.emplace_back(expression.substr(pos, i-pos));
                alnum = punct = false;
            }
        }
    }
    if (alnum || punct) {
        splits.emplace_back(expression.substr(pos, expression.length()-pos));
    }

    return splits;
}
bool ExpressionTree::isOperator(std::string value){
    return isUnary(value) || isBinary(value) || value == "collect";
}
bool ExpressionTree::isBrace(std::string value){
    return value == "(" || value == ")";
}
bool ExpressionTree::isBinary(std::string value){
    auto itB = std::find(binaryOperators.begin(), binaryOperators.end(), value);
    return itB != binaryOperators.end();
}
bool ExpressionTree::isUnary(std::string value){
    auto itU = std::find(unaryOperators.begin(), unaryOperators.end(), value);
    return itU != unaryOperators.end();
}

int ExpressionTree::getPrecedence(std::string key){
    return precedenceMap.at(key);
}

void ExpressionTree::buildOperatorNode(std::deque<std::string>& operatorStack, std::deque<std::shared_ptr<ASTNode>>& nodeStack, std::shared_ptr<ASTNode>& root) {
    if(isUnary(operatorStack.back())){
        auto operand = nodeStack.back();
        nodeStack.pop_back();
        root = std::make_shared<UnaryOperator>(operatorStack.back(), operand);
        
        operatorStack.pop_back();
        nodeStack.emplace_back(std::move(root));
    }
    else if(operatorStack.back() == "collect"){
        auto right = nodeStack.back();
        nodeStack.pop_back();
        auto middle = nodeStack.back();
        nodeStack.pop_back();
        auto left= nodeStack.back();
        nodeStack.pop_back();

        root = std::make_shared<CollectOperator>(left, middle, right);
        operatorStack.pop_back();
        nodeStack.emplace_back(std::move(root));

    }
    else{
        auto right = nodeStack.back();
        nodeStack.pop_back();
        auto left= nodeStack.back();
        nodeStack.pop_back();

        root = std::make_shared<BinaryOperator>(operatorStack.back(), left, right);
        operatorStack.pop_back();
        nodeStack.emplace_back(std::move(root));
    }
}

void ExpressionTree::build(std::string expression){
    std::deque<std::string> tokens = split(expression);
    std::deque<std::string> operatorStack;

    std::deque<std::shared_ptr<ASTNode>> nodeStack;

    std::string prevToken;

    tokens.emplace_front("(");
    tokens.emplace_back(")");
    for(auto token : tokens){

        if(token == "(") {
            operatorStack.emplace_back(token);
        }
        else if ( !isOperator(token) && !isBrace(token)) {

            //if token is a gameList name i.e. constants, variables, etc
            auto gameListIter = gameListsMap.find(token);
            if(gameListIter != gameListsMap.end() && prevToken != "collect"){
                root = std::make_shared<ListNode>(token, gameListIter->second);
                nodeStack.emplace_back(std::move(root));
            }

            else if(token == "players"){
                root = std::make_shared<PlayersNode>(playerMap);
                nodeStack.emplace_back(std::move(root));
            }
                
            //else it is just a string
            else {
                root = std::make_shared<NameNode>(token);
                nodeStack.emplace_back(std::move(root));
            }
        }
        //if token is operator
        else if(getPrecedence(token) > 0){
            //ignore "." if it directly precedes operator
            if(prevToken == ".")
                operatorStack.pop_back();
                
            while (!operatorStack.empty() && operatorStack.back() != "(" && getPrecedence(operatorStack.back()) >= getPrecedence(token) ){
                buildOperatorNode(operatorStack, nodeStack, root);
            }
            operatorStack.emplace_back(token);
        }

        else if(token == ")") {
            while( !operatorStack.empty() && operatorStack.back() != "(") {
               buildOperatorNode(operatorStack, nodeStack, root);
            }
            operatorStack.pop_back();
        }

        prevToken = token;
    }
   
    root = std::move(nodeStack.back());
}


