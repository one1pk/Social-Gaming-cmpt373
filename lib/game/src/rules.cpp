#include "rules.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <glog/logging.h>

// Foreach //

Foreach::Foreach(ElementSptr list, RuleVector rules) 
    : list(list), rules(rules) {
}

RuleStatus Foreach::execute(ElementSptr) {
    // initialize the elements vector from the dynamic list object
    // initialize the rule and list iterators
    LOG(INFO) << "* Foreach Rule *\n";

    if (!initialized) {
        elements = list->getVector();
        element = elements.begin();
        rule = rules.begin();
        initialized = true;
    }

    // execute the child rules for each element until input is required
    for (; element != elements.end(); element++) {
        for (; rule != rules.end(); rule++) {
            if ((*rule)->execute(*element) == RuleStatus::InputRequired) {
                return RuleStatus::InputRequired;
            }
        }
        rule = rules.begin();
    }

    // resets the rule iterators when rule is executed in a different context
    initialized = false;
    return RuleStatus::Done;
}

// ParallelFor //

ParallelFor::ParallelFor(std::shared_ptr<PlayerMap> player_maps, RuleVector rules) 
    : player_maps(player_maps), rules(rules) {
}

RuleStatus ParallelFor::execute(ElementSptr element) {
    // initialize the player rule iterators to the first rule
    LOG(INFO) << "* ParallelFor Rule *\n";
    if (!initialized) {
        for (auto& player_map: *player_maps) {
            player_rule_it[player_map.first] = rules.begin();
        }
        initialized = true;
    }

    RuleStatus status = RuleStatus::Done;
    // for each player, start rule execution at their stored rule iterator
    //  and execute the rules until an Input rule is encountered
    for(auto player_map = player_maps->begin(); player_map != player_maps->end(); player_map++) {
        for (auto& rule = player_rule_it[player_map->first]; rule != rules.end(); rule++) {
            if ((*rule)->execute(player_map->second) == RuleStatus::InputRequired) {
                status = RuleStatus::InputRequired;
                break;
            }
        }
    }

    // if rule execution is done this resets the rule iterators when rule is executed again in a different context 
    // otherwise the rule will continue where it left off after input is retrieved
    if (status == RuleStatus::Done) initialized = false;
    return status;
}

// When //

When::When(std::vector<std::pair<std::function<bool(ElementSptr)>,RuleVector>> _case_rules)
    : case_rules(_case_rules), case_rule_pair(case_rules.begin()),
      rule(case_rule_pair->second.begin()) {
}

RuleStatus When::execute(ElementSptr element) {
    // traverse the cases and execute the rules for the first case condition that returns true
    // a case_rule_pair consists of a case condition (a lambda returning bool) and a rule vector
    LOG(INFO) << "* When Rule *\n";
    for (; case_rule_pair != case_rules.end(); case_rule_pair++, rule = case_rule_pair->second.begin()) {
        if (case_rule_pair->first(element)) {
            LOG(INFO) << "Case Match!\nExecuting Case Rules\n";
            for (; rule != case_rule_pair->second.end(); rule++) {
                if ((*rule)->execute(element) == RuleStatus::InputRequired) {
                    return RuleStatus::InputRequired;
                }
            }
            break;
        } else {
            LOG(INFO) << "Case Fail, testing next case\n";
        }
    }

    // reset the rule state to be executed in a different context 
    case_rule_pair = case_rules.begin();
    rule = case_rule_pair->second.begin();
    return RuleStatus::Done;
}

// Extend //

Extend::Extend(ElementSptr target, std::function<ElementSptr(ElementSptr)> extension)
    : target(target), extension(extension) {
}

RuleStatus Extend::execute(ElementSptr element) {
    LOG(INFO) << "* Extend Rule *\n";
    target->extend(extension(element));
    return RuleStatus::Done;
}

// Discard //

Discard::Discard(ElementSptr list, std::function<size_t(ElementSptr)> count)
    : list(list), count(count) {
}

