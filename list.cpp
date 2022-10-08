#include "list.h"

List::List(std::vector<std::map<std::string, std::any>> _list)
    : list(_list) {}

void List::extend(std::map<std::string, std::any> element) { }

void List::extend(List other_list) { }

void List::reverse() { }

void List::shuffle() { }

void List::sort() { }

void List::sort(std::string attribute) { }

void List::deal(List &to, unsigned count) { }

void List::discard(unsigned count) { }

size_t List::size() { }

std::vector<std::any> List::sublist(std::string attribute) { }

std::map<std::string, std::any> List::element(std::string attribute, std::any value) { }

bool List::contains(std::string attribute, std::any value) {}