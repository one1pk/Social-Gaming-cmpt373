#include "rules.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include "TreePrinter.h"
#include "ExpressionTree.h"
// Foreach //

Foreach::Foreach(std::shared_ptr<ASTNode> listExpressionRoot, RuleVector _rules, std::string elementName)
    : listExpressionRoot(listExpressionRoot), rules(_rules), elementName(elementName) {
}

bool Foreach::executeImpl(ElementSptr, ElementMap elementsMap) {
    std::cout << "* Foreach Rule *\n";

    // initialze the elements vector from the dynamic list object
    if (!initialized) {
        listExpressionRoot->accept(resolver, elementsMap);
        list = resolver.getResult();
        elements = list->getVector();
        element = elements.begin();
        rule = rules.begin();
        initialized = true;
    }

    // execute the child rules for each element
    for (; element != elements.end(); element++) {
        //sets element in elementMap so that visitors in subrules can lookup this element (eg. round, weapon, etc)
        elementsMap[elementName] = *element;
        for (; rule != rules.end(); rule++) {
            if (!(*rule)->execute(elementsMap, *element)) {
                return false;
            }
            // reset the rule (to be executed by next element)
            (*rule)->reset();
        }
        rule = rules.begin();
    }
    executed = true;
    return true;
}

void Foreach::resetImpl() {
    initialized = false;
}

// ParallelFor //

ParallelFor::ParallelFor(std::shared_ptr<PlayerMap> player_maps, RuleVector rules, std::string elementName)
    : player_maps(player_maps), rules(rules), elementName(elementName) {
}

bool ParallelFor::executeImpl(ElementSptr element, ElementMap elementsMap) {
    std::cout << "* ParallelFor Rule *\n";

    // initialze the player rule iterators to the first rule
    if (!initialized) {
        for (auto player_map: *player_maps) {
            player_rule_it[player_map.first] = rules.begin();
        }
        initialized = true;
    }

    executed = true;
    // for each player, start rule execution at their stored rule iterator
    // for each player, execute the rules until an InputRequest rule is encountered
    for(auto player_map = player_maps->begin(); player_map != player_maps->end(); player_map++) {
        //sets elementsMap at "player" to ElementSptr player so visitors of sub rules can lookup this element
        elementsMap[elementName] = player_map->second;
        for (auto& rule = player_rule_it[player_map->first]; rule != rules.end(); rule++) {
            if (!(*rule)->execute(elementsMap, player_map->second)) {
                // InputRequest rule encountered
                executed = false;
                break;
            }
            (*rule)->reset(); // reset the rule (to be executed by the next player)
        }
    }
    return executed;
}

void ParallelFor::resetImpl() {
    initialized = false;
}

// When //

void When::set(){
    condition_rule_pair = condition_rule_pairs.begin();
    rule = condition_rule_pair->second.begin(); 
}

bool When::executeImpl(ElementSptr element, ElementMap elementsMap) {
    std::cout << "* When Rule *\n";
    
    // traverse the cases and execute the rules for the case condition that returns true
    // a case_rule_pair consists of a case condition (a lambda returning bool) and a rule vector
  
        
    
    for (; condition_rule_pair != condition_rule_pairs.end(); condition_rule_pair++) {
        rule = condition_rule_pair->second.begin();
        conditionRoot = condition_rule_pair->first;
        conditionRoot->accept(resolver, elementsMap);

        
        bool caseMatch = resolver.getResult()->getBool();
    

    
        if (caseMatch) {
            std::cout << "Case Match!\nExecuting Case Rules\n";
            
            for (; rule != condition_rule_pair->second.end(); rule++) {

                if (!(*rule)->execute(elementsMap, element)) {
                    return false;
                }
                (*rule)->reset();
            }
            break;
        } else {
            std::cout << "Case Fail, testing next case\n";
        }
    }

    executed = true;
    return true;
}

void When::resetImpl() {
    condition_rule_pair = condition_rule_pairs.begin();
    rule = condition_rule_pair->second.begin();
}

// Extend //

Extend::Extend(std::shared_ptr<ASTNode> targetExpressionRoot, std::shared_ptr<ASTNode> extensionExpressionRoot)
    : targetExpressionRoot(targetExpressionRoot), extensionExpressionRoot(extensionExpressionRoot){
}

bool Extend::executeImpl(ElementSptr element, ElementMap elementsMap) {
    std::cout << "* Extend Rule *\n";
    targetExpressionRoot->accept(resolver, elementsMap);
    target = resolver.getResult();

    extensionExpressionRoot->accept(resolver, elementsMap);
    extensionList = resolver.getResult();
    
    target->extend(extensionList);

    executed = true;
    return true;
}