RuleStatus Discard::execute(ElementSptr element) {
    LOG(INFO) << "* Discard Rule *\n";
    list->discard(count(element));
    return RuleStatus::Done;
}

// Add //

Add::Add(std::string to, ElementSptr value)
    : to(to), value(value) {
}

RuleStatus Add::execute(ElementSptr element) {
    LOG(INFO) << "* Add Rule *\n";
    element->getMapElement(to)->addInt(value->getInt());
    return RuleStatus::Done;
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
    std::shared_ptr<std::deque<InputRequest>> input_requests,
    std::shared_ptr<std::map<Connection, InputResponse>> player_input,
    unsigned timeout_s)
    : prompt(prompt), choices(choices), result(result), 
    input_requests(input_requests), player_input(player_input), timeout_s(timeout_s) {
}

RuleStatus InputChoice::execute(ElementSptr player) {
    LOG(INFO) << "* InputChoiceRequest Rule *\n";
    Connection player_connection = player->getMapElement("connection")->getConnection();

    if (!awaitingInput[player_connection]) {
        // first execution of rule
        // format the input prompt and flag that input is required
        std::stringstream formatted_prompt(formatString(prompt, player));
        formatted_prompt << "Enter an index to select:\n";
        for (size_t i = 0; i < choices.size(); i++) {
            formatted_prompt << "["<<i<<"] " << choices[i]->getString() << "\n";
        }
        if (timeout_s) formatted_prompt << "Input will timeout in " << timeout_s << " seconds\n"; 

        InputRequest request;
        request.user = player_connection;
        request.prompt = formatted_prompt.str();
        request.num_choices = choices.size();
        request.type = InputType::Choice;
        if (timeout_s) {
            request.hasTimeout = true;
            request.timeout_ms = timeout_s*1000;
        }
        input_requests->push_back(request);

        awaitingInput[player_connection] = true;
        return RuleStatus::InputRequired;
    }
    // execution will continue from here after input is recieved

    int chosen_index;
    InputResponse input = player_input->at(player_connection);
    if (input.timedout) {
        // for now, if an input request times out, a default index of 0 is chosen
        chosen_index = 0;
    } else {
        chosen_index = std::stoi(input.response);
    }
    player->setMapElement(result, choices[chosen_index]);

    awaitingInput[player_connection] = false;
    return RuleStatus::Done;
}

// GlobalMsg //

GlobalMsg::GlobalMsg(std::string msg, std::shared_ptr<std::deque<std::string>> global_msgs)
    : msg(msg), global_msgs(global_msgs) {
}

RuleStatus GlobalMsg::execute(ElementSptr element) {
    LOG(INFO) << "* GlobalMsg Rule *\n";
    global_msgs->push_back(formatString(msg, element));
    return RuleStatus::Done;
}

// Scores //

Scores::Scores(std::shared_ptr<PlayerMap> player_maps, std::string attribute_key,
    bool ascending, std::shared_ptr<std::deque<std::string>> global_msgs)
    : player_maps(player_maps), attribute_key(attribute_key),
      ascending(ascending), global_msgs(global_msgs) {
}

RuleStatus Scores::execute(ElementSptr element) {
    LOG(INFO) << "* Scores Rule *\n";
    std::stringstream msg;
    msg << "\nScores are " << (ascending? "(in ascending order)\n" : "(in descending order)\n");

    std::vector<std::pair<int, uintptr_t>> scores;
    for (auto player_map = player_maps->begin(); player_map != player_maps->end(); player_map++) {
        scores.push_back({
            player_map->second->getMapElement(attribute_key)->getInt(), 
            player_map->first.id
        });
    }
    
    std::sort(scores.begin(), scores.end(), [=](auto a, auto b){ return (a.first<b.first && ascending); });
    for (auto score: scores) {
        msg << "player " << score.second << ": " << score.first << "\n";
    }

    global_msgs->push_back(msg.str());
    return RuleStatus::Done;
}