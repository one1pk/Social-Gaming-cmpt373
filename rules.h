#pragma once

#include "list.h"
#include <string>
#include <vector>

// Rule Interace //

class Rule {
public:
    virtual ~Rule();
    virtual void execute() const = 0;
};

// Control Structures//

class Foreach : public Rule {
    const List list;
    const std::vector<Rule> rules;
public:
    Foreach(List _list, std::vector<Rule> _rules);
    void execute();
};

class Loop : public Rule {
    bool until;
    // ADD: condition
    const std::vector<Rule> rules;
public:
    Loop(bool _until, std::vector<Rule> _rules);
    void execute();
};

class Inparallel : public Rule {
    const std::vector<Rule> rules;
public:  
    Inparallel(std::vector<Rule> _rules);
    void execute();
};

class Parallelfor : public Rule {
    const List list;
    const std::vector<Rule> rules;
public:
    Parallelfor(List _list, std::vector<Rule> _rules);
    void execute();
};

class Switch : public Rule {
    const std::any value;
    const List list;
    const std::vector<std::pair<std::any,std::vector<Rule>>> case_rules; // a rule list for every case
public:   
    Switch(std::any _value, List _list, std::vector<std::pair<std::any,std::vector<Rule>>> _case_rules);
    void execute();
};

class When : public Rule {
    const std::any value;
    const std::vector<std::pair<std::any,std::vector<Rule>>> case_rules; // a rule list for every case
public: 
    When(std::any _value, std::vector<std::pair<std::any,std::vector<Rule>>> _case_rules);
    void execute();
};

// Arithmetic Operations //

class Add : public Rule {
    const int &to;
    const int value;
public:
    Add(int &_to, int _value);
    void execute();
};

class Countup : public Rule {
    const int value;
public:
    Countup(int _value); // count up from const or literal 
    void execute();
};

// Timing //

