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

ParallelFor::ParallelFor(std::shared_ptr<PlayerMap> players, RuleVector rules) 
    : _players(players), _rules(rules) {
}

void ParallelFor::execute(ElementSptr element) const {
    std::cout << "* ParallelFor Rule *\n";

    for(auto player = _players->begin(); player != _players->end(); player++) {
        for (auto rule: _rules) {
            rule->execute(player->second);
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
            break;
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

Add::Add(std::string to, ElementSptr value)
    : _to(to), _value(value) {
}

void Add::execute(ElementSptr element) const {
    std::cout << "* Add Rule *\n";

    element->getMapElement(_to)->addInt(_value->getInt());
}

// InputChoice //

void formatString(std::string& str, ElementSptr element) {
    if (element) {
        size_t open_brace = 0; 
        while ((open_brace = str.find("{", open_brace)) != std::string::npos) {
            size_t close_brace = str.find("}", open_brace);

            if (close_brace == open_brace+1) {
                str.replace(open_brace, 2, std::to_string(element->getInt()));
            } else {
                std::string value_str = std::to_string(
                    element->getMapElement(str.substr(open_brace+1, close_brace-open_brace-1))->getInt()
                );
                str.replace(open_brace, close_brace-open_brace+1, value_str);
            }
        }
    }
}

InputChoice::InputChoice(std::string prompt, ElementVector choices, std::string result/*, unsigned timeout_s*/)
    : _prompt(prompt), _choices(choices), _result(result)/*, _timeout_s(timeout_s) */ {
}

void InputChoice::execute(ElementSptr element) const {
    std::cout << "* InputChoice Rule *\n";

    std::string tmp = _prompt;
    formatString(tmp, element);
    std::cout << "prompt: " << tmp << "\n";

    std::cout << "Enter an index to select:\n";
    for (size_t i = 0; i < _choices.size(); i++) {
        std::cout << "["<<i<<"] " << _choices[i]->getString() << "\n";
    }

    // must get user response somehow //
    int chosen_index = 0;

    element->setMapElement(_result, _choices[chosen_index]);
}

// GlobalMsg //

GlobalMsg::GlobalMsg(std::string msg)
    : _msg(msg) {
}

void GlobalMsg::execute(ElementSptr element) const {
    std::cout << "* GlobalMsg Rule *\n";

    std::string tmp = _msg;
    formatString(tmp, element);
    std::cout << "message: " << tmp << "\n";
}

// Scores //

Scores::Scores(std::shared_ptr<PlayerMap> player_maps, std::string attribute_key, bool ascending)
    : _player_maps(player_maps), _attribute_key(attribute_key), _ascending(ascending) {
}

void Scores::execute(ElementSptr element) const {
    std::cout << "* Scores Rule *\n";
    std::cout << "Scores are " << (_ascending? "(in ascedning order)\n" : "(in descedning order)\n");

    std::vector<int> scores;
    for (auto player_map = _player_maps->begin(); player_map != _player_maps->end(); player_map++) {
        scores.push_back(player_map->second->getMapElement(_attribute_key)->getInt());
    }
    
    std::sort(scores.begin(), scores.end(), [=](int a, int b){ return (a<b && _ascending); });
    for (auto score: scores) {
        std::cout << score << "\n";
    }
}