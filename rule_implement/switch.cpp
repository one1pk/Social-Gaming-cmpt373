#include "switch.h"

template <typename T>
Switch<T>::Switch() {
	this->setName("Switch");
}

template <typename T>
Switch<T>::~Switch() {
	if (!list.empty()) {
		list.clear();
	}
	if (!caseToRulesMap.empty()) {
		caseToRulesMap.clear();
	}
}

template <typename T>
void Switch<T>::setList(const vector<T>& newList) {
	if (!list.empty()) {
		list.clear();
	}
	list = vector<T>(newList);
}

template <typename T>
vector<T>& Switch<T>::getList() const {
	return list;
}

template <typename T>
void Switch<T>::setValue(const T& newValue) {
	value = newValue;
}

template <typename T>
T Switch<T>::getValue() const {
	return value;
}

template <typename T>
void Switch<T>::addNewRuleToCase(const T& value, const vector<Rule*> rules) {
	caseToRulesMap.insert({ value, rules });
}

template <typename T>
void Switch<T>::setMapping(const vector<T> values) { // Note: this utilizes the set of rules in the parent class

}

template <typename T>
void Switch<T>::execute() {

}