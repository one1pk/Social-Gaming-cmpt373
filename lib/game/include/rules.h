#pragma once

#include "list.h"

#include <vector>
#include <iostream>
// Rule Interface //

class Rule {
public:
    Rule();
    virtual ~Rule();
    virtual void execute();
private:
    const List list;
    const std::vector<Rule> rules;
};


// Control Structures//

class Foreach : public Rule {
private:
    const List list;
    const std::vector<Rule> rules;
public:
    Foreach(List _list, std::vector<Rule> _rules);
    void execute();
};
