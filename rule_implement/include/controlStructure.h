#pragma once
#include <iostream>
#include "rule.h"
#include <vector>

using std::vector;

class ControlStructure : public Rule {
public:
	ControlStructure() : Rule(RuleType::ControlStructure) {}
	virtual ~ControlStructure();
	void addNewExecutedRule(Rule* const newRule);
private:
	vector<Rule*> rules;
};
