#pragma once

#include "list.h"

#include <vector>

// Rule Interface //

class Rule {
public:
    virtual ~Rule() {}
    virtual void execute() const = 0;
};

typedef std::unique_ptr<Rule> RuleUptr;

// Control Structures//

class Foreach : public Rule {
private:
    const ElementSptr _list;
    const std::vector<RuleUptr> _rules;
public:
    Foreach(ElementSptr list, std::vector<RuleUptr> rules);
    void execute() const final;
};

class ParallelFor : public Rule {
    const ElementSptr _list;
    const std::vector<RuleUptr> _rules;
public:
    ParallelFor(ElementSptr list, std::vector<RuleUptr> rules);
    void execute() const final;
};

class When : public Rule {
    // const std::any value;
    // const std::vector<std::pair<std::any,std::vector<Rule>>> case_rules; // a rule list for every case
public: 
    When(/*std::any _value, std::vector<std::pair<std::any,std::vector<Rule>>> _case_rules*/);
    void execute() const final;
};

// List Operations //

class Extend : public Rule {
    ElementSptr _target;
    ElementSptr _extension;
public:
    Extend(ElementSptr target, ElementSptr extension);
    void execute() const final;
};

class Discard : public Rule {
    ElementSptr _list;
    unsigned _count;
public:
    Discard(ElementSptr list, unsigned count);
    void execute() const final;
};

// Arithmetic //

class Add : public Rule {
    int* _to;
    int* _value;
public: 
    Add(int* to, int* value);
    void execute() const final;
};

// Input/ Output //

class InputChoice : public Rule {
    uintptr_t _to;
    std::string _prompt;
    ElementSptr _choices;
    ElementSptr _result;
    unsigned _timeout_s; // in seconds
public:
    InputChoice(uintptr_t to, std::string prompt, ElementSptr choices, ElementSptr result, unsigned timeout_s);
    void execute() const final;
};

class GlobalMsg : public Rule {
    // std::string _msg;
public:
    GlobalMsg();
    void execute() const final;
};

class Scores : public Rule {
    // std::vector<Element*> _score;
    bool _ascending;
public:
    Scores(bool ascending);
    void execute() const final;
};