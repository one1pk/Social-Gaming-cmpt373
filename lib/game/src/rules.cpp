#include "rules.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <glog/logging.h>

// Foreach //

Foreach::Foreach(std::shared_ptr<ASTNode> list_expression_root, std::string element_name, RuleVector rules) 
    : list_expression_root(list_expression_root), element_name(element_name), rules(rules) {
}

RuleStatus Foreach::execute(ElementMap& game_state) {
    LOG(INFO) << "* Foreach Rule *";

    // initialize the elements vector from the dynamic list object
    // initialize the rule and list iterators
    if (!initialized) {
        list_expression_root->accept(resolver, game_state);
        list = resolver.getResult();
        elements = list->getVector();
        element = elements.begin();
        rule = rules.begin();
        initialized = true;
    }

    // execute the child rules for each element until input is required
    for (; element != elements.end(); element++) {
        // add element to game_state so that subrules can find it (eg. round, weapon, etc)
        game_state[element_name] = *element;

        for (; rule != rules.end(); rule++) {
            if ((*rule)->execute(game_state) == RuleStatus::InputRequired) {
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

ParallelFor::ParallelFor(std::shared_ptr<PlayerMap> player_maps, RuleVector rules, std::string element_name) 
    : player_maps(player_maps), rules(rules), element_name(element_name) {
}

RuleStatus ParallelFor::execute(ElementMap& game_state) {
    LOG(INFO) << "* ParallelFor Rule *";

    // initialize the player rule iterators to the first rule
    if (!initialized) {
        for (auto& [player_connection, _]: *player_maps) {
            player_rule_it[player_connection] = rules.begin();
        }
        initialized = true;
    }

    RuleStatus status = RuleStatus::Done;
    // for each player, start rule execution at their stored rule iterator
    //  and execute the rules until an Input rule is encountered
    for(auto& [player_connection, player]: *player_maps) {
        // set map element "player" to current player list so that subrules can find it
        game_state[element_name] = player;

        for (auto& rule = player_rule_it[player_connection]; rule != rules.end(); rule++) {
            if ((*rule)->execute(game_state) == RuleStatus::InputRequired) {
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

// Switch //

Switch::Switch(std::shared_ptr<ASTNode> value_expression_root, std::shared_ptr<ASTNode> list_expression_root, Condition_Rules _conditionExpression_rule_pairs)
    : value_expression_root(value_expression_root), list_expression_root(list_expression_root), conditionExpression_rule_pairs(_conditionExpression_rule_pairs), 
    conditionExpression_rule_pair(conditionExpression_rule_pairs.begin()),
    rule(conditionExpression_rule_pair->second.begin()) {
}

RuleStatus Switch::execute(ElementMap& game_state) {
    LOG(INFO) << "* Switch Rule *";

    value_expression_root->accept(resolver, game_state);
    auto value = resolver.getResult();
    for (; conditionExpression_rule_pair != conditionExpression_rule_pairs.end(); conditionExpression_rule_pair++) {
        auto& [condition_root, rules] = *conditionExpression_rule_pair;
        rule = rules.begin();

        condition_root->accept(resolver, game_state);
        auto switch_case = resolver.getResult();    
        if (switch_case == value) {
            LOG(INFO) << "Case Match!" << std::endl << "Executing Case Rules";

            for (; rule != rules.end(); rule++) {
                if ((*rule)->execute(game_state) == RuleStatus::InputRequired) {
                    return RuleStatus::InputRequired;
                }
            }
            break;
        } else {
            LOG(INFO) << "Case Fail, testing next case";
        }
    }

    conditionExpression_rule_pair = conditionExpression_rule_pairs.begin();
    rule = conditionExpression_rule_pair->second.begin();
    return RuleStatus::Done;
}

// When //

When::When(Condition_Rules _conditionExpression_rule_pairs)
    : conditionExpression_rule_pairs(_conditionExpression_rule_pairs), 
    conditionExpression_rule_pair(conditionExpression_rule_pairs.begin()),
    rule(conditionExpression_rule_pair->second.begin()) {
}

RuleStatus When::execute(ElementMap& game_state) {
    LOG(INFO) << "* When Rule *";

    // traverse the cases and execute the rules for the first case condition that returns true
    // a conditionExpression_rule_pairs consists of a case condition (an expression tree root) and a rule vector
    for (; conditionExpression_rule_pair != conditionExpression_rule_pairs.end(); conditionExpression_rule_pair++) {
        auto& [condition_root, rules] = *conditionExpression_rule_pair;
        rule = rules.begin();

        condition_root->accept(resolver, game_state);
        bool caseMatch = resolver.getResult()->getBool();    
        if (caseMatch) {
            LOG(INFO) << "Case Match!" << std::endl << "Executing Case Rules";

            for (; rule != rules.end(); rule++) {
                if ((*rule)->execute(game_state) == RuleStatus::InputRequired) {
                    return RuleStatus::InputRequired;
                }
            }
            break;
        } else {
            LOG(INFO) << "Case Fail, testing next case";
        }
    }

    // reset the rule state to be executed in a different context 
    conditionExpression_rule_pair = conditionExpression_rule_pairs.begin();
    rule = conditionExpression_rule_pair->second.begin();
    return RuleStatus::Done;
}

// Extend //

Extend::Extend(std::shared_ptr<ASTNode> target_expression_root, std::shared_ptr<ASTNode> extension_expression_root)
    : target_expression_root(target_expression_root), extension_expression_root(extension_expression_root) {
}

RuleStatus Extend::execute(ElementMap& game_state) {
    LOG(INFO) << "* Extend Rule *";
    target_expression_root->accept(resolver, game_state);
    auto target = resolver.getResult();

    extension_expression_root->accept(resolver, game_state);
    auto extension = resolver.getResult();

    target->extend(extension);
    return RuleStatus::Done;
}

// Discard //

Discard::Discard(std::shared_ptr<ASTNode> list_expression_root,  std::shared_ptr<ASTNode> count_expression_root)
    : list_expression_root(list_expression_root), count_expression_root(count_expression_root) {
}

RuleStatus Discard::execute(ElementMap& game_state) {
    LOG(INFO) << "* Discard Rule *";
    list_expression_root->accept(resolver, game_state);
    auto list = resolver.getResult();

    count_expression_root->accept(resolver, game_state);
    auto count = resolver.getResult()->getInt();

    list->discard(count);
    return RuleStatus::Done;
}

// Shuffle //

Shuffle::Shuffle(std::shared_ptr<ASTNode> list_expression_root)
    : list_expression_root(list_expression_root){   
}

RuleStatus Shuffle::execute(ElementMap& game_state) {
    LOG(INFO) << "* Shuffle Rule *";

    list_expression_root->accept(resolver, game_state);
    auto list = resolver.getResult();

    list->shuffle();
    return RuleStatus::Done;
}

// Sort //

Sort::Sort(std::shared_ptr<ASTNode> list_expression_root, std::optional<std::string> key)
    : list_expression_root(list_expression_root), key(key) {   
}

RuleStatus Sort::execute(ElementMap& game_state) {
    LOG(INFO) << "* Sort Rule *";

    list_expression_root->accept(resolver, game_state);
    auto list = resolver.getResult();

    list->sortList(key);
    return RuleStatus::Done;
}

// Deal //

Deal::Deal(std::shared_ptr<ASTNode> from_expression_root, std::shared_ptr<ASTNode> to_expression_root, int count)
    : from_expression_root(from_expression_root), to_expression_root(to_expression_root), count(count) {
}

RuleStatus Deal::execute(ElementMap& game_state) {
    LOG(INFO) << "* Deal Rule *";

    from_expression_root->accept(resolver, game_state);
    auto from = resolver.getResult();

    to_expression_root->accept(resolver, game_state);
    auto to = resolver.getResult();

    from->deal(to, count);
    return RuleStatus::Done;
}

// Add //

Add::Add(std::shared_ptr<ASTNode> element_expression_root,  std::shared_ptr<ASTNode> value_expression_root)
    : element_expression_root(element_expression_root), value_expression_root(value_expression_root) {
}

RuleStatus Add::execute(ElementMap& game_state) {
    LOG(INFO) << "* Add Rule *";
    element_expression_root->accept(resolver, game_state);
    auto element = resolver.getResult();

    value_expression_root->accept(resolver, game_state);
    auto value = resolver.getResult()->getInt();

    element->addInt(value);
    return RuleStatus::Done;
}

// InputChoice //

//replaces string in {} with element
std::string formatString(std::string msg, ExpressionResolver& resolver) {
    size_t open_brace = 0; 
    
    if ((open_brace = msg.find("{", open_brace)) != std::string::npos) {
        size_t close_brace = msg.find("}", open_brace);
        std::string resolvedString;

        if(resolver.getResult()->type == VECTOR){
            ElementVector resolvedStringVector = resolver.getResult()->getVector();
            for(auto it = resolvedStringVector.begin(); it != resolvedStringVector.end(); it++){
                if(it != resolvedStringVector.begin())
                    resolvedString += ", ";
                resolvedString += (*it)->getString();
            }
        }
        else
            resolvedString = resolver.getResult()->getString();

        msg.replace(open_brace, close_brace - open_brace + 1, resolvedString);
    }
    return msg + "\n";
}

InputChoice::InputChoice(std::string prompt, 
                        std::shared_ptr<ASTNode> element_to_replace_root,
                        std::shared_ptr<ASTNode> choices_expression_root,
                        std::shared_ptr<std::deque<InputRequest>> input_requests,
                        std::shared_ptr<std::map<Connection, InputResponse>> player_input,
                        std::string result, unsigned timeout_s)
    : prompt(prompt), element_to_replace_root(element_to_replace_root),
    choices_expression_root(choices_expression_root),
    input_requests(input_requests), player_input(player_input), 
    result(result), timeout_s(timeout_s) {
}

RuleStatus InputChoice::execute(ElementMap& game_state) {
    LOG(INFO) << "* InputChoiceRequest Rule *";
    Connection player_connection = game_state["player"]->getMapElement("connection")->getConnection();

    if (!awaiting_input[player_connection]) {
        // first execution of rule

        // resolve choices
        /// TODO: for choices, weapons.name should resolve to weapons.sublist.name
        choices_expression_root->accept(resolver, game_state);
        choices = resolver.getResult()->getVector();

        // format the input prompt
        element_to_replace_root->accept(resolver, game_state);

        std::stringstream formatted_prompt = std::stringstream(formatString(prompt, resolver));
        formatted_prompt << formatted_prompt.str() << "Enter an index to select:\n";
        for (size_t i = 0; i < choices.size(); i++) {
            formatted_prompt << "["<<i<<"] " << choices[i]->getString() << "\n";
        }
        if (timeout_s) formatted_prompt << "Input will timeout in " << timeout_s << " seconds\n"; 

        // create an input request and flag that input is required
        input_requests->emplace_back(
            player_connection,
            formatted_prompt.str(),
            InputType::Choice,
            choices.size(),
            timeout_s,
            timeout_s*1000
        );
        awaiting_input[player_connection] = true;
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
    game_state["player"]->setMapElement(result, choices[chosen_index]);

    awaiting_input[player_connection] = false;
    return RuleStatus::Done;
}


// GlobalMsg //

GlobalMsg::GlobalMsg(std::string msg, std::shared_ptr<ASTNode> element_to_replace_root, 
                    std::shared_ptr<std::deque<std::string>> global_msgs)
    : msg(msg), element_to_replace_root(element_to_replace_root), global_msgs(global_msgs) {
}

RuleStatus GlobalMsg::execute(ElementMap& game_state) {
    LOG(INFO) << "* GlobalMsg Rule *";

    element_to_replace_root->accept(resolver, game_state);
    
    global_msgs->push_back(formatString(msg, resolver));
    return RuleStatus::Done;
}

// Scores //

Scores::Scores(std::shared_ptr<PlayerMap> player_maps, std::string attribute_key,
    bool ascending, std::shared_ptr<std::deque<std::string>> global_msgs)
    : player_maps(player_maps), attribute_key(attribute_key),
      ascending(ascending), global_msgs(global_msgs) {
}

RuleStatus Scores::execute(ElementMap& game_state) {
    LOG(INFO) << "* Scores Rule *";
    std::stringstream msg;
    msg << "\nScores are " << (ascending? "(in ascending order)\n" : "(in descending order)\n");

    std::vector<std::pair<int, uintptr_t>> scores;
    for (auto& [player_connection, player_list]: *player_maps) {
        scores.emplace_back(
            player_list->getMapElement(attribute_key)->getInt(), 
            player_connection.id
        );
    }
    
    std::sort(scores.begin(), scores.end(), [=](auto a, auto b){ return (a.first<b.first && ascending); });
    for (auto& [score, player_id]: scores) {
        msg << "player " << player_id << ": " << score << "\n";
    }

    global_msgs->push_back(msg.str());
    return RuleStatus::Done;
}