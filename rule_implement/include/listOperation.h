#pragma once
#include <iostream>
#include "rule.h"
#include <vector>

using std::vector;

template <typename T>
class ListOperation : public Rule {
public:
	ListOperation() : Rule(RuleType::ListOperation) {}
	virtual ~ListOperation();
	virtual void execute() = 0;
protected:
	vector<T> list;
};

template <typename T>
class Extend : public ListOperation {
public: 
	virtual ~Extend();
	virtual void execute();
private:
	vector<T> target;
};

template <typename T>
class Reverse : public ListOperation {
public:
	virtual ~Reverse();
	virtual void execute();
};

template <typename T>
class Shuffle : public ListOperation {
public: 
	virtual ~Shuffle();
	virtual void execute();
};

template <typename T, typename T1>
class Sort : public ListOperation {
public: 
	Sort() { key = NULL; }
	virtual ~Sort();
	void setKey(T1 newKey);
	T1 getKey() const;
	virtual void execute();
private: 
	T1 key;
};

template <typename T>
class Deal : public ListOperation {
public: 
	virtual ~Deal();
	virtual void execute();
private:
	//vector<T> from;
	vector<T> to; // Note: This utilizes the list in the parent class as from
	int count;
};

template <typename T>
class Discard : public ListOperation {
public: 
	virtual ~Discard();
	virtual void execute();
private:
	//vector<T> from;
	//Note : This utilizes the list in the parent class as from
	int count;
};
