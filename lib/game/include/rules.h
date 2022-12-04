#pragma once

#include "list.h"

#include <vector>
#include <functional>
#include "ASTVisitor.h"
#include "ExpressionResolver.h"

class Rule;
typedef std::shared_ptr<Rule> RuleSptr; // shared pointer to a rule object
typedef std::vector<std::shared_ptr<Rule>> RuleVector; // a vector of shared pointers to rule objects

// Rule Interface //

class Rule {
public:
    virtual ~Rule() {}

    bool execute(ElementMap elementsMap, ElementSptr element = nullptr) {
        if (executed) {
            return true;
        }
        return executeImpl(element, elementsMap);
    }
    void reset() {
        executed = false;
        resetImpl();
    }

//for testing
ElementSptr getList();

ExpressionResolver resolver;

private:
    virtual bool executeImpl(ElementSptr element, ElementMap elementsMap) = 0;
    virtual void resetImpl() {}
    
protected:
    bool executed = false;
};

// Control Structures//

class Foreach : public Rule {
private:
    std::shared_ptr<ASTNode> listExpressionRoot;
    ElementSptr list;
    RuleVector rules;
    std::string elementName;

    ElementVector elements;
    ElementVector::iterator element;
    RuleVector::iterator rule;
    bool initialized = false;

public:
    Foreach(std::shared_ptr<ASTNode> listExpressionRoot, RuleVector _rules, std::string elementName);
    bool executeImpl(ElementSptr element, ElementMap elementsMap) final;
    void resetImpl() final;
};

class ParallelFor : public Rule {
    std::shared_ptr<PlayerMap> player_maps;
    RuleVector rules;
    std::string elementName;

    std::map<Connection, RuleVector::iterator> player_rule_it;
    bool initialized = false;

public:
    ParallelFor(std::shared_ptr<PlayerMap> player_maps, RuleVector rules, std::string elementName);
    bool executeImpl(ElementSptr element, ElementMap elementsMap) final;
    void resetImpl() final;
};

class When : public Rule {
    std::shared_ptr<ASTNode> conditionRoot;
    // a vector of condtion-rules pairs
    // containes a rule list for every case
    // a condition is the root of the expression tree for that condition
    
    std::vector<std::pair<std::shared_ptr<ASTNode>, RuleVector>>::iterator condition_rule_pair;
    
    RuleVector::iterator rule;
    bool match = false;

public: 
    std::vector<std::pair<std::shared_ptr<ASTNode>, RuleVector>> condition_rule_pairs;
    When() = default;
    void set();
    bool executeImpl(ElementSptr element, ElementMap elementsMap) final;
    void resetImpl() final;
};

// List Operations //

class Extend : public Rule {
    std::shared_ptr<ASTNode> targetExpressionRoot;
    ElementSptr target;

    std::shared_ptr<ASTNode> extensionExpressionRoot;
    ElementSptr extensionList;

public:
    Extend(std::shared_ptr<ASTNode> targetExpressionRoot, std::shared_ptr<ASTNode> extensionExpressionRoot);
    bool executeImpl(ElementSptr element, ElementMap elementsMap) final;
};

class Discard : public Rule {
    std::shared_ptr<ASTNode> listExpressionRoot;
    ElementSptr list;
    std::shared_ptr<ASTNode> countExpressionRoot;
    int resolvedCount;

public:
    Discard(std::shared_ptr<ASTNode> listExpressionRoot,  std::shared_ptr<ASTNode> countExpressionRoot);
    bool executeImpl(ElementSptr element, ElementMap elementsMap) final;
};

// Arithmetic //

class Add : public Rule {
    std::shared_ptr<ASTNode> toExpressionRoot;
    ElementSptr to;
    ElementSptr value;
public: 
    Add(std::shared_ptr<ASTNode> toExpressionRoot, ElementSptr value);
    bool executeImpl(ElementSptr element, ElementMap elementsMap) final;
};

// Input/ Output //

class InputChoice : public Rule {
    std::string prompt;
    std::shared_ptr<ASTNode> elementToReplace;
    
    std::shared_ptr<ASTNode> choicesExpressionRoot;
    ElementVector choices;

    unsigned timeout_s; // in seconds
    std::string result;
    std::shared_ptr<std::deque<Message>> player_msgs;
    std::shared_ptr<std::map<Connection, std::string>> player_input;
    std::map<Connection, bool> alreadySentInput;

public:
    InputChoice(std::string prompt, std::shared_ptr<ASTNode> elementToReplace, std::shared_ptr<ASTNode> expressionRoot, 
                unsigned timeout_s, std::string result,
                std::shared_ptr<std::deque<Message>> player_msgs,
                std::shared_ptr<std::map<Connection, std::string>> player_input);
    bool executeImpl(ElementSptr element, ElementMap elementsMap) final;
};

class GlobalMsg : public Rule {
    std::string msg;
    std::shared_ptr<std::deque<std::string>> global_msgs;
    std::shared_ptr<ASTNode> elementToReplace;
public:
    GlobalMsg(std::string msg,
              std::shared_ptr<std::deque<std::string>> global_msgs, 
              std::shared_ptr<ASTNode> elementToReplace);
    bool executeImpl(ElementSptr element, ElementMap elementsMap) final;
};

class Scores : public Rule {
    std::shared_ptr<PlayerMap> player_maps;
    std::string attribute_key;
    bool ascending;
    std::shared_ptr<std::deque<std::string>> global_msgs;
public:
    Scores(std::shared_ptr<PlayerMap> player_maps, std::string attribute_key, 
           bool ascending, std::shared_ptr<std::deque<std::string>> global_msgs);
    bool executeImpl(ElementSptr element, ElementMap elementsMap) final;
};