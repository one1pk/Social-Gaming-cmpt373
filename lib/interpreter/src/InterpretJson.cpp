#include <fstream>
#include <iostream>
#include "InterpretJson.h"
#include "game.h"
#include <algorithm>
#include <cctype>

#define MACRO_VARIABLE_TO_STRING(Variable) (void(Variable), #Variable)

using namespace std;
using Json = nlohmann::json;

InterpretJson::InterpretJson(string path){
    try{
    ifstream f(path);
    Json jData = Json::parse(f);
    f.close();
    data = jData;
    } catch (std::exception& e){
        cout << "error reading file" << e.what() << endl;
    }
}

InterpretJson::InterpretJson(Json j){
    data = j;
}

Json InterpretJson::getData(){
    return data;
}

void InterpretJson::interpret(Game& game){
    game = data.get<Game>();
}

void InterpretJson::interpretWithRules(Game& game){
    game = data.get<Game>();

    ElementSptr rulesFromJson;
    data.at("rules").get_to(rulesFromJson);
    //convert ElementSptr _rules_from_json to rule vector containing rule objects _rules
    RuleVector rules;
    toRuleVec(game, rulesFromJson, rules);
    game.setRules(rules);

    game.setID();
    game.setName("TestGame");
    game.setStatusCreated();
}


//Interpret Rules
std::vector<std::string> InterpretJson::splitString(const std::string& listName) {
    int pos = 0;
    bool alnum = false;
    std::vector<std::string> splits;
    splits.reserve(listName.length());

    for (unsigned i = 0; i < listName.length(); i++) {
        if (isalnum(listName[i])) {
            if (!alnum) {
                pos = i;
                alnum = true;
            }
        }  else {
            if (alnum) {
                splits.emplace_back(listName.substr(pos, i-pos));
                alnum = false;
            }
        }
    }
    if (alnum) {
        splits.emplace_back(listName.substr(pos, listName.length()-pos));
    }

    return splits;
}


ElementSptr InterpretJson::resolveName(Game& game, const std::string& listName){
    std::vector<std::string> listNames = splitString(listName);

    ElementSptr constants = game.constants();
    ElementSptr setup = game.setup();
    ElementSptr variables = game.variables();
    ElementSptr per_player = game.per_player();
    ElementSptr per_audience = game.per_audience();

    std::map<std::string, ElementSptr> listMap = std::map<std::string, ElementSptr>{{"constants", constants},
        {"variables", variables},
        {"setup", setup},
        {"per-player", per_player},
        {"per-audience", per_audience}};
    
    ElementSptr actualList = listMap.at(listNames[0]);

    //TODO: Create abstract syntax tree for operations instead of using if statements
    int i;
    bool isOperation = false;
    std::string prevName;
    for(i = 1; i < listNames.size(); i++){
        auto name = listNames[i];
        if(listNames[i] == "upfrom" || listNames[i] == "sublist"){
            isOperation = true;
            prevName = listNames[i];
            continue;
        }
        else if(listNames[i] == "size"){
            ElementSptr size = std::make_shared<Element<int>>(actualList->getSizeAsInt());
            actualList.swap(size);
        }
        else if(isOperation == true){
            if(prevName == "upfrom"){
                int start = stoi(listNames[i]);
                ElementSptr upfromList = actualList->upfrom(start);
                actualList.swap(upfromList);
                isOperation = false;
            }
            else if(prevName == "sublist"){
                ElementSptr subList = std::make_shared<Element<ElementVector>>(actualList->getSubList(listNames[i]));
                actualList.swap(subList);
                isOperation = false;
            }
        }
        else{
            ElementSptr deeperElement = actualList->getMapElement(listNames[i]);
            actualList.swap(deeperElement);
        }
    }
    return actualList;
}



void InterpretJson::toRuleVec(Game& game, const ElementSptr& rules_from_json, RuleVector& rule_vec) {

    for (auto rule: rules_from_json->getVector()){
        std::string ruleName = rule->getMapElement("rule")->getString();
        RuleSptr rule_object;

        if(ruleName == "foreach"){
            std::string listName = rule->getMapElement("list")->getString();
            ElementSptr list = resolveName(game, listName);
            RuleVector sub_rules;
            toRuleVec(game, rule->getMapElement("rules"), sub_rules);
            rule_object = std::make_shared<Foreach>(list, sub_rules);
            
        }
        else if(ruleName == "global-message"){
            auto msg = rule->getMapElement("value")->getString();
            rule_object = std::make_shared<GlobalMsg>(msg, game._global_msgs);
        }

        else if(ruleName == "parallelfor"){
            RuleVector sub_rules;
            toRuleVec(game, rule->getMapElement("rules"), sub_rules);
            rule_object = std::make_shared<ParallelFor>(game._players, sub_rules);
        }
        else if(ruleName == "input-choice"){
            auto prompt = rule->getMapElement("prompt")->getString();
            auto choices = resolveName(game, rule->getMapElement("choices")->getString())->getVector();
            auto result = rule->getMapElement("result")->getString();
            auto timeout = rule->getMapElement("timeout")->getInt();
            rule_object = std::make_shared<InputChoice>(prompt, choices, timeout, result, game._player_msgs, game._player_input);
        }
        else if (ruleName == "add"){
            auto to = rule->getMapElement("to")->getString();
            auto value = rule->getMapElement("value");
            rule_object = std::make_shared<Add>(to, value);
        }
        else if (ruleName == "scores"){
            auto score = rule->getMapElement("score")->getString();
            auto ascending = rule->getMapElement("ascending")->getBool();
            rule_object = std::make_shared<Scores>(game._players, score, ascending, game._global_msgs);
        }
        rule_vec.push_back(rule_object);
    }
}


