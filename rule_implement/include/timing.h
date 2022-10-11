#pragma once
#pragma once
#include <iostream>
#include "rule.h"
#include <vector>

using std::vector;

//Note: We could use Chrono library to work with these
class Timing : public Rule {
public:
	Timing() : Rule(RuleType::Timing) {}
	virtual ~Timing();
	void setDuration(int newDuration);
	int getDuration() const;
	void addNewExecutedRule(Rule* const newRule);
protected:
	int duration; // Note: in seconds
	vector<Rule*> rules;
};

class Timer : public Timing {
public:
	enum ModeType {
		Exact, AtMost, Track
	};
	Timer();
	virtual ~Timer();
	void setMode(const ModeType& newMode);
	ModeType getMode() const;
	void setFlag(bool newValue);
	bool getFlag() const;
	virtual void execute();
private:
	ModeType mode;
	bool flag;
};