#pragma once
#include <iostream>
#include <vector>
#include "rule.h"

using std::vector;

class ArithmeticOperation : public Rule {
public:
	ArithmeticOperation() : Rule(RuleType::ArithmeticOperation) {}
	virtual ~ArithmeticOperation();

	void setTo(int newTo);
	int getTo() const;
	void setValue(int newValue);
	int getValue() const;
	virtual void execute() = 0;
protected:
	int to;
	int value;
};

class Add : public ArithmeticOperation {
public: 
	virtual void execute();
};

/*class NumericalAttribute : public ArithmeticOperation {
public:
	virtual void execute();
	void setNumber(int newNumber);
	int getNumber() const;
	vector<int> getResultList() const;
public: 
	int number; 
	vector<int> resultList;
};*/
