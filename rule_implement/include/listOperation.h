#pragma once
#include <iostream>
#include "rule.h"
#include <vector>

using std::vector;

template <typename T>
class ListOperation : public Rule {
public:
	ListOperation() : Rule(RuleType::ListOperation) {}

	virtual void execute() const;
private:
	vector<T> list;
};

template <typename T>
class Extend : public ListOperation {
public: 
	virtual void execute() const;
private:
	vector<T> target;
};

template <typename T>
class Reverse : public ListOperation {
public:
	virtual void execute() const;
};

template <typename T>
class Shuffle : public ListOperation {
public: 
	virtual void execute() const;
};

template <typename T, typename T1>
class Sort : public ListOperation {
public: 
	virtual void execute() const;
private: 
	T1 key;
};

template <typename T>
class Deal : public ListOperation {
public: 
	virtual void execute() const;
private:
	//vector<T> from;
	vector<T> to; // Note: This utilizes the list in the parent class as from
	int count;
};

template <typename T>
class Discard : public ListOperation {
public: 
	virtual void execute() const;
private:
	//vector<T> from;
	//Note : This utilizes the list in the parent class as from
	int count;
};
