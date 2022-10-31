#include "rules.h"

#include <algorithm>
#include <iostream>
#include <sstream>

// Foreach //

Foreach::Foreach(ElementSptr list, RuleVector rules) 
    : _list(list), _rules(rules) {
}

void Foreach::execute(ElementSptr) const {
    std::cout << "* Foreach Rule *\n";

    ElementVector elements = _list->getVector();
    for (auto element: elements) {
        std::cout << "\nForeach loop: value = " << element->getInt() <<"\n";

        for (auto rule: _rules) {
            rule->execute(element);
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

When::When(std::vector<std::pair<std::function<bool(ElementSptr)>,RuleVector>> case_rules)
    : _case_rules(case_rules) {
}

void When::execute(ElementSptr element) const {
    std::cout << "* When Rule *\n";
    for (auto case_rule_pair: _case_rules) {
        if (case_rule_pair.first(element)) {
            std::cout << "Case Match!\nExecuting Case Rules\n";
            for (auto rule: case_rule_pair.second) {
                rule->execute(element);
            }
            break;
        } else {
            std::cout << "Case Fail, testing next case\n";
        }
    }
}

// Extend //

Extend::Extend(ElementSptr target, std::function<ElementSptr(ElementSptr)> extension)
    : _target(target), _extension(extension) {
}

void Extend::execute(ElementSptr element) const {
    std::cout << "* Extend Rule *\n";

    _target->extend(_extension(element));
}

// Discard //

Discard::Discard(ElementSptr list, std::function<size_t(ElementSptr)> count)
    : _list(list), _count(count) {
}

void Discard::execute(ElementSptr element) const {
    std::cout << "* Discard Rule *\n";

    _list->discard(_count(element));
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

std::string formatString(std::string_view str, ElementSptr element) {
    std::string res(str);
    if (element) {
        size_t open_brace = 0; 
        while ((open_brace = res.find("{", open_brace)) != std::string::npos) {
            size_t close_brace = res.find("}", open_brace);

            if (close_brace == open_brace+1) {
                res.replace(open_brace, 2, element->getString());
            } else {
                std::string value_str = element->getMapElement(res.substr(open_brace+1, close_brace-open_brace-1))->getString();
                res.replace(open_brace, close_brace-open_brace+1, value_str);
            }
        }
    }
    return res;
}

InputChoice::InputChoice(std::string prompt, ElementVector choices, std::string result,
    std::shared_ptr<std::deque<Message>> player_msgs/*, unsigned timeout_s*/)
    : _prompt(prompt), _choices(choices), _result(result),
    _player_msgs(player_msgs) /*, _timeout_s(timeout_s) */ {
}

void InputChoice::execute(ElementSptr player) const {
    std::cout << "* InputChoice Rule *\n";

    std::stringstream msg(formatString(_prompt, player));

    msg << "Enter an index to select:\n";
    for (size_t i = 0; i < _choices.size(); i++) {
        msg << "["<<i<<"] " << _choices[i]->getString() << "\n";
    }

    _player_msgs->push_back({ player->getMapElement("connection")->getConnection(), msg.str() });


    // must get user response somehow //
    int chosen_index = 0;
    player->setMapElement(_result, _choices[chosen_index]);


    std::stringstream confirmation;
    confirmation << "You Chose: " << _choices[chosen_index]->getString() << "\n";
    _player_msgs->push_back({ player->getMapElement("connection")->getConnection(), confirmation.str() });
}

// GlobalMsg //

GlobalMsg::GlobalMsg(std::string msg, std::shared_ptr<std::deque<std::string>> global_msgs)
    : _msg(msg), _global_msgs(global_msgs) {
}

void GlobalMsg::execute(ElementSptr element) const {
    std::cout << "* GlobalMsg Rule *\n";

    _global_msgs->push_back(formatString(_msg, element));
}

// Scores //

Scores::Scores(std::shared_ptr<PlayerMap> player_maps, std::string attribute_key,
    bool ascending, std::shared_ptr<std::deque<std::string>> global_msgs)
    : _player_maps(player_maps), _attribute_key(attribute_key),
    _ascending(ascending), _global_msgs(global_msgs) {
}

void Scores::execute(ElementSptr element) const {
    std::cout << "* Scores Rule *\n";

    std::stringstream msg;
    msg << "Scores are " << (_ascending? "(in ascedning order)\n" : "(in descedning order)\n");

    std::vector<std::pair<int, uintptr_t>> scores;
    for (auto player_map = _player_maps->begin(); player_map != _player_maps->end(); player_map++) {
        scores.push_back({
            player_map->second->getMapElement(_attribute_key)->getInt(), 
            player_map->first.id
        });
    }
    
    std::sort(scores.begin(), scores.end(), [=](auto a, auto b){ return (a.first<b.first && _ascending); });
    for (auto score: scores) {
        msg << "player " << score.second << ": " << score.first << "\n";
    }

    _global_msgs->push_back(msg.str());
}