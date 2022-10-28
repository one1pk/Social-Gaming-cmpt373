#include "rules.h"

#include <algorithm>
#include <iostream>

// Foreach //

Foreach::Foreach(ElementSptr list, RuleVector rules) 
    : _list(list), _rules(rules) {
}

void Foreach::execute(ElementSptr element) const {
    std::cout << "* Foreach Rule *\n";

    ElementVector::iterator begin, end;
    if (_list->getIterator(begin, end)) {
        for (auto it = begin; it != end; it++) {
            ElementSptr element = *it;
            std::cout << "\nForeach loop: value = " << element->getInt() <<"\n";

            for (auto rule: _rules) {
                rule->execute(element);
            }
        }
    }
}

// ParallelFor //

ParallelFor::ParallelFor(std::vector<Connection> list, RuleVector rules) 
    : _list(list), _rules(rules) {
}

void ParallelFor::execute(ElementSptr element) const {
    std::cout << "* ParallelFor Rule *\n";

    for (auto player: _list) {
        for (auto rule: _rules) {
            rule->execute(std::make_shared<Element<int>>((int)player.id));
        }
    }
}

// When //

When::When(std::vector<std::pair<std::function<bool()>,RuleVector>> case_rules)
    : _case_rules(case_rules) {
}

void When::execute(ElementSptr element) const {
    std::cout << "* When Rule *\n";
    for (auto case_rule_pair: _case_rules) {
        if (case_rule_pair.first()) {
            std::cout << "Case Match!\nExecuting Case Rules\n";
            for (auto rule: case_rule_pair.second) {
                rule->execute();
            }
        } else {
            std::cout << "Case Fail, testing next case\n";
        }
    }
}

// Extend //

Extend::Extend(ElementSptr target, ElementSptr extension)
    : _target(target), _extension(extension) {
}

void Extend::execute(ElementSptr element) const {
    std::cout << "* Extend Rule *\n";

    _target->extend(_extension);
}

// Discard //

Discard::Discard(ElementSptr list, unsigned count)
    : _list(list), _count(count) {
}

void Discard::execute(ElementSptr element) const {
    std::cout << "* Discard Rule *\n";

    _list->discard(_count);
}

// Add //

Add::Add(int* to, int* value)
    : _to(to), _value(value) {
}

void Add::execute(ElementSptr element) const {
    std::cout << "* Add Rule\n";

    *_to += *_value;
}

// InputChoice //

InputChoice::InputChoice(std::string prompt, ElementVector choices/*, ElementSptr result, unsigned timeout_s*/)
    : _prompt(prompt), _choices(choices)/*, _result(result), _timeout_s(timeout_s) */ {
}

void InputChoice::execute(ElementSptr element) const {
    std::cout << "* InputChoice Rule *\n";

    std::string tmp = _prompt;
    if (element) {
        size_t replace_index = 0;
        std::string value_str = std::to_string(element->getInt());
        while ((replace_index = tmp.find("%p", replace_index)) != std::string::npos) {
            tmp.replace(replace_index, 2, value_str);
        }
    }
    std::cout << "prompt: " << tmp << "\n";

    std::cout << "Enter an index to select:\n";
    for (size_t i = 0; i < _choices.size(); i++) {
        std::cout << "["<<i<<"] " << _choices[i]->getString() << "\n";
    }
}

// GlobalMsg //

GlobalMsg::GlobalMsg(std::string msg)
    : _msg(msg) {
}

void GlobalMsg::execute(ElementSptr element) const {
    std::cout << "* GlobalMsg Rule *\n";

    std::string tmp = _msg;
    if (element) {
        size_t replace_index = 0;
        std::string value_str = std::to_string(element->getInt());
        while ((replace_index = tmp.find("%p", replace_index)) != std::string::npos) {
            tmp.replace(replace_index, 2, value_str);
        }
    }
    std::cout << "message: " << tmp << "\n";
}

// Scores //

Scores::Scores(ElementSptr player_maps, std::string attribute_key, bool ascending)
    : _player_maps(player_maps), _attribute_key(attribute_key), _ascending(ascending) {
}

void Scores::execute(ElementSptr element) const {
    std::cout << "* Scores Rule *\n";
    std::cout << "Scores are " << (_ascending? "(in ascedning order)\n" : "(in descedning order)\n");

    std::vector<int> scores;
    ElementVector::iterator begin, end;
    if (_player_maps->getIterator(begin, end)) {
        for (auto map = begin; map != end; map++) {
            scores.push_back((*map)->getMapElement(_attribute_key)->getInt());
        }
    }

    std::sort(scores.begin(), scores.end(), [=](int a, int b){ return (a<b && _ascending); });
    for (auto score: scores) {
        std::cout << score << "\n";
    }
}