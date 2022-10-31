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
    virtual void execute(ElementSptr element = nullptr) const = 0;
};

// Control Structures//

class Foreach : public Rule {
private:
    ElementSptr _list;
    RuleVector _rules;
public:
    Foreach(ElementSptr list, RuleVector rules);
    void execute(ElementSptr element = nullptr) const final;
};

class ParallelFor : public Rule {
    std::shared_ptr<PlayerMap> _players;
    RuleVector _rules;
public:
    ParallelFor(std::shared_ptr<PlayerMap> players, RuleVector rules);
    void execute(ElementSptr element = nullptr) const final;
};

class When : public Rule {
    const std::vector<std::pair<std::function<bool(ElementSptr)>,RuleVector>> _case_rules; // a rule list for every case
public: 
    When(std::vector<std::pair<std::function<bool(ElementSptr)>,RuleVector>> case_rules);
    void execute(ElementSptr element = nullptr) const final;
};

// List Operations //

class Extend : public Rule {
    ElementSptr _target;
    std::function<ElementSptr(ElementSptr)> _extension;
public:
    Extend(ElementSptr target, std::function<ElementSptr(ElementSptr)> extension);
    void execute(ElementSptr element = nullptr) const final;
};

class Discard : public Rule {
    ElementSptr _list;
    std::function<size_t(ElementSptr)> _count;
public:
    Discard(ElementSptr list, std::function<size_t(ElementSptr)> count);
    void execute(ElementSptr element = nullptr) const final;
};

// Arithmetic //

class Add : public Rule {
    std::string _to;
    ElementSptr _value;
public: 
    Add(std::string to, ElementSptr value);
    void execute(ElementSptr element = nullptr) const final;
};

// Input/ Output //

class InputChoice : public Rule {
    std::string _prompt;
    ElementVector _choices;
    std::string _result;
    std::shared_ptr<std::deque<Message>> _player_msgs;
    // unsigned _timeout_s; // in seconds
public:
    InputChoice(std::string prompt, ElementVector choices, std::string result,
                std::shared_ptr<std::deque<Message>> player_msgs/*, unsigned timeout_s*/);
    void execute(ElementSptr player) const final;
};

class GlobalMsg : public Rule {
    std::string _msg;
    std::shared_ptr<std::deque<std::string>> _global_msgs;
public:
    GlobalMsg(std::string msg,
              std::shared_ptr<std::deque<std::string>> global_msgs);
    void execute(ElementSptr element = nullptr) const final;
};

class Scores : public Rule {
    std::shared_ptr<PlayerMap> _player_maps;
    std::string _attribute_key;
    bool _ascending;
    std::shared_ptr<std::deque<std::string>> _global_msgs;
public:
    Scores(std::shared_ptr<PlayerMap> player_maps, std::string attribute_key, 
           bool ascending, std::shared_ptr<std::deque<std::string>> global_msgs);
    void execute(ElementSptr element = nullptr) const final;
};