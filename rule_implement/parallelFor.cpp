#include "parallelFor.h"

template <typename T>
ParallelFor<T>::ParallelFor() {
	this->setName("Parallelfor");
}

template <typename T>
ParallelFor<T>::~ParallelFor() {
	if (!list.empty()) {
		list.clear();
	}
}

template <typename T>
void ParallelFor<T>::setList(const vector<T>& newList) {
	if (!list.empty()) {
		list.clear();
	}
	list = vector<T>(newList);
}

template <typename T>
vector<T>& ParallelFor<T>::getList() const {
	return list;
}

template <typename T>
void ParallelFor<T>::setElement(const T& newElement) {
	element = newElement; 
}

template <typename T>
T ParallelFor<T>::getElement() const {
	return element;
}

template <typename T>
void ParallelFor<T>::execute() {
	for (int i = 0; i < list.size(); i++) {
		setElement(list[i]);
		//Note: we could use the library 'execution' here
	}
}