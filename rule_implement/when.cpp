#include "when.h"
#include "booleanExpression.h"

template <typename T>
When<T>::When() {
	this->setName("When");
}

template <typename T>
When<T>::~When() {

}

template <typename T>
void When<T>::setCondition(BooleanExpression::OperatorType opType, const T& left, const T& right) {
	condition.setWholeExpression(opType, left, right);
}

template <typename T>
bool When<T>::getConditionValue() const {
	return condition;
}

template <typename T>
void When<T>::execute() {
	if (condition.getOperatorType() != BooleanExpression::OperatorType::N/a && condition.getReturnedValue()) {
		for (Rule* rule : rules) {
			rule->execute();
		}
	}
}