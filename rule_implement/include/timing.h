#pragma once
#pragma once
#include <iostream>
#include "rule.h"
#include <vector>

using std::vector;

//Note: We could use Chrono library to work with these
class Timing : public Rule {
public:
	enum ModeType {
		Exact, AtMost, Track
	};
	Timing() : Rule(RuleType::Timing) {}
	void setDuration(int newDuration);
	int getDuration() const;
	void setMode(const ModeType& newMode);
	ModeType getMode() const;
	void setFlag(bool newValue);
	bool getFlag() const;
	void addNewExecutedRule(const Rule* newRule);
private:
	int duration; // Note: in seconds
	ModeType mode;
	vector<Rule*> rules;
	bool flag;
};

class Timer : public Timing {
public:
	enum ModeType {
		Exact, AtMost, Track
	};
	void setMode(const ModeType& newMode);
	ModeType getMode() const;
	void setFlag(bool newValue);
	bool getFlag() const;
private:
	ModeType mode;
	bool flag;
};