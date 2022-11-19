#include "ExpressionTree.h"

ExpressionTree::ExpressionTree(ElementMap gameListsMap) : gameListsMap(gameListsMap) { }

std::shared_ptr<ASTNode> ExpressionTree::getRoot(){
    return root;
}
std::vector<std::string> ExpressionTree::split(std::string expression) {
    int pos = 0;
    bool alnum = false;
    bool punct = false;
    std::vector<std::string> splits;
    splits.reserve(expression.length());

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
        } else if (ispunct(expression[i]) && !(expression[i] == '-' || expression[i] == '>')) {
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
    std::vector<std::string> tokens = split(expression);
    std::deque<std::string> operatorStack;

    std::deque<std::shared_ptr<ASTNode>> nodeStack;
    
    for(auto token : tokens){
        if(token == "(") {
            operatorStack.emplace_front(token);
        }

        else if (!isOperator(token)){
            //if token is a gameList name i.e. constants, variables, etc
            auto gameListIter = gameListsMap.find(token);
            if(gameListIter != gameListsMap.end()){
                root = std::make_unique<ListNode>(gameListIter->second);
                nodeStack.emplace_front(root);
            }
            //else it is just a string
            else {
                root = std::make_unique<NameNode>(token);
                nodeStack.emplace_front(root);
            }
        }
        //if operator
        else if(getPrecedence(token) > 0){
            std::string stackTop = *operatorStack.begin();
            //ignore "." if followed by an operator
            if(stackTop == ".")
                operatorStack.pop_front();
                
            while (!operatorStack.empty() && stackTop != "(" && getPrecedence(stackTop) >= getPrecedence(token) ){
                if(isUnary(token)){
                    auto operand = *(nodeStack.begin());
                    nodeStack.pop_front();
                    root = std::make_unique<UnaryOperator>(*operatorStack.begin(), *operand);
                    
                    operatorStack.pop_front();
                    nodeStack.emplace_front(root);
                }
                else {
                    auto right = *(nodeStack.begin());
                    nodeStack.pop_front();
                    auto left = *(nodeStack.begin());
                    nodeStack.pop_front();

                    root = std::make_unique<BinaryOperator>(*operatorStack.begin(), *left, *right);
                    operatorStack.pop_front();
                    nodeStack.emplace_front(root);
                }
            }
            operatorStack.emplace_front(token);
        }

        else if(token == ")") {
            std::string stackTop = *operatorStack.begin();
            while(!operatorStack.empty() && stackTop != "("){
                auto left = *(nodeStack.begin());
                nodeStack.pop_front();
                auto right = *(nodeStack.begin());
                nodeStack.pop_front();

                root = std::make_unique<BinaryOperator>(*operatorStack.begin(), *left, *right);
                operatorStack.pop_front();
                nodeStack.emplace_front(root);
            }
            operatorStack.pop_front();
        }
    }

    root = *nodeStack.begin();
}


