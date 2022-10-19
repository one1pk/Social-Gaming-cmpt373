#include "rules.h"

#include <algorithm>

Foreach::Foreach(List _list, std::vector<Rule> _rules) 
    : list(_list), rules(_rules) {}

void Foreach::execute() {
    
}
