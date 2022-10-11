#include "timing.h"
#include <chrono>

using std::chrono::seconds;
using std::chrono::steady_clock;
using std::chrono::duration_cast;

Timing::~Timing() {
	if (!rules.empty()) {
		rules.clear();
	}
}
void Timing::setDuration(int newDuration) {
	duration = newDuration;
}
int Timing::getDuration() const {
	return duration;
}
void Timing::addNewExecutedRule(Rule* const newRule) {
	rules.push_back(newRule);
}

Timer::Timer() {
	this->setRuleName("Timer");
}

Timer::~Timer() {

}

void Timer::setMode(const ModeType& newMode) {
	mode = newMode;
}

Timer::ModeType Timer::getMode() const {
	return mode;
}


void Timer::setFlag(bool newValue) {
	flag = newValue;
}

bool Timer::getFlag() const {
	return flag;
}

void Timer::execute() {
	switch (mode) {
		case AtMost:
			break;
		case Exact:
			seconds lastingDuration(duration);
			auto startTime = steady_clock::now;
			auto currentTime = steady_clock::now;
			while (std::chrono::duration_cast<seconds>(currentTime - startTime) < lastingDuration) {
				for (Rule* rule : rules) {
					rule->execute();
				}
				currentTime = steady_clock::now;
			}
			break;
		case Track:
			flag = false;
			while (true) {
				for (Rule* rule : rules) {
					rule->execute();
				}
			}
			flag = true;
			break;
		default:
			return;

	}
}