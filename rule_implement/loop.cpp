#include "loop.h"
#include "booleanExpression.h"

template <typename T>
Loop<T>::Loop() {

}

template <typename T>
Loop<T>::~Loop() {

}

template <typename T>
void Loop<T>::setUntilCondition(BooleanExpression::OperatorType opType, const T& left, const T& right) {
	untilCondition.setWholeExpression(opType, left, right);
}

template <typename T>
bool Loop<T>::getUntilConditionValue() const {
	return untilCondition.getReturnedValue();
}

template <typename T>
void Loop<T>::setWhileCondition(BooleanExpression::OperatorType opType, const T& left, const T& right) {
	whileCondition.setWholeExpression(opType, left, right);
}

template <typename T>
bool Loop<T>::getWhileConditionValue() const {
	return whileCondition.getReturnedValue();
}

template <typename T>
void Loop<T>::execute() {
	if (whileCondition.getOperatorType() == BooleanExpression::OperatorType::N/a) {
		if (untilCondition.getOperatorType() != BooleanExpression::OperatorType::N/a) {
			while (!untilCondition.getReturnedValue()) {
				for (Rule* rule : rules) {
					rule->execute();
				}
				//Note: Needs a way to update the condition here.
			}
		}
		else {
			return;
		}
	}
	else {
		while (whileCondition.getReturnedValue()) {
			for (Rule* rule : rules) {
				rule->execute();
			}
			//Note: Needs a way to update the condition here.
		}
	}
}