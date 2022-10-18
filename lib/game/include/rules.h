#pragma once

#include "list.h"

#include <vector>

// Rule Interface //

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
    /*Foreach(List list, std::vector<Rule> rules);*/
    void execute();
};
