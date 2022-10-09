#pragma once
#include <iostream>
#include "rule.h"
#include <vector>

using std::vector;

class ControlStructure : public Rule {
public:
	ControlStructure() : Rule(RuleType::ControlStructure) {}
	void addNewExecutedRule(const Rule* newRule);
private:
	vector<Rule*> rules;
};
