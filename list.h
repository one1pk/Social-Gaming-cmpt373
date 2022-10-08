#pragma once

#include <string>
#include <vector>
#include <map>
#include <any>

class List {
public:
    List(std::vector<std::map<std::string, std::any>> _list);
    void extend(std::map<std::string, std::any> element);
    void extend(List other_list);
    void reverse();
    void shuffle();
    void sort();
    void sort(std::string attribute);
    void deal(List &to, unsigned count);
    void discard(unsigned count);

    size_t size();
    std::vector<std::any> sublist(std::string attribute);
    std::map<std::string, std::any> element(std::string attribute, std::any value);
    bool contains(std::string attribute, std::any value);
    // collect ??

private:
    std::vector<std::map<std::string, std::any>> list;
    std::vector<std::string> elements;
};
