#include "number.h"

void Number::setValue(int newValue) {
	value = newValue;
}
int Number::getValue() const {
	return value;
}

vector<int> Number::upFrom(int startingPoint) const {
	vector<int> resultList;
	if (startingPoint <= value) {
		for (int i = startingPoint; i <= value; i++) {
			resultList.push_back(i);
		}
	}
	return resultList;
}