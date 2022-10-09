#pragma once
#include <iostream>

using std::string;

class User {
public: 
	enum Role {
		Owner, Player, Audience
	};
	User();
	User(const Role& role);
	~User();
	void setRole(const Role& role);
	void getRole() const;

private: 
	string name;
	Role role; 
};
