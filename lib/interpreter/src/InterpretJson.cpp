#include <fstream>
#include <iostream>
#include "InterpretJson.h"
#include "game.h"
#include <algorithm>

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
    //create rule vector _rules from elementSptr _rules_from_json

    //convert ElementSptr _rules_from_json to rule vector containing rule objects _rules
    toRuleVec(game, game.rules_from_json(), game.rules());
}


//Interpret Rules

ElementSptr InterpretJson::resolveName(Game& game, std::string name){
    ElementSptr constants = game.constants();
    ElementSptr setup = game.setup();
    
    if(MACRO_VARIABLE_TO_STRING(constants) == name){
        return game.constants();
    }
    else if(MACRO_VARIABLE_TO_STRING(setup) == name){
        return setup->getMapElement("Rounds")->upfrom(1);
    }
    return nullptr;
}

void InterpretJson::toRuleVec(Game& game, const ElementSptr& rules_from_json, RuleVector& rule_vec) {

    std::shared_ptr<PlayerMap> player_list = std::make_shared<PlayerMap>(PlayerMap{});
    std::shared_ptr<std::deque<Message>> player_msg_list = std::make_shared<std::deque<Message>>();
    std::shared_ptr<std::deque<std::string>> global_msg_list = std::make_shared<std::deque<std::string>>();
    std::shared_ptr<std::map<Connection, std::string>> player_input_list = std::make_shared<std::map<Connection, std::string>>();
    std::shared_ptr<PlayerMap> audience_list = std::make_shared<PlayerMap>(PlayerMap{});

    for (auto rule: rules_from_json->getVector()){
        std::string rule_name = rule->getMapElement("rule")->getString();
        RuleSptr rule_object;

        if(rule_name == "foreach"){
            ElementSptr list = resolveName(game, rule->getMapElement("list")->getString());
            RuleVector sub_rules;
            toRuleVec(game, rule->getMapElement("rules"), sub_rules);
            rule_object = std::make_shared<Foreach>(list, sub_rules);
            rule_vec.push_back(rule_object);
        }
        else if(rule_name == "global-message"){
            auto msg = rule->getMapElement("value")->getString();
            rule_object = std::make_shared<GlobalMsg>(msg, global_msg_list);
            rule_vec.push_back(rule_object);
        }
    }
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

std::map lists = std::map<std::string, ElementSptr>{{"constants", game.constants()},
             {"variables", game.variables()},
             {"setup", game.setup()},
             {"per-player", game.per_player()},
             {"per-audience", game.per_audience()}};
    auto location = std::find(lists.begin(), lists.end(), name);
    return
    */

