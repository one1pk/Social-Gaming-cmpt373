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
    virtual ~When();
    void setCondition(BooleanExpression::OperatorType opType, const T& left, const T& right);
    bool getConditionValue() const;

    virtual void execute();
private:
    BooleanExpression condition;
};
