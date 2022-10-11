#include "rule.h"

void Rule::setRuleName(const string& newName) {
	name = newName;
}
string Rule::getRuleName() const {
	return name;
}
void Rule::setRuleType(const RuleType& newType) {
	type = newType;
}
Rule::RuleType Rule::getRuleType() const {
	return type;
}