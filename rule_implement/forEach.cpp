#include "forEach.h"

template <typename T>
ForEach<T>::ForEach() {
	this->setName("ForEach");
}

template <typename T>
ForEach<T>::~ForEach() {
	if (!list.empty()) {
		list.clear();
	}
}

template <typename T>
void ForEach<T>::setList(const vector<T>& newList) {
	if (!list.empty()) {
		list.clear();
	}
	list = vector<T>(newList);
}

template <typename T>
vector<T>& ForEach<T>::getList() const {
	return list;
}

template <typename T>
void ForEach<T>::setElement(const T& newElement) {
	element = newElement;
}

template <typename T>
T ForEach<T>::getElement() const {
	return element;
}

template <typename T>
void ForEach<T>::execute() {
	for (int i = 0; i < list.size(); i++) {
		setElement(list[i]);
		for (Rule* rule : rules) {
			rule->execute();
		}
	}
}