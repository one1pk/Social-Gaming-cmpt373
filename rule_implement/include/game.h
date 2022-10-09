#pragma once
#include <iostream>
#include <map>
#include <vector>
#include "rule.h"
#include "configuration.h"

using std::map;
using std::vector;

template <typename T, typename T1, typename T2, typename T3, typename T4>
class Game {
public:
private:
	Configuration gameConfig;
	map<string, T1> constants;
	map<string, T2> variables;
	map<string, T3> perPlayer;
	map<string, T4> perAudience;
	vector<Rule*> rules;
};