// Discard //

Discard::Discard(std::shared_ptr<ASTNode> listExpressionRoot,  std::shared_ptr<ASTNode> countExpressionRoot)
: listExpressionRoot(listExpressionRoot), countExpressionRoot(countExpressionRoot){
}

bool Discard::executeImpl(ElementSptr element, ElementMap elementsMap) {
    std::cout << "* Discard Rule *\n";

    listExpressionRoot->accept(resolver, elementsMap);
    list = resolver.getResult();

    countExpressionRoot->accept(resolver, elementsMap);
    resolvedCount = resolver.getResult()->getInt();

    list->discard(resolvedCount);
    executed = true;
    return true;
}

// Add //

Add::Add(std::shared_ptr<ASTNode> toExpressionRoot, ElementSptr value)
    : toExpressionRoot(toExpressionRoot), value(value) {
}

bool Add::executeImpl(ElementSptr element, ElementMap elementsMap) {
    std::cout << "* Add Rule *\n";
    toExpressionRoot->accept(resolver, elementsMap);
    to = resolver.getResult();
    to->addInt(value->getInt());
    executed = true;
    return true;
}

// InputChoice //

std::string formatString(std::string_view msgView, std::shared_ptr<ASTNode> elementToReplace, ExpressionResolver& resolver, ElementMap elementsMap) {
    std::string msg(msgView);
    size_t open_brace = 0; 
    
    if ((open_brace = msg.find("{", open_brace)) != std::string::npos) {
        size_t close_brace = msg.find("}", open_brace);
        elementToReplace->accept(resolver, elementsMap);
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
    return msg;
}

InputChoice::InputChoice(std::string prompt, std::shared_ptr<ASTNode> elementToReplace, std::shared_ptr<ASTNode> choicesExpressionRoot, 
                unsigned timeout_s, std::string result,
                std::shared_ptr<std::deque<Message>> player_msgs,
                std::shared_ptr<std::map<Connection, std::string>> player_input)
    : prompt(prompt), elementToReplace(elementToReplace), choicesExpressionRoot(choicesExpressionRoot), timeout_s(timeout_s),
      result(result), player_msgs(player_msgs), player_input(player_input) {
}

bool InputChoice::executeImpl(ElementSptr player, ElementMap elementsMap) {
    std::cout << "* InputChoiceRequest Rule *\n";
    
   
    choicesExpressionRoot->accept(resolver, elementsMap);
    choices = resolver.getResult()->getVector();
    Connection player_connection = player->getMapElement("connection")->getConnection();

    if (!alreadySentInput[player_connection]) {
        std::string formattedMsg = formatString(prompt, elementToReplace, resolver, elementsMap);
        std::stringstream msg(formattedMsg);
        msg << msg.str() << std::endl << "Enter an index to select:\n";
        for (size_t i = 0; i < choices.size(); i++) {
            msg << "["<<i<<"] " << choices[i]->getString() << "\n";
        }
        player_msgs->push_back({ player_connection, msg.str() });

        // returning false indicates that input is needed from the user
        alreadySentInput[player_connection] = true;
        return false;
    }
    // execution will continue from here after input is recieved

    /// TODO: check if the selected index is within range, if not ask user to provide a valid index
    /// TODO: add player element to players list
    int chosen_index = std::stoi(player_input->at(player_connection));
    
    player->setMapElement(result, choices[chosen_index]);

    alreadySentInput[player_connection] = false;
    return true;
}


// GlobalMsg //

GlobalMsg::GlobalMsg(std::string msg, std::shared_ptr<std::deque<std::string>> global_msgs, std::shared_ptr<ASTNode> elementToReplace)
    : msg(msg), global_msgs(global_msgs), elementToReplace(elementToReplace) {
}

bool GlobalMsg::executeImpl(ElementSptr element, ElementMap elementsMap) {
    std::cout << "* GlobalMsg Rule *\n";
    std::string formattedMsg = formatString(msg, elementToReplace, resolver, elementsMap);
    formattedMsg += "\n";
    global_msgs->push_back(formattedMsg);
    executed = true;
    return true;
}

// Scores //

Scores::Scores(std::shared_ptr<PlayerMap> player_maps, std::string attribute_key,
    bool ascending, std::shared_ptr<std::deque<std::string>> global_msgs)
    : player_maps(player_maps), attribute_key(attribute_key),
      ascending(ascending), global_msgs(global_msgs) {
}

bool Scores::executeImpl(ElementSptr element, ElementMap elementsMap) {
    std::cout << "* Scores Rule *\n";

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
    executed = true;
    return true;
}