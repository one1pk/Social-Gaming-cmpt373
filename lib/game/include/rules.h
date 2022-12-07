#pragma once

#include "list.h"

#include <vector>
#include <functional>
#include "ASTVisitor.h"
#include "ExpressionResolver.h"

class Rule;
using RuleSptr = std::shared_ptr<Rule>;
using RuleVector = std::vector<RuleSptr>;

enum RuleStatus {
    Done,
    InputRequired,
};

enum InputType {
    Choice,
    Text,
    Vote,
};

struct InputRequest {
    InputRequest(User user, std::string prompt, InputType type,
                 unsigned num_choices, bool has_timeout = false, unsigned timeout_ms = 0)
        : user(user), prompt(prompt), type(type), 
        num_choices(num_choices), has_timeout(has_timeout), timeout_ms(timeout_ms) {
    }
    User user;
    std::string prompt;
    InputType type;
    unsigned num_choices; // doesn't apply for InputType::Text
    bool has_timeout = false;
    unsigned timeout_ms = 0;
};

struct InputResponse {
    std::string response;
    bool timedout = false;
};

// Rule Interface //

class Rule {
public:
    virtual ~Rule() {}
    virtual RuleStatus execute(ElementMap& game_state) = 0;
    
protected:
    ExpressionResolver resolver;
};

// Control Structures//

class Foreach : public Rule {
private:
    std::shared_ptr<ASTNode> list_expression_root;
    std::string element_name;

    ElementSptr list;
    RuleVector rules;

    ElementVector elements;
    ElementVector::iterator element;
    RuleVector::iterator rule;
    bool initialized = false;

public:
    Foreach(std::shared_ptr<ASTNode> list_expression_root, std::string element_name, RuleVector rules);
    RuleStatus execute(ElementMap& game_state) final;
};

class ParallelFor : public Rule {
    std::shared_ptr<PlayerMap> player_maps;
    RuleVector rules;
    std::string element_name;

    std::map<User, RuleVector::iterator> player_rule_it;
    bool initialized = false;

public:
    ParallelFor(std::shared_ptr<PlayerMap> player_maps, RuleVector rules, std::string element_name);
    RuleStatus execute(ElementMap& game_state) final;
};

class Switch : public Rule {
    using Condition_Rules = std::vector<std::pair<std::shared_ptr<ASTNode>, RuleVector>>;
    std::shared_ptr<ASTNode> value_expression_root;
    std::shared_ptr<ASTNode> list_expression_root;
    Condition_Rules conditionExpression_rule_pairs;
    Condition_Rules::iterator conditionExpression_rule_pair;
    RuleVector::iterator rule;
public: 
    Switch(std::shared_ptr<ASTNode> value_expression_root, std::shared_ptr<ASTNode> list_expression_root, Condition_Rules conditonExpression_rule_pairs);
    RuleStatus execute(ElementMap& game_state) final;
};

class When : public Rule {
    using Condition_Rules = std::vector<std::pair<std::shared_ptr<ASTNode>, RuleVector>>;
    // a vector of case-rules pairs
    // containes a rule list for every case
    // a case is a function (lambda) that returns a bool
    Condition_Rules conditionExpression_rule_pairs;
    Condition_Rules::iterator conditionExpression_rule_pair;
    RuleVector::iterator rule;
public: 
    When(Condition_Rules conditonExpression_rule_pairs);
    RuleStatus execute(ElementMap& game_state) final;
};

// List Operations //

class Extend : public Rule {
    std::shared_ptr<ASTNode> target_expression_root;
    std::shared_ptr<ASTNode> extension_expression_root;
public:
    Extend(std::shared_ptr<ASTNode> target_expression_root, std::shared_ptr<ASTNode> extension_expression_root);
    RuleStatus execute(ElementMap& game_state) final;
};

class Discard : public Rule {
    std::shared_ptr<ASTNode> list_expression_root;
    std::shared_ptr<ASTNode> count_expression_root;
public:
    Discard(std::shared_ptr<ASTNode> list_expression_root,  std::shared_ptr<ASTNode> count_expression_root);
    RuleStatus execute(ElementMap& game_state) final;
};

class Sort : public Rule {
    std::shared_ptr<ASTNode> list_expression_root;
    std::optional<std::string> key;
public:
    Sort(std::shared_ptr<ASTNode> list_expression_root, std::optional<std::string> key = std::nullopt);
    RuleStatus execute(ElementMap& game_state) final;
};

class Shuffle : public Rule {
    std::shared_ptr<ASTNode> list_expression_root;
public:
    Shuffle(std::shared_ptr<ASTNode> list_expression_root);
    RuleStatus execute(ElementMap& game_state) final;
};

class Deal : public Rule {
    std::shared_ptr<ASTNode> from_expression_root;
    std::shared_ptr<ASTNode> to_expression_root;
    int count;
public:
    Deal(std::shared_ptr<ASTNode> from_expression_root, std::shared_ptr<ASTNode> to_expression_root, int count);
    RuleStatus execute(ElementMap& game_state) final;
};

// Arithmetic //

class Add : public Rule {
    std::shared_ptr<ASTNode> element_expression_root;
    std::shared_ptr<ASTNode> value_expression_root;
public: 
    Add(std::shared_ptr<ASTNode> element_expression_root,  std::shared_ptr<ASTNode> value_expression_root);
    RuleStatus execute(ElementMap& game_state) final;
};

// Input/ Output //

class InputChoice : public Rule {
    std::string prompt;
    std::shared_ptr<ASTNode> element_to_replace_root;

    std::shared_ptr<ASTNode> choices_expression_root;
    ElementVector choices;

    std::shared_ptr<std::deque<InputRequest>> input_requests;
    std::shared_ptr<std::map<User, InputResponse>> player_input;
    
    std::string result;
    unsigned timeout_s; // in seconds

    std::map<User, bool> awaiting_input;
public:
    InputChoice(std::string prompt, 
                std::shared_ptr<ASTNode> element_to_replace_root,
                std::shared_ptr<ASTNode> choices_expression_root,
                std::shared_ptr<std::deque<InputRequest>> input_requests,
                std::shared_ptr<std::map<User, InputResponse>> player_input,
                std::string result, unsigned timeout_s = 0);
    RuleStatus execute(ElementMap& game_state) final;
};

class GlobalMsg : public Rule {
    std::string msg;
    std::shared_ptr<ASTNode> element_to_replace_root;
    std::shared_ptr<std::deque<std::string>> global_msgs;
    
public:
    GlobalMsg(std::string msg, std::shared_ptr<ASTNode> element_to_replace_root,
              std::shared_ptr<std::deque<std::string>> global_msgs);
    RuleStatus execute(ElementMap& game_state) final;
};

class Scores : public Rule {
    std::shared_ptr<PlayerMap> player_maps;
    std::string attribute_key;
    bool ascending;
    std::shared_ptr<std::deque<std::string>> global_msgs;
public:
    Scores(std::shared_ptr<PlayerMap> player_maps, std::string attribute_key, 
           bool ascending, std::shared_ptr<std::deque<std::string>> global_msgs);
    RuleStatus execute(ElementMap& game_state) final;
};