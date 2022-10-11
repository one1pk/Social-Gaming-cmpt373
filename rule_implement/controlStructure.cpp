#include "rule.h"
#include "controlStructure.h"
#include <vector>

ControlStructure::~ControlStructure() {
	if (!rules.empty())
		rules.clear();
}

void ControlStructure::addNewExecutedRule(Rule* const newRule) {
	rules.push_back(newRule);
}