#pragma once

#include "list.h"

#include <vector>
#include <functional>

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
    Connection user;
    std::string prompt;
    InputType type;
    unsigned num_choices; // doesn't apply for InputType::Text
    bool hasTimeout = false;
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
    virtual RuleStatus execute(ElementSptr element = nullptr) = 0;
};

// Control Structures//

class Foreach : public Rule {
private:
    ElementSptr list;
    ElementVector elements;
    ElementVector::iterator element;
    RuleVector rules;
    RuleVector::iterator rule;
    bool initialized = false;
public:
    Foreach(ElementSptr list, RuleVector rules);
    RuleStatus execute(ElementSptr element) final;
};

class ParallelFor : public Rule {
    std::shared_ptr<PlayerMap> player_maps;
    RuleVector rules;
    std::map<Connection, RuleVector::iterator> player_rule_it;
    bool initialized = false;
public:
    ParallelFor(std::shared_ptr<PlayerMap> player_maps, RuleVector rules);
    RuleStatus execute(ElementSptr element) final;
};

class When : public Rule {
    // a vector of case-rules pairs
    // containes a rule list for every case
    // a case is a function (lambda) that returns a bool
    std::vector<std::pair<std::function<bool(ElementSptr)>,RuleVector>> case_rules; 
    std::vector<std::pair<std::function<bool(ElementSptr)>,RuleVector>>::iterator case_rule_pair;
    RuleVector::iterator rule;
    bool match = false;
public: 
    When(std::vector<std::pair<std::function<bool(ElementSptr)>,RuleVector>> case_rules);
    RuleStatus execute(ElementSptr element) final;
};

// List Operations //

class Extend : public Rule {
    ElementSptr target;
    std::function<ElementSptr(ElementSptr)> extension;
public:
    Extend(ElementSptr target, std::function<ElementSptr(ElementSptr)> extension);
    RuleStatus execute(ElementSptr element) final;
};

class Discard : public Rule {
    ElementSptr list;
    std::function<size_t(ElementSptr)> count;
public:
    Discard(ElementSptr list, std::function<size_t(ElementSptr)> count);
    RuleStatus execute(ElementSptr element) final;
};

class Sort : public Rule {
    ElementSptr list;
    std::optional<std::string> key;
public:
    Sort(ElementSptr list, std::optional<std::string> key = std::nullopt);
    RuleStatus execute(ElementSptr element) final;
};

class Deal : public Rule {
    ElementSptr from;
    ElementSptr to;
    int count;
public:
    Deal(ElementSptr from, ElementSptr to, int count);
    RuleStatus execute(ElementSptr element) final;
};

// Arithmetic //

class Add : public Rule {
    std::string to;
    ElementSptr value;
public: 
    Add(std::string to, ElementSptr value);
    RuleStatus execute(ElementSptr element) final;
};

// Input/ Output //

class InputChoice : public Rule {
    std::string prompt;
    ElementVector choices;
    std::string result;
    std::shared_ptr<std::deque<InputRequest>> input_requests;
    std::shared_ptr<std::map<Connection, InputResponse>> player_input;
    unsigned timeout_s; // in seconds

    std::map<Connection, bool> awaitingInput;
public:
    InputChoice(std::string prompt, ElementVector choices, std::string result,
                std::shared_ptr<std::deque<InputRequest>> input_requests,
                std::shared_ptr<std::map<Connection, InputResponse>> player_input,
                unsigned timeout_s = 0);
    RuleStatus execute(ElementSptr element) final;
};

class GlobalMsg : public Rule {
    std::string msg;
    std::shared_ptr<std::deque<std::string>> global_msgs;
public:
    GlobalMsg(std::string msg,
              std::shared_ptr<std::deque<std::string>> global_msgs);
    RuleStatus execute(ElementSptr element) final;
};

class Scores : public Rule {
    std::shared_ptr<PlayerMap> player_maps;
    std::string attribute_key;
    bool ascending;
    std::shared_ptr<std::deque<std::string>> global_msgs;
public:
    Scores(std::shared_ptr<PlayerMap> player_maps, std::string attribute_key, 
           bool ascending, std::shared_ptr<std::deque<std::string>> global_msgs);
    RuleStatus execute(ElementSptr element) final;
};