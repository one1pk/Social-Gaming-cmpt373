#include "ExpressionTree.h"

ExpressionTree::ExpressionTree(ElementMap gameListsMap, std::shared_ptr<ASTNode> root)
    : gameListsMap(gameListsMap), root(root) { }

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
    return isUnary(value) || isBinary(value);
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

void ExpressionTree::build(std::string expression){
    std::deque<std::string> tokens = split(expression);
    std::deque<std::string> operatorStack;

    std::deque<std::shared_ptr<ASTNode>> nodeStack;
    
    tokens.emplace_front("("); 
    tokens.emplace_back(")");

    for(auto token : tokens){
        if(token == "(") {
            operatorStack.emplace_back(token);
        }

        else if (!isOperator(token)){
            //if token is a gameList name i.e. constants, variables, etc
            auto gameListIter = gameListsMap.find(token);
            if(gameListIter != gameListsMap.end()){
                root = std::make_shared<ListNode>(gameListIter->second);
                nodeStack.emplace_back(root);
            }
            //else it is just a string
            else {
                root = std::make_shared<NameNode>(token);
                nodeStack.emplace_back(root);
            }
        }
        //if operator
        else if(getPrecedence(token) > 0){
            std::string stackTop = *operatorStack.begin();
            //ignore "." if followed by an operator
            if(stackTop == ".")
                operatorStack.pop_back();
                
            while (!operatorStack.empty() && stackTop != "(" && getPrecedence(stackTop) >= getPrecedence(token) ){
                if(isUnary(token)){
                    auto operand = *(nodeStack.begin());
                    nodeStack.pop_back();
                    root = std::make_shared<UnaryOperator>(*operatorStack.begin(), *operand);
                    
                    operatorStack.pop_back();
                    nodeStack.emplace_back(root);
                }
                else {
                    auto right = *(nodeStack.begin());
                    nodeStack.pop_back();
                    auto left = *(nodeStack.begin());
                    nodeStack.pop_back();

                    root = std::make_shared<BinaryOperator>(*operatorStack.begin(), *left, *right);
                    operatorStack.pop_back();
                    nodeStack.emplace_back(root);
                }
            }
            operatorStack.emplace_back(token);
        }

        else if(token == ")") {
            std::string stackTop = *operatorStack.begin();
            while(!operatorStack.empty() && stackTop != "("){
                auto left = *(nodeStack.begin());
                nodeStack.pop_back();
                auto right = *(nodeStack.begin());
                nodeStack.pop_back();

                root = std::make_shared<BinaryOperator>(*operatorStack.begin(), *left, *right);
                operatorStack.pop_back();
                nodeStack.emplace_back(root);
            }
            operatorStack.pop_back();
        }
    }

    root = *nodeStack.begin();
}


