#include "humanInput.h"
#include "output.h"
#include <chrono>

using std::cin;
using std::chrono::seconds;
using std::chrono::steady_clock;
using std::chrono::duration_cast;

template <typename T>
HumanInput<T>::~HumanInput() {

}

template <typename T>
void HumanInput<T>::setTo(const User& newUser) {
	to = newUser;
}

template <typename T>
User HumanInput<T>::getTo() const {
	return to;
}

template <typename T>
void HumanInput<T>::setPrompt(const string& newPrompt) {
	prompt = newPrompt;
}

template <typename T>
string HumanInput<T>::getPrompt() const {
	return prompt;
}

template <typename T>
void HumanInput<T>::setResult(const T& newResult) {
	result = newResult;
}

template <typename T>
T HumanInput<T>::getResult() const {
	return result;
}

template <typename T>
void HumanInput<T>::setTimeout(int newTimeout) {
	timeout = newTimeout;
}

template <typename T>
int HumanInput<T>::getTimeout() const {
	return timeout;
}

template <typename T>
void HumanInput<T>::setLitmitedWaitingResponse(bool newValue) {
	limitedWaitingResponse = newValue;
}

template <typename T>
bool HumanInput<T>::getLimitedWaitingResponse() const {
	return limitedWaitingResponse;
}

template <typename T>
InputChoice<T>::~InputChoice() {
	if (!choices.empty()) {
		choices.clear();
	}
}

template <typename T>
void InputChoice<T>::addNewChoice(const T& newChoice) {
	choices.push_back(newChoice);
}

template <typename T>
void InputChoice<T>::execute() {
	Message promptOut(to, prompt);
	promptOut.execute();
	string response;
	map<string, T> stringToChoice;
	if (getLimitedWaitingResponse()) {
		auto startTime = steady_clock::now;
		auto currentTime = steady_clock::now;
		while (std::chrono::duration_cast<seconds>(currentTime - startTime) < getTimeout()) {
			cin >> response; // Note: We could put choices simply in string type, and then map them to the real choices
							// Therefore, we should implement it here
			currentTime = steady_clock::now;
			// Note: This could be an infinite loop
		}
	}
	else {
		cin >> response; // Note: We could put choices simply in string type, and then map them to the real choices
	}					// Therefore, we should implement it here
	result = stringToChoice.at(response);
}

template <typename T>
InputText<T>::~InputText() {

}

template <typename T>
void InputText<T>::execute() const {
	Message promptOut(to, prompt);
	promptOut.execute();
	if (getLimitedWaitingResponse()) {
		auto startTime = steady_clock::now;
		auto currentTime = steady_clock::now;
		while (std::chrono::duration_cast<seconds>(currentTime - startTime) < getTimeout()) {
			cin >> result; 
			currentTime = steady_clock::now;
			// Note: This could be an infinite loop
		}
	}
	else {
		cin >> result;
	}
}

template <typename T>
InputVote<T>::~InputVote() {
	if (!choices.empty()) {
		choices.clear();
	}
	if (!result.empty()) {
		result.clear();
	}
}

template <typename T>
void InputVote<T>::addNewChoice(const T& newChoice) {
	choices.push_back(newChoice);
}

template <typename T>
int InputVote<T>::getVoteCountOf(const T& choice) {
	return result.at(choice);
}

template <typename T>
void InputVote<T>::execute() const {
	Message promptOut(to, prompt);
	promptOut.execute();
	string response;
	map<string, T> stringToChoice;
	if (getLimitedWaitingResponse()) {
		auto startTime = steady_clock::now;
		auto currentTime = steady_clock::now;
		while (std::chrono::duration_cast<seconds>(currentTime - startTime) < getTimeout()) {
			cin >> response;
			currentTime = steady_clock::now;
			// Note: This could be an infinite loop
		}
	}
	else {
		cin >> response;
	}
	map<string, T>::iterator itr = result.find(stringToChoice.at(response));
	if (itr != result.end()) {
		itr->second += 1;
	}
}