#pragma once
#include <iostream>
#include <vector>

using std::vector;

// Note: This class is used for the Numerical-Attributes feature

class Number {
public:
	void setValue(int newValue);
	int getValue() const;
	vector<int> upFrom(int startingPoint) const;
private:
	int value;
};