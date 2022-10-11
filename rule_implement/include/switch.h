#pragma once
#include <iostream>
#include "controlStructure.h"
#include <vector>
#include <map>

using std::vector;
using std::map;

template <typename T>
class Switch : public ControlStructure {
public:
    Switch();
    virtual ~Switch();
    void setList(const vector<T>& newList);
    vector<T>& getList() const;
    void setValue(const T& newElement);
    T getValue() const;
    void addNewRuleToCase(const T& value, const vector<Rule*> rules);
    void setMapping(const vector<T> values); // Note: this utilizes the set of rules in the parent class

    virtual void execute();
private:
    T value;
    vector<T> list;
    map<T, vector<Rule*>> caseToRulesMap;
};