#pragma once
#include <iostream>
#include "rule.h"
#include "user.h"
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

template <typename T>
class HumanInput : public Rule {
public:
	HumanInput() : Rule(RuleType::HumanInput) {}
	virtual ~HumanInput();
	void setTo(const User& newUser);
	User getTo() const;
	void setPrompt(const string& newPrompt);
	string getPrompt() const;
	void setResult(const T& newResult);
	T getResult() const;
	void setTimeout(int newTimeout);
	int getTimeout() const;
	void setLitmitedWaitingResponse(bool newValue);
	bool getLimitedWaitingResponse() const;
private:
	User to;
	string prompt;
	T result;
	int timeout; //Note: this is in seconds
	bool limitedWaitingResponse;
};

template <typename T>
class InputChoice : public HumanInput {
public:
	virtual ~InputChoice();
	void addNewChoice(const T& newChoice);
	virtual void execute();
private:
	vector<T> choices;
};

template <typename T>
class InputText : public HumanInput {
public:
	virtual ~InputText();
	virtual void execute() const;
};

template <typename T>
class InputVote : public HumanInput {
public:
	virtual ~InputVote();
	void addNewChoice(const T& newChoice);
	int getVoteCountOf(const T& choice);
	virtual void execute() const;
private:
	vector<T> choices;
	map<T, int> result;
};