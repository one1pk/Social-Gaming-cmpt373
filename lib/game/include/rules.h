#pragma once

#include "list.h"

#include <vector>
#include <functional>

class Rule;
typedef std::shared_ptr<Rule> RuleSptr; // shared pointer to a rule object
typedef std::vector<std::shared_ptr<Rule>> RuleVector; // a vector of shared pointers to rule objects

// Rule Interface //

class Rule {
public:
    virtual ~Rule() {}

    bool execute(ElementSptr element = nullptr) {
        if (executed) {
            return true;
        }
        return executeImpl(element);
    }
    void reset() {
        executed = false;
        resetImpl();
    }

private:
    virtual bool executeImpl(ElementSptr element) = 0;
    virtual void resetImpl() {}

protected:
    bool executed = false;
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
    bool executeImpl(ElementSptr element) final;
    void resetImpl() final;
};

class ParallelFor : public Rule {
    std::shared_ptr<PlayerMap> player_maps;
    RuleVector rules;
    std::map<Connection, RuleVector::iterator> player_rule_it;
    bool initialized = false;
public:
    ParallelFor(std::shared_ptr<PlayerMap> player_maps, RuleVector rules);
    bool executeImpl(ElementSptr element) final;
    void resetImpl() final;
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
    bool executeImpl(ElementSptr element) final;
    void resetImpl() final;
};

// List Operations //

class Extend : public Rule {
    ElementSptr target;
    std::function<ElementSptr(ElementSptr)> extension;
public:
    Extend(ElementSptr target, std::function<ElementSptr(ElementSptr)> extension);
    bool executeImpl(ElementSptr element) final;
};

class Discard : public Rule {
    ElementSptr list;
    std::function<size_t(ElementSptr)> count;
public:
    Discard(ElementSptr list, std::function<size_t(ElementSptr)> count);
    bool executeImpl(ElementSptr element) final;
};

// Arithmetic //

class Add : public Rule {
    std::string to;
    ElementSptr value;
public: 
    Add(std::string to, ElementSptr value);
    bool executeImpl(ElementSptr element) final;
};

// Input/ Output //

class InputChoice : public Rule {
    std::string prompt;
    ElementVector choices;
    unsigned timeout_s; // in seconds
    std::string result;
    std::shared_ptr<std::deque<Message>> player_msgs;
    std::shared_ptr<std::map<Connection, std::string>> player_input;
    std::map<Connection, bool> awaitingInput;
public:
    InputChoice(std::string prompt, ElementVector choices, 
                       unsigned timeout_s, std::string result,
                       std::shared_ptr<std::deque<Message>> player_msgs,
                       std::shared_ptr<std::map<Connection, std::string>> player_input);
    bool executeImpl(ElementSptr element) final;
};

class GlobalMsg : public Rule {
    std::string msg;
    std::shared_ptr<std::deque<std::string>> global_msgs;
public:
    GlobalMsg(std::string msg,
              std::shared_ptr<std::deque<std::string>> global_msgs);
    bool executeImpl(ElementSptr element) final;
};

class Scores : public Rule {
    std::shared_ptr<PlayerMap> player_maps;
    std::string attribute_key;
    bool ascending;
    std::shared_ptr<std::deque<std::string>> global_msgs;
public:
    Scores(std::shared_ptr<PlayerMap> player_maps, std::string attribute_key, 
           bool ascending, std::shared_ptr<std::deque<std::string>> global_msgs);
    bool executeImpl(ElementSptr element) final;
};