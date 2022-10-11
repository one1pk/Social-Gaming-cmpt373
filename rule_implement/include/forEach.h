#pragma once
#include <iostream>
#include "controlStructure.h"
#include <vector>

using std::vector;

template <typename T>
class ForEach : public ControlStructure {
public:
    ForEach();
    virtual ~ForEach();
    void setList(const vector<T>& newList);
    vector<T>& getList() const;
    void setElement(const T& newElement);
    T getElement() const;

    virtual void execute();
private:
    vector<T> list;
    T element;
};