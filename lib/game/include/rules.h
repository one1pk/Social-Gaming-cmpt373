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
    const std::vector<std::pair<std::function<bool()>,RuleVector>> _case_rules; // a rule list for every case
public: 
    When(std::vector<std::pair<std::function<bool()>,RuleVector>> case_rules);
    void execute(ElementSptr element = nullptr) const final;
};

// List Operations //

class Extend : public Rule {
    ElementSptr _target;
    ElementSptr _extension;
public:
    Extend(ElementSptr target, ElementSptr extension);
    void execute(ElementSptr element = nullptr) const final;
};

class Discard : public Rule {
    ElementSptr _list;
    unsigned _count;
public:
    Discard(ElementSptr list, unsigned count);
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
    // unsigned _timeout_s; // in seconds
public:
    InputChoice(std::string prompt, ElementVector choices, std::string result/*, unsigned timeout_s*/);
    void execute(ElementSptr element = nullptr) const final;
};

class GlobalMsg : public Rule {
    std::string _msg;
public:
    GlobalMsg(std::string msg);
    void execute(ElementSptr element = nullptr) const final;
};

class Scores : public Rule {
    std::shared_ptr<PlayerMap> _player_maps;
    std::string _attribute_key;
    bool _ascending;
public:
    Scores(std::shared_ptr<PlayerMap> player_maps, std::string attribute_key, bool ascending);
    void execute(ElementSptr element = nullptr) const final;
};