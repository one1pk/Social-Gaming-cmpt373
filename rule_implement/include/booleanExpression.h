#pragma once
#include <iostream>

template <typename T>
class BooleanExpression {
public:
	enum OperatorType {
		N/a, &&, ||, !, ==, !=, >, <, >=, <=
	};
	void setOperatorType(OperatorType opType);
	OperatorType getOperatorType() const;
	void setLeftOperand(const T& left);
	void setRightOperand(const T& right);
	void setWholeExpression(OperatorType opType, const T& left, const T& right);
	bool getReturnedValue() const;
private:
	T leftOperand, rightOperand; // Note: the ! operator needs only the rightOperand
	OperatorType operatorType;
	bool returnedValue;
};