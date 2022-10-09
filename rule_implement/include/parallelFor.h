#pragma once
#include <iostream>
#include "controlStructure.h"
#include <vector>

using std::vector;

template <typename T>
class ParallelFor : public ControlStructure {
public:
    ParallelFor();
    ~ParallelFor();
    void setList(const vector<T>& newList);
    vector<T>& getList() const;
    void setElement(const T& newElement);
    T getElement() const;

    virtual void execute() const;
private:
    vector<T> list;
    T element;
};