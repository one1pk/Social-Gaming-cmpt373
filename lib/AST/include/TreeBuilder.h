#include "ASTVisitor.h"

class TreeBuilder{
    public:
    TreeBuilder() = default;
    std::vector<std::string> split(std::string expression);
    std::shared_ptr<ASTNode> buildTree(std::string expression);

    bool isOperator(std::string value);
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

    
    std::shared_ptr<ASTNode> root;
};

std::vector<std::string> TreeBuilder::split(std::string expression) {
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
bool TreeBuilder::isOperator(std::string value){
    auto itB = std::find(binaryOperators.begin(), binaryOperators.end(), value);
    auto itU = std::find(unaryOperators.begin(), unaryOperators.end(), value);
    return itB != binaryOperators.end() || itU != unaryOperators.end();
}
bool TreeBuilder::isUnary(std::string value){
    auto itU = std::find(unaryOperators.begin(), unaryOperators.end(), value);
    return itU != unaryOperators.end();
}

int TreeBuilder::getPrecedence(std::string key){
    return precedenceMap.at(key);
}

std::shared_ptr<ASTNode> TreeBuilder::buildTree(std::string expression){
    std::vector<std::string> tokens = split(expression);
    std::deque<std::shared_ptr<ASTNode>> nodeStack;;
    std::deque<std::string> operatorStack;
    
    for(auto token : tokens){
        if(token == "(") {
            operatorStack.emplace_front(token);
        }
        else if (!isOperator(token)){
            root = std::make_unique<NameNode>(token);
            nodeStack.emplace_front(root);
        }
        else if(getPrecedence(token) > 0){
            std::string stackTop = *operatorStack.begin();

            while (!operatorStack.empty() && stackTop == "(" && getPrecedence(stackTop) >= getPrecedence(token) ){
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
    return root;
}

