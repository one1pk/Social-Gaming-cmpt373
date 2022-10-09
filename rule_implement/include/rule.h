#pragma once
#include <iostream>

using std::string;

class Rule {
public: 
	enum RuleType {
		ControlStructure, ListOperation, ArithmeticOperation, Timing, HumanInput, Output
	};

	Rule(const RuleType& newType) {	name = newType;	}

	void setRuleName(const string& newName);
	string getRuleName() const;
	void setRuleType(const RuleType& newType);
	RuleType getRuleType() const;
	virtual void execute() const = 0;
private:
	string name;
	RuleType type;
};