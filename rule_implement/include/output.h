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
class Output : public Rule {
public:
	Output() : Rule(RuleType::Output) {}
	void addNewRecipient(const User& newRecipient);	
	void setValue(const string& newPrompt);
	T getValue() const;
private:
	vector<User> to;
	T value; //Note: This is used as score in the class Score	
};

template <typename T>
class Message : public Output {
public:
	void addNewRecipient(const User& newRecipient);
	virtual void execute() const;
private:
	vector<User> to;
};

template <typename T>
class GlobalMessage : public Output {
public:
	virtual void execute() const;
};

template <typename T>
class Scores : public Output {
public:
	void setAscending(bool newValue);
	bool getAscending() const;
	virtual void execute() const;
private:
	//Note: We use the member variable 'value' from the parent class as 'score' for this class
	bool ascending;
};