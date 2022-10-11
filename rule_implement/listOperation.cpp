#include "listOperation.h"
#include <algorithm>
#include <random>
#include <chrono>

using std::reverse;
using std::shuffle;
using std::sort;
using std::default_random_engine;
using std::chrono::system_clock;

template <typename T>
ListOperation<T>::~ListOperation() {
	if (!list.empty())
		list.clear();
}

template <typename T>
Extend<T>::~Extend() {
	if (!target.empty()) {
		target.clear();
	}
}

template <typename T>
void Extend<T>::execute() {
	target.insert(target.end(), list.begin(), list.end());
}

template <typename T>
Reverse<T>::~Reverse() {

}

template <typename T>
void Reverse<T>::execute() {
	reverse(list.begin(), list.end());
}

template <typename T>
Shuffle<T>::~Shuffle() {

}

template <typename T>
void Shuffle<T>::execute() {
	unsigned int seed = system_clock::now().time_since_epoch().count();
	shuffle(list.begin(), list.end(), default_random_engine(seed));
}

template <typename T, typename T1>
Sort<T, T1>::~Sort() {

}

template <typename T, typename T1>
void Sort<T, T1>::setKey(T1 newKey) {
	key = newKey;
}

template <typename T, typename T1>
T1 Sort<T, T1>::getKey() const {
	return key;
}

template <typename T, typename T1>
void Sort<T, T1>::execute() {
	if (key == NULL) {
		sort(list.begin(), list.end());
	}
	//Note: Find the way to sort based on key
}

template <typename T>
Deal<T>::~Deal() {

}

template <typename T>
void Deal<T>::execute() {
	//Note: I assume dealing means sharing in this
	if (count <= list.size()) {
		for (int i = 0; i < count; i++) {
			if (find(to.begin(), to.end(), list[i]) != to.end())
				to.push_back(list[i]);
		}
	}
}

template <typename T>
Discard<T>::~Discard() {

}

template <typename T>
void Discard<T>::execute() {
	if (count <= size) {
		for (int i = 0; i < count; i++) {
			list.pop_back();
		}
	}
}