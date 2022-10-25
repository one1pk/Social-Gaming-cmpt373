#include "rules.h"

#include <algorithm>

// Foreach //

Foreach::Foreach(ElementSptr list, std::vector<RuleUptr> rules) 
    : _list(list), _rules(std::move(rules)) 
{}

#include <iostream>
void Foreach::execute() const {
    std::cout << "Foreach Rule\n";

    ElementVector::iterator begin, end;
    if (_list->getIterator(begin, end)) {
        std::cout << "Foreach Check\n";

        for (auto it = begin; it != end; it++) {
            std::cout << "Foreach Loop\n";
            std::cout << "value: " << (*it)->getInt() <<"\n";
        }
    }
}

// ParallelFor //

ParallelFor::ParallelFor(ElementSptr list, std::vector<RuleUptr> rules) 
    : _list(list), _rules(std::move(rules)) 
{}

void ParallelFor::execute() const {
    
}

// When //

When::When() {}

void When::execute() const {

}

// Extend //

Extend::Extend(ElementSptr target, ElementSptr extension)
    : _target(target), _extension(extension)
{}

void Extend::execute() const {
    _target->extend(_extension);
}

// Discard //

Discard::Discard(ElementSptr list, unsigned count)
    : _list(list), _count(count)
{}

void Discard::execute() const {
    _list->discard(_count);
}

// Add //

Add::Add(int* to, int* value)
    : _to(to), _value(value)
{}

void Add::execute() const {
    *_to += *_value;
}

// InputChoice //

InputChoice::InputChoice(uintptr_t to, std::string prompt, ElementSptr choices, ElementSptr result, unsigned timeout_s)
    : _to(to), _prompt(prompt), _choices(choices), _result(result), _timeout_s(timeout_s) 
{}

void InputChoice::execute() const {

}

// GlobalMsg //

GlobalMsg::GlobalMsg() {}

void GlobalMsg::execute() const {

}

// Scores //

Scores::Scores(bool ascending)
    : _ascending(ascending)
{}

void Scores::execute() const {

}