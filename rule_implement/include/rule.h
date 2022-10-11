#pragma once
#include <iostream>

using std::string;

class Rule {
public: 
	enum RuleType {
		ControlStructure, ListOperation, ArithmeticOperation, Timing, HumanInput, Output
	};

	Rule(const RuleType& newType) {	name = newType;	}
	virtual ~Rule() = 0;

	void setRuleName(const string& newName);
	string getRuleName() const;
	void setRuleType(const RuleType& newType);
	RuleType getRuleType() const;
	virtual void execute() = 0;
private:
	string name;
	RuleType type;
};