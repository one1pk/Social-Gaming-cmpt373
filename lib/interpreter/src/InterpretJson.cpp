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

    //registerListsToChai(game);

    //convert ElementSptr _rules_from_json to rule vector containing rule objects _rules
    //toRuleVec(game, game.rules_from_json(), game.rules());
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

    std::shared_ptr<PlayerMap> player_map = std::make_shared<PlayerMap>(PlayerMap{});
    std::shared_ptr<std::deque<Message>> player_msg_list = std::make_shared<std::deque<Message>>();
    std::shared_ptr<std::deque<std::string>> global_msg_list = std::make_shared<std::deque<std::string>>();
    std::shared_ptr<std::map<Connection, std::string>> player_input_list = std::make_shared<std::map<Connection, std::string>>();
    std::shared_ptr<PlayerMap> audience_list = std::make_shared<PlayerMap>(PlayerMap{});

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
            rule_object = std::make_shared<GlobalMsg>(msg, global_msg_list);
        }

        else if(ruleName == "parallelfor"){
            RuleVector sub_rules;
            toRuleVec(game, rule->getMapElement("rules"), sub_rules);
            rule_object = std::make_shared<ParallelFor>(player_map, sub_rules);
        }
        else if(ruleName == "input-choice"){
            auto prompt = rule->getMapElement("prompt")->getString();
            auto choices = resolveName(game, rule->getMapElement("choices")->getString())->getVector();
            auto result = rule->getMapElement("result")->getString();
            auto timeout = rule->getMapElement("timeout")->getInt();
            rule_object = std::make_shared<InputChoice>(prompt, choices, timeout, result, player_msg_list, player_input_list);
        }
        else if (ruleName == "add"){
            auto to = rule->getMapElement("to")->getString();
            auto value = rule->getMapElement("value");
            rule_object = std::make_shared<Add>(to, value);
        }
        else if (ruleName == "scores"){
            auto score = rule->getMapElement("score")->getString();
            auto ascending = rule->getMapElement("ascending")->getBool();
            rule_object = std::make_shared<Scores>(player_map, score, ascending, global_msg_list);
        }
        rule_vec.push_back(rule_object);
    }
    
    game.setExternalLists(player_map, audience_list, player_msg_list, global_msg_list, player_input_list);
    
}


//Register
/*using namespace chaiscript;
void InterpretJson::registerListsToChai(Game& game){
    ElementSptr constants = game.constants();
    ElementSptr variables = game.variables();
    ElementSptr setup = game.setup();
    ElementSptr per_audience = game.per_audience();
    ElementSptr per_player = game.per_player();
    chai.add(var(constants), "constants");
    chai.add(var(variables), "variables");
    chai.add(var(setup), "setup");
    chai.add(var(per_player), "per_player");
    chai.add(var(per_audience), "per_audience");
}


    */

