#pragma once
#include <iostream>
#include "rule.h"

class ArithmeticOperation : public Rule {
public:
	ArithmeticOperation() : Rule(RuleType::ArithmeticOperation) {}
	void setTo(int newTo);
	int getTo() const;
	void setValue(int newValue);
	int getValue() const;
private:
	int to;
	int value;
};

class Add : public ArithmeticOperation {
public: 
	virtual void execute() const;
};

class NumericalAttribute : public ArithmeticOperation {
public:
	virtual void execute() const;
};
