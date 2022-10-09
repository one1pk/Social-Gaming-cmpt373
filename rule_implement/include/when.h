#pragma once
#include <iostream>
#include "controlStructure.h"
#include "booleanExpression.h"
#include <vector>

using std::vector;

template <typename T>
class When : public ControlStructure {
public:
    When();
    ~When();
    void setCondition(BooleanExpression::OperatorType opType, const T& left, const T& right);
    bool getConditionValue() const;

    virtual void execute() const;
private:
    BooleanExpression condition;
};
