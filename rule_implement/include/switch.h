#pragma once
#include <iostream>
#include "controlStructure.h"
#include <vector>
#include <map>

using std::vector;
using std::map;

template <typename T>
class ForEach : public ControlStructure {
public:
    ForEach();
    ~ForEach();
    void setList(const vector<T>& newList);
    vector<T>& getList() const;
    void setElement(const T& newElement);
    T getElement() const;
    void addNewRuleToCase(const T& value, const vector<Rule*> rules);
    void setMapping(const vector<T> values); // Note: this utilizes the set of rules in the parent class

    virtual void execute() const;
private:
    vector<T> list;
    T element;
    map<T, vector<Rule*>> caseToRulesMap;
};