#pragma once
#include <iostream>
#include <map>

using std::string;
using std::map;

template <typename T>
class Configuration {
public:
	struct BoundedInt {
		int min;
		int max;
	};
	void setName(const string& newName);
	string getName() const;
	void setAudience(bool newValue);
	bool getAudience() const;
	void setPlayerCount(BoundedInt newPlayerCount);
	BoundedInt getPlayerCount() const;
private:
	string name;
	bool audience;
	BoundedInt playerCount;
	map<string, T> setup;
};
