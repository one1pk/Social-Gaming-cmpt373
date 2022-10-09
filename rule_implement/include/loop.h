#pragma once
#include <iostream>
#include "controlStructure.h"
#include "booleanExpression.h"
#include <vector>

using std::vector;

template <typename T>
class Loop : public ControlStructure {
public:
    Loop();
    ~Loop();
    void setUntilCondition(BooleanExpression::OperatorType opType, const T& left, const T& right);
    bool getUntilConditionValue() const;
    void setWhileCondition(BooleanExpression::OperatorType opType, const T& left, const T& right);
    bool getWhileConditionValue() const;

    virtual void execute() const;
private:
    BooleanExpression untilCondition;
    BooleanExpression whileCondition;
};