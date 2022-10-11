#include "arithmeticOperation.h"

ArithmeticOperation::~ArithmeticOperation() {

}

void ArithmeticOperation::setTo(int newTo) {
	to = newTo;
}

int ArithmeticOperation::getTo() const {
	return to;
}

void ArithmeticOperation::setValue(int newValue) {
	value = newValue;
}

int ArithmeticOperation::getValue() const {
	return value;
}

void Add::execute() {
	to += value;
}

/*void NumericalAttribute::setNumber(int newNumber) {
	number = newNumber;
}

int NumericalAttribute::getNumber() const {
	return number;
}

vector<int> NumericalAttribute::getResultList() const {
	return resultList;
}

void NumericalAttribute::execute() {
	
}*/